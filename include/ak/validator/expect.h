#ifndef AK_LIB_VALIDATOR_EXPECT_H_
#define AK_LIB_VALIDATOR_EXPECT_H_

#include "ak/validator/_internals/validator.h"

namespace ak::validator {

template <typename T>
auto expect (const T &value) -> _internals::Validator<T> { return value; }

} // namespace ak::validator

#endif
