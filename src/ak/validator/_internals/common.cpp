#include "../../../../include/ak/validator/_internals/common.h"

namespace ak::validator::_internals {

void validate (bool t, bool inverse) {
  if (inverse == t) throw ValidationException();
}

} // namespace ak::validator::_internals
