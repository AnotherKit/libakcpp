#ifndef AK_LIB_FILE_FILE_H_
#define AK_LIB_FILE_FILE_H_

#include <errno.h>
#include <sys/stat.h>

#include <fstream>
#include <functional>
#include <limits>
#include <memory>
#include <unordered_map>

#include "../base.h"

namespace ak::file {
/**
 * a chunked file storage with manual garbage collection, with chunk size of szChunk and a cache powered by unordered_map.
 * the stored class need to be "serializable," i.e. has a subclass (or subtype) Serialized.
 * File methods will call Serialized(T) on serialization, and T(Serialized, File *file, int id) on deserialization.
 * Serialized need to be able to be stored directly, i.e. it has no pointer fields and no data stored on heap space (e.g. no std::string, std::set, etc.).
 * Serialized also need to be copy constructible.
 * File would call the default constructor of Serialized, so it need to be present.
 */
template <size_t szChunk>
class File {
 private:
  static_assert(szChunk > sizeof(size_t));
  struct Metadata {
    struct Serialized {
      size_t next : std::numeric_limits<size_t>::digits - 1;
      bool hasNext : 1;
      Serialized () = default;
      Serialized (const Metadata &meta) : next(meta.next), hasNext(meta.hasNext) {}
    };
    size_t next;
    bool hasNext;
    Metadata (size_t next, bool hasNext) : next(next), hasNext(hasNext) {}
    Metadata (const Serialized &serialized, File * /* unused */, int /* unused */) : next(serialized.next), hasNext(serialized.hasNext) {}
  };
  Metadata meta_ () { return get<Metadata>(-1); }
  size_t offset_ (size_t index) { return (index + 1) * szChunk; }
  std::fstream file_;
  std::unordered_map<size_t, std::shared_ptr<void>> cache_;
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
      set(-1, Metadata(0, false));
      initializer();
    }
  }

  template <typename T>
  T get (size_t index) {
    if (index != -1 && cache_.count(index) > 0) return T(*reinterpret_cast<typename T::Serialized *>(cache_[index].get()), index);
    typename T::Serialized read;
    file_.seekg(offset_(index));
    file_.read(reinterpret_cast<char *>(&read), sizeof(read));
    if (index != -1) cache_[index] = std::shared_ptr<void>(new typename T::Serialized(read));
    return T(read, this, index);
  }
  template <typename T>
  void set (size_t index, const T &object) {
    typename T::Serialized serialized{object};
    if (index != -1) cache_[index] = std::shared_ptr<void>(new typename T::Serialized(serialized));
    file_.seekp(offset_(index));
    file_.write(reinterpret_cast<const char *>(&serialized), sizeof(serialized));
    file_.flush();
  }
  /// @returns the stored index of the object
  template <typename T>
  int push (const T &object) {
    Metadata meta = meta_();
    int id = meta.next;
    set(id, object);
    if (meta.hasNext) {
      Metadata nextMeta = get<Metadata>(meta.next);
      set(-1, nextMeta);
    } else {
      ++meta.next;
      set(-1, meta);
    }
    return id;
  }
  void remove (size_t index) {
    set(index, meta_());
    set(-1, Metadata(index, true));
    cache_.erase(index);
  }

  void clearCache () { cache_.clear(); }
};
} // namespace ak::file

#endif
