#include "../../../../include/ak/validator/_internals/common.h"

namespace ak::validator::_internals {

auto validate (bool t, bool inverse) -> void {
  if (inverse == t) throw ValidationException();
}

} // namespace ak::validator::_internals
