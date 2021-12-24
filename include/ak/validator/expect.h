#ifndef AK_LIB_VALIDATOR_EXPECT_H_
#define AK_LIB_VALIDATOR_EXPECT_H_

#include "_internals/validator.h"

namespace ak::validator {

template <typename T>
_internals::Validator<T> expect (const T &value) { return value; }

} // namespace ak::validator

#endif
