#ifndef AK_LIB_FILE_BPTREE_H_
#define AK_LIB_FILE_BPTREE_H_

#include <unistd.h>
#include <string.h>

#include <algorithm>
#include <optional>
#include <vector>

#include "array.h"
#include "file.h"
#include "set.h"

namespace ak::file {
/**
 * an implementation of the B+ tree. It stores key and value together in order to support duplicate keys.
 *
 * constraints: KeyType and ValueType need to be comparable (i.e. bool operator< (const {Key,Value}Type &, const {Key,Value}Type&) need to be defined.)
 *
 * why default szNode = 512: excerpt of `sudo fdisk -l` on my machine:
 *   Disk /dev/nvme1n1: 1.82 TiB, 2000398934016 bytes, 3907029168 sectors
 *   Disk model: WD_BLACK  SN750 2TB
 *   Units: sectors of 1 * 512 = 512 bytes
 *   Sector size (logical/physical): 512 bytes / 512 bytes
 *   I/O size (minimum/optimal): 512 bytes / 512 bytes
*/
template <typename KeyType, typename ValueType, size_t szNode = 512> // NOLINT(readability-magic-numbers): explained above.
class BPTree {
 private:
  File<szNode> file_;
  void init_ () {
    // set the 0th node to the root node
    file_.push(IndexNode());
  }
  /// store key and value together to support dupe keys. this is the structure that is actually stored.
  struct Payload {
    KeyType key;
    ValueType value;
    bool operator< (const Payload &that) const {
      if (key != that.key) return key < that.key;
      return value < that.value;
    }
  };
  /// nodes are referenced by its numerical id.
  using NodeId = unsigned int;
  /**
   * a data node has at least l and at most 2l-1 records.
   * this kind of node is called a "record" on Wikipedia.
   */
  struct DataNode {
    static constexpr size_t l = (szNode - 3 * sizeof(NodeId)) / sizeof(Payload) / 2;
    // l=1 is insane. we need at least 2 branches to be a tree.
    static_assert(l >= 2);
    /// previous and next data nodes, 0 means it does not exist
    NodeId prev = 0, next = 0;
    Set<Payload, 2 * l> entries;
    NodeId parent = 0;

    // these fields are not stored, and need to be initialized on deserialization.
    NodeId id = 0;
    File<szNode> *file_;

    struct Serialized {
      // we need these to make sizeof() work
      NodeId prev = 0, next = 0;
      Set<Payload, 2 * l> entries;
      NodeId parent = 0;
      Serialized () = default;
      Serialized (const DataNode &node) { memcpy(this, &node, sizeof(*this)); }
    };
    static_assert(sizeof(Serialized) <= szNode);
    DataNode (const Serialized &serialized, File<szNode> *file, size_t id) : file_(file), id(id) { memcpy(this, &serialized, sizeof(serialized)); }

    DataNode () = default;
    explicit DataNode (NodeId id) : id(id) {}

