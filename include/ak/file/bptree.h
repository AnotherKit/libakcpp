#ifndef AK_LIB_FILE_BPTREE_H_
#define AK_LIB_FILE_BPTREE_H_

#include <unistd.h>
#include <string.h>

#include <algorithm>
#include <compare>
#include <list>
#include <optional>
#include <vector>

#include "array.h"
#include "file.h"
#include "set.h"

#ifdef AK_DEBUG
#include <iostream>
#endif

namespace ak::file {
template <typename T>
concept BptStorable = requires(T a, T b) {
  { a <=> b } -> std::convertible_to<std::weak_ordering>;
  { a == b } -> std::same_as<bool>;
  { a != b } -> std::same_as<bool>;
  std::copy_constructible<T>;
};
/**
 * an implementation of the B+ tree. It stores key and value together in order to support duplicate keys.
 *
 * constraints: KeyType and ValueType need to be comparable.
 *
 * why default szNode = 512: excerpt of `sudo fdisk -l` on my machine:
 *   Disk /dev/nvme1n1: 1.82 TiB, 2000398934016 bytes, 3907029168 sectors
 *   Disk model: WD_BLACK  SN750 2TB
 *   Units: sectors of 1 * 512 = 512 bytes
 *   Sector size (logical/physical): 512 bytes / 512 bytes
 *   I/O size (minimum/optimal): 512 bytes / 512 bytes
*/
template <BptStorable KeyType, BptStorable ValueType, size_t szNode = 512> // NOLINT(readability-magic-numbers): explained above.
class BPTree {
 private:
  File<szNode> file_;
  void init_ () {
    // set the 0th node to the root node
    IndexNode root;
    root.leaf = true;
    root.id = file_.push(root);
    AK_ASSERT(root.id == 0);
  }
  /// store key and value together to support dupe keys. this is the structure that is actually stored.
  struct Payload {
    KeyType key;
    ValueType value;
    std::weak_ordering operator<=> (const Payload &that) const {
      if (key != that.key) return key <=> that.key;
      return value <=> that.value;
    }
    bool operator== (const Payload &) const = default;
    bool operator!= (const Payload &) const = default;
  };
  /// compares a Payload and a KeyType that key alone is greater than all payloads with this key
  class KeyComparator_ {
   public:
    bool operator() (const Payload &lhs, const KeyType &rhs) { return lhs.key <= rhs; }
    bool operator() (const KeyType &lhs, const Payload &rhs) { return lhs < rhs.key; }
  };
  /// compares a Payload and a KeyType that key alone is less than all payloads with this key
  class KeyComparatorLess_ {
   public:
    bool operator() (const Payload &lhs, const KeyType &rhs) { return lhs.key < rhs; }
    bool operator() (const KeyType &lhs, const Payload &rhs) { return lhs <= rhs.key; }
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
    NodeId id;
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
    DataNode (const Serialized &serialized, File<szNode> *file, size_t id_) {
      // not using designated initializer to prevent memcpy from overwriting id and file
      memcpy(this, &serialized, sizeof(serialized));
      file_ = file;
      id = id_;
    }

    DataNode () = default;
    explicit DataNode (NodeId id) : id(id) {}

    void insert (const Payload &entry) {
      entries.insert(entry);
      if (entries[0] == entry) file_->template get<IndexNode>(parent).updateSplit(id, entry);
      if (entries.length == 2 * l) split_();
      AK_ASSERT(entries.length < 2 * l);
      file_->set(id, *this);
    }
    void remove (const Payload &entry) {
      if (entries[0] == entry && entries.length > 1) {
        file_->template get<IndexNode>(parent).updateSplit(id, entries[1]);
      }
      entries.remove(entry);
      if (entries.length < l) merge_();
      file_->set(id, *this);
    }
    std::optional<ValueType> findOne (const KeyType &key) {
      size_t ix = std::upper_bound(entries.content, entries.content + entries.length, key, KeyComparator_()) - entries.content;
      if (ix == 0) return std::nullopt;
      Payload payload = entries[ix - 1];
      if (payload.key != key) return std::nullopt;
      return payload.value;
    }
    bool includes (const Payload &entry) {
      return entries.includes(entry);
    }
    std::list<ValueType> findMany (const KeyType &key) {
      size_t start = std::upper_bound(entries.content, entries.content + entries.length, key, KeyComparatorLess_()) - entries.content;
      size_t end = std::upper_bound(entries.content, entries.content + entries.length, key, KeyComparator_()) - entries.content;
      std::list<ValueType> result;
      for (size_t i = start; i < end; ++i) if (entries[i].key == key) result.push_back(entries[i].value);
      return result;
    }

#ifdef AK_DEBUG
    void print () {
      std::cerr << "Data Node " << id << "; parent " << parent << "; entry count " << entries.length << '\n';
    }
#endif

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
    IndexNode (const Serialized &serialized, File<szNode> *file, size_t id_) {
      memcpy(this, &serialized, sizeof(serialized));
      file_ = file;
      id = id_;
    }

    IndexNode () = default;

