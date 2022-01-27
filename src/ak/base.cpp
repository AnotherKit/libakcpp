#include "ak/base.h"

namespace ak {
auto Exception::what () const noexcept -> const char * { return message; }
Exception::~Exception () = default;
} // namespace ak