    void insert (const Payload &entry) {
      entries.insert(entry);
      if (entries.length == 2 * l) split_();
      AK_ASSERT(entries.length < 2 * l);
      file_->set(id, *this);
    }
    void remove (const Payload &entry) {
      size_t pos = entries.indexOf(entry);
      if (pos != entries.length - 1) memmove(&entries.content[pos], &entries.content[pos + 1], sizeof(entries[0]) * (entries.length - pos - 1));
      if (entries.length < l) merge_();
      file_->set(id, *this);
    }
    std::optional<ValueType> findOne (const KeyType &key) {}
    std::vector<ValueType> findMany (const KeyType &key) {}
   private:
    void split_ () {
      AK_ASSERT(entries.length == 2 * l);

      // create a new data node
      DataNode newNode;
      newNode.prev = id;
      newNode.next = next;
      entries.length = newNode.entries.length = l;
      memmove(newNode.entries.content, &entries.content[l], l * sizeof(entries.content[0]));
      newNode.parent = parent;
      next = newNode.id = file_->push(newNode);

      // update the parent index node
      IndexNode inode = file_->template get<IndexNode>(parent);
      inode.children.insert(next, inode.children.indexOf(id) + 1);
      inode.splits.insert(newNode.entries[0]);
      inode.splitIfNeeded();
      file_->set(inode.id, inode);
    }
    void destroy_ () {
      AK_ASSERT(entries.length == 0);
      file_->remove(id);
      file_->template get<IndexNode>(parent).removeChild(id);
    }
    void merge_ () {
      AK_ASSERT(entries.length == l - 1);
      if (next == 0) {
        // do not do anything to the only node.
        if (prev == 0) return;
        DataNode prevNode = file_->template get<DataNode>(prev);
        // if there are more than l entries in the previous node, insert the last one.
        if (prevNode.entries.length > l) {
          entries.insert(prevNode.entries.pop());
          return;
        }
        // we now have prevNode.entries: [b[0],...,b[l-1]] and entries: [a[0]...a[l-2]], want entries: [b[0],...,b[l-1],a[0],...,a[l-2]]
        entries.copyFrom(entries, 0, l, l - 1);
        entries.copyFrom(prevNode.entries, 0, 0, l);
        entries.length += prevNode.entries.length;
        prevNode.entries.length = 0;
        // now the previous node is totally unused. discard it.
        prevNode.destroy_();

        // update parent
        file_->template get<IndexNode>(prevNode.parent).removeChild(prevNode.id);
      } else {
        DataNode nextNode = file_->template get<DataNode>(next);
        if (nextNode.entries.length > l) {
          entries.insert(nextNode.entries.shift());
          return;
        }
        entries.copyFrom(nextNode.entries, 0, l, l);
        entries.length += nextNode.entries.length;
        nextNode.entries.length = 0;
        nextNode.destroy_();
        file_->template get<IndexNode>(nextNode.parent).removeChild(nextNode.id);
      }
    }
  };
  /**
   * an index node in a B+ tree has at least k and at most 2k-1 child nodes, unless it's the root.
   * if we have n child nodes, then we have n-1 split points, which are the lower bounds of the (i+1)th section.
   * this kind of node is called a "node" on Wikipedia.
   */
  struct IndexNode {
    static constexpr size_t k = (szNode - 3 * sizeof(NodeId)) / (sizeof(NodeId) + sizeof(Payload)) / 2;
    static_assert(k >= 2);
    bool leaf = false;
    /// for leaf nodes, childs are the indices of data nodes.
    Array<NodeId, 2 * k> children;
    Set<Payload, 2 * k> splits;
    /// id === parent <=> id === 0 <=> this is root
    NodeId parent = 0;

    // these fields are not stored, and need to be initialized on deserialization.
    NodeId id = 0;
    File<szNode> *file_;

    struct Serialized {
      bool leaf;
      Array<NodeId, 2 * k> children;
      Set<Payload, 2 * k> splits;
      NodeId parent;
      Serialized () = default;
      Serialized (const IndexNode &node) { memcpy(this, &node, sizeof(*this)); }
    };
    static_assert(sizeof(Serialized) <= szNode);
    IndexNode (const Serialized &serialized, File<szNode> *file, size_t id) : file_(file), id(id) { memcpy(this, &serialized, sizeof(serialized)); }

    IndexNode () = default;

    std::optional<IndexNode> insert (const Payload &entry) {
      // we first find the position for the entry to be inserted, then insert it into the position, and split recursively starting from the data node.
      // TODO
    }
    std::optional<IndexNode> remove (const Payload &entry) {}
    std::optional<ValueType> findOne (const KeyType &key) {}
    std::vector<ValueType> findMany (const KeyType &key) {}

