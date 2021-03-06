#ifndef AK_LIB_FILE_FILE_H_
#define AK_LIB_FILE_FILE_H_

#include <string.h>
#include <stddef.h>
#include <errno.h>
#include <sys/stat.h>

#include <fstream>
#include <functional>
#include <limits>
#include <unordered_map>

#include "ak/base.h"

namespace ak::file {
constexpr size_t kDefaultSzChunk = 4096;
/// a chunked file storage with manual garbage collection, with chunk size of szChunk and a cache powered by unordered_map.
template <size_t szChunk = kDefaultSzChunk>
class File {
 private:
  struct Metadata {
    size_t next;
    bool hasNext;
    Metadata () = default;
    Metadata (size_t next, bool hasNext) : next(next), hasNext(hasNext) {}
  };
  static_assert(szChunk > sizeof(Metadata));
  auto meta_ () -> Metadata {
    Metadata retval;
    get(&retval, -1, sizeof(retval));
    return retval;
  }
  auto offset_ (size_t index) -> size_t { return (index + 1) * szChunk; }
  std::fstream file_;
  std::unordered_map<size_t, char *> cache_;
  auto putCache_ (const void *buf, size_t index, size_t n) -> void {
    char *cache = new char[n];
    memcpy(cache, buf, n);
    if (cache_.count(index) > 0) delete[] cache_[index];
    cache_[index] = cache;
  }
 public:
  File () = delete;
  File (const char *filename, const std::function<void (void)> &initializer) {
    struct stat _st;
    bool shouldCreate = stat(filename, &_st) != 0;
    if (shouldCreate) {
      if (errno != ENOENT) throw IOException("File::init_: Unable to stat");
      file_.open(filename, std::ios_base::out);
      file_.close();
    }
    file_.open(filename);
    if (!file_.is_open() || !file_.good()) throw IOException("Unable to open file");
    if (shouldCreate) {
      Metadata meta(0, false);
      set(&meta, -1, sizeof(meta));
      initializer();
    }
  }
  ~File () { clearCache(); }

  /// read n bytes at index into buf.
  auto get (void *buf, size_t index, size_t n) -> void {
    if (index != -1 && cache_.count(index) > 0) {
      memcpy(buf, cache_[index], n);
      return;
    }
    file_.seekg(offset_(index));
    file_.read((char *) buf, n);
    AK_ASSERT(file_.good());
    if (index != -1) putCache_(buf, index, n);
  }
  /// write n bytes at index from buf.
  auto set (const void *buf, size_t index, size_t n) -> void {
    if (index != -1) {
      // dirty check
      if (cache_.count(index) > 0 && memcmp(buf, cache_[index], n) == 0) return;
      putCache_(buf, index, n);
    }
    file_.seekp(offset_(index));
    file_.write((const char *) buf, n);
    AK_ASSERT(file_.good());
  }
  /// @returns the stored index of the object
  auto push (const void *buf, size_t n) -> size_t {
    Metadata meta = meta_();
    size_t id = meta.next;
    if (meta.hasNext) {
      Metadata nextMeta;
      get(&nextMeta, meta.next, sizeof(nextMeta));
      set(&nextMeta, -1, sizeof(nextMeta));
    } else {
      ++meta.next;
      set(&meta, -1, sizeof(meta));
    }
    set(buf, id, n);
    return id;
  }
  auto remove (size_t index) -> void {
    Metadata meta = meta_();
    set(&meta, index, sizeof(meta));
    Metadata newMeta(index, true);
    set(&newMeta, -1, sizeof(newMeta));
    if (cache_.count(index) > 0) delete[] cache_[index];
    cache_.erase(index);
  }

  auto clearCache () -> void {
    for (const auto &[ _, ptr ] : cache_) delete[] ptr;
    cache_.clear();
  }
};

/**
 * an opinionated utility base class for the objects to be stored.
 * it handles get, update, and push for the object.
 * the inherited object must have two zero-length char arrays `_start` and `_end` to indicate the start and end positions to store.
 */
template <typename T, size_t szChunk = kDefaultSzChunk>
class ManagedObject {
 private:
  File<szChunk> *file_;
  size_t id_ = -1;
  ManagedObject (File<szChunk> &file, size_t id) : file_(&file), id_(id) {}
  static auto getSize_ () -> size_t { return offsetof(T, _end) - offsetof(T, _start); }
  static auto getOffset_ () -> size_t { return offsetof(T, _start); }
 public:
  ManagedObject () = delete;
  ManagedObject (File<szChunk> &file) : file_(&file) {}
  virtual ~ManagedObject () = default;

  auto id () -> size_t { return id_; }

  static auto get (File<szChunk> &file, size_t id) -> T {
    char buf[sizeof(T)];
    file.get(buf + getOffset_(), id, getSize_());
    ManagedObject &result = *reinterpret_cast<ManagedObject *>(buf);
    result.file_ = &file;
    result.id_ = id;
    return *reinterpret_cast<T *>(buf);
  }
  auto save () -> void {
    if (id_ != -1) throw Exception("Already saved");
    id_ = file_->push(reinterpret_cast<char *>(this) + getOffset_(), getSize_());
  }
  auto update () -> void {
    if (id_ == -1) throw Exception("Not saved");
    file_->set(reinterpret_cast<char *>(this) + getOffset_(), id_, getSize_());
  }
  auto destroy () -> void {
    if (id_ == -1) throw Exception("Not saved");
    file_->remove(id_);
    id_ = -1;
  }
};
} // namespace ak::file

#endif