    void insert (const Payload &entry) {
      if (std::string(entry.key) == "c0gas") {
        std::cerr << "233";
      }
      // if this is the first entry of the root, go create a data node.
      if (children.length == 0) {
        AK_ASSERT(isRoot_());
        AK_ASSERT(leaf);
        DataNode child;
        child.entries.insert(entry);
        child.parent = id;
        child.id = file_->push(child);
        children.push(child.id);
        splits.insert(entry);
        file_->set(id, *this);
        return;
      }
      // we first find the position for the entry to be inserted, then insert it into the position, and split recursively starting from the data node.
      size_t ix = findPos_(entry);
      if (entry < splits[ix]) splits[ix] = entry;
      if (leaf) file_->template get<DataNode>(children[ix]).insert(entry);
      else file_->template get<IndexNode>(children[ix]).insert(entry);
    }
    /// caller must guarantee that entry exists in the tree
    void remove (const Payload &entry) {
      size_t ix = findPos_(entry);
      if (!leaf) {
        file_->template get<IndexNode>(children[ix]).remove(entry);
        return;
      }
      DataNode data = file_->template get<DataNode>(children[ix]);
      data.remove(entry);
      // destroy the only empty data node
      if (isRoot_() && data.entries.length == 0) {
        children.length = splits.length = 0;
        file_->remove(data.id);
      }
    }
    std::optional<ValueType> findOne (const KeyType &key) {
      size_t ix = std::upper_bound(splits.content, splits.content + splits.length, key, KeyComparator_()) - splits.content;
      if (ix > 0) --ix;
      if (leaf) return file_->template get<DataNode>(children[ix]).findOne(key);
      return file_->template get<IndexNode>(children[ix]).findOne(key);
    }
    bool includes (const Payload &entry) {
      size_t ix = findPos_(entry);
      if (leaf) return file_->template get<DataNode>(children[ix]).includes(entry);
      return file_->template get<IndexNode>(children[ix]).includes(entry);
    }
    std::list<ValueType> findMany (const KeyType &key) {
      size_t start = std::upper_bound(splits.content, splits.content + splits.length, key, KeyComparatorLess_()) - splits.content;
      size_t end = std::upper_bound(splits.content, splits.content + splits.length, key, KeyComparator_()) - splits.content;
      std::list<ValueType> result;
      for (size_t i = start; i < end; ++i) {
        if (leaf) result.merge(file_->template get<DataNode>(children[i]).findMany(key));
        else result.merge(file_->template get<IndexNode>(children[i]).findMany(key));
      }
      return result;
    }

    void updateSplit (NodeId id1, const Payload &value) {
      size_t index = children.indexOf(id1);
      splits[index] = value;
      if (index == 0 && !isRoot_()) file_->template get<IndexNode>(parent).updateSplit(id, value);
      file_->set(id, *this);
    }

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
        } else {
          left.leaf = right.leaf = false;
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
          children.unshift(prev.children.pop());
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

        // update parent node
        parentNode.splits[parentNode.children.indexOf(id)] = splits[0];
        parentNode.removeChild(prev.id);
        return;
      }
      IndexNode next = file_->template get<IndexNode>(parentNode.children[ix - 1]);
      if (next.children.length > k) {
        children.push(next.children.shift());
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

    void removeChild (NodeId id1) {
      size_t ix = children.indexOf(id1);
      children.removeAt(ix);
      splits.removeAt(ix);
      mergeIfNeeded();
      // don't save if wiped out
      if (children.length > 0) file_->set(id, *this);
    }

#ifdef AK_DEBUG
    void print () {
      std::cerr << "Index Node " << id << "; parent " << parent << "; leaf " << leaf << "\nChildren:\n";
      children.forEach([] (NodeId i) { std::cerr << i << '\t'; });
      std::cerr << "\n\n";
      if (leaf) children.forEach([this] (NodeId i) { file_->template get<DataNode>(i).print(); });
      else children.forEach([this] (NodeId i) { file_->template get<IndexNode>(i).print(); });
    }
#endif

   private:
    bool isRoot_ () { return id == 0; }
    void destroy_ () {
      AK_ASSERT(children.length == 0);
      file_->remove(id);
      file_->template get<IndexNode>(parent).removeChild(id);
    }
    void mergeRoot_ () {
      AK_ASSERT(isRoot_());
      AK_ASSERT(children.length == 1); // root is only merged from its only child
      AK_ASSERT(!leaf); // leaf root need not be merged
      IndexNode child = file_->template get<IndexNode>(children[0]);
      memcpy(this, &child, offsetof(IndexNode, id));
      parent = id; // i.e. 0
    }
    /// find the position of entry recursively. entry may and may not exist in the tree.
    size_t findPos_ (const Payload &entry) {
      size_t ix = std::upper_bound(splits.content, splits.content + splits.length, entry) - splits.content;
      return ix == 0 ? ix : ix - 1;
    }
  };

  IndexNode getRoot_ () { return file_.template get<IndexNode>(0); }
 public:
  BPTree () = delete;
  BPTree (const char *filename) : file_(filename, [this] () { this->init_(); }) {}
  void insert (const KeyType &key, const ValueType &value) { getRoot_().insert({ .key = key, .value = value }); }
  void remove (const KeyType &key, const ValueType &value) { getRoot_().remove({ .key = key, .value = value }); }
  std::optional<ValueType> findOne (const KeyType &key) { return getRoot_().findOne(key); }
  std::vector<ValueType> findMany (const KeyType &key) {
    std::list<ValueType> l = getRoot_().findMany(key);
    return std::vector(l.begin(), l.end());
  }
  bool includes (const KeyType &key, const ValueType &value) { return getRoot_().includes({ .key = key, .value = value }); }

  void clearCache () { file_.clearCache(); }

#ifdef AK_DEBUG
  void print () { getRoot_().print(); }
#endif
}; // class BPTree

} // namespace ak::file

#endif