    void splitIfNeeded () { if (children.length >= 2 * k) split(); }
    void split () {
      AK_ASSERT(children.length == 2 * k);
      if (isRoot_()) {
        // the split of the root node is a bit different from other nodes. it produces two extra subnodes.
        IndexNode left, right;

        // copy children and splits
        left.children.copyFrom(children, 0, 0, k);
        left.splits.copyFrom(splits, 0, 0, k);
        right.children.copyFrom(children, k, 0, k);
        right.splits.copyFrom(splits, k, 0, k);
        left.children.length = left.splits.length = right.children.length = right.splits.length = k;

        // set misc properties and save
        if (leaf) {
          leaf = false;
          left.leaf = right.leaf = true;
        }
        left.parent = right.parent = id; // i.e. 0
        left.id = file_->push(left);
        right.id = file_->push(right);

        // initiate the new root node
        children.length = splits.length = 0;
        children.insert(left.id, 0);
        children.insert(right.id, 1);
        splits.insert(left.splits[0]);
        splits.insert(right.splits[0]);
        return;
      }

      // create a new next node
      IndexNode next;
      next.children.copyFrom(children, k, 0, k);
      next.splits.copyFrom(splits, k, 0, k);
      splits.length = next.splits.length = children.length = next.children.length = k;
      next.parent = parent;
      next.leaf = leaf;
      next.id = file_->push(next);

      // update parent node
      IndexNode inode = file_->template get<IndexNode>(parent);
      size_t ix = inode.children.indexOf(id);
      inode.children.insert(next.id, ix + 1);
      inode.splits.insert(next.splits[0]);
      inode.splitIfNeeded();
      file_->set(inode.id, inode);
    }
    void mergeIfNeeded () {
      if (isRoot_()) {
        if (children.length == 1) mergeRoot_();
        return;
      }
      if (children.length < k) merge();
    }
    void merge () {
      AK_ASSERT(!isRoot_());
      AK_ASSERT(children.length < k);
      IndexNode parentNode = file_->template get<IndexNode>(parent);
      size_t ix = parentNode.children.indexOf(id);
      const bool hasPrev = ix != 0, hasNext = ix != parentNode.children.length - 1;
      if (!hasNext) {
        // all nodes has at least 2 child nodes, except if this is the root node.
        AK_ASSERT(hasPrev);
        IndexNode prev = file_->template get<IndexNode>(parentNode.children[ix - 1]);
        // if there are more than k children in the previous node, insert the last one.
        if (prev.children.length > k) {
          children.insert(prev.children.pop());
          splits.insert(prev.splits.pop());
          return;
        }

        // we now have prev: [b[0],...,b[l-1]] and this: [a[0]...a[l-2]], want this: [b[0],...,b[l-1],a[0],...,a[l-2]]
        // FIXME: find out a way to remove dupe code here
        if (prev.parent != parent) prev.children.forEach([this] (const NodeId &nodeId) {
          IndexNode node = file_->template get<IndexNode>(nodeId);
          node.parent = parent;
          file_->set(nodeId, node);
        });
        children.copyFrom(children, 0, k, k - 1);
        children.copyFrom(prev.children, 0, 0, k);
        children.length += prev.children.length;
        prev.children.length = 0;

        splits.copyFrom(splits, 0, k, k - 1);
        splits.copyFrom(prev.splits, 0, 0, k);
        splits.length += prev.splits.length;
        prev.splits.length = 0;

        // now the previous node is totally unused. discard it.
        prev.destroy_();
        parentNode.removeChild(prev.id);
        return;
      }
      IndexNode next = file_->template get<IndexNode>(parentNode.children[ix - 1]);
      if (next.children.length > k) {
        children.insert(next.children.shift());
        splits.insert(next.splits.shift());
        return;
      }

      // FIXME: remove dupe code here
      if (next.parent != parent) next.children.forEach([this] (const NodeId &nodeId) {
        IndexNode node = file_->template get<IndexNode>(nodeId);
        node.parent = parent;
        file_->set(nodeId, node);
      });
      children.copyFrom(next.children, 0, k, k);
      children.length += next.children.length;
      next.children.length = 0;

      splits.copyFrom(next.splits, 0, k, k);
      splits.length += next.splits.length;
      next.splits.length = 0;

      next.destroy_();
      file_->template get<IndexNode>(next.parent).removeChild(next.id);
    }

    void removeChild (NodeId id) {
      size_t ix = children.indexOf(id);
      children.removeAt(ix);
      splits.removeAt(ix);
      mergeIfNeeded();
      // don't save if wiped out
      if (children.length) file_->set(id, *this);
    }

   private:
    bool isRoot_ () { return id == 0; }
    void mergeRoot_ () {
      AK_ASSERT(isRoot_());
      IndexNode child = file_->template get<IndexNode>(children[0]);
      memcpy(this, &child, offsetof(IndexNode, id));
      parent = id; // i.e. 0
    }
  };
 public:
  BPTree () = delete;
  BPTree (const char *filename) : file_(filename, [this] () { this->init_(); }) {}
  void insert (const KeyType &key, const ValueType &value) {}
  void remove (const KeyType &key, const ValueType &value) {}
  std::optional<ValueType> findOne (const KeyType &key) {}
  std::vector<ValueType> findMany (const KeyType &key) {}
}; // class BPTree

} // namespace ak::file

#endif
