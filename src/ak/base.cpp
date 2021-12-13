#include "../../include/ak/base.h"

namespace ak {
const char *Exception::what () const noexcept { return message; }
Exception::~Exception () {}
} // namespace ak
