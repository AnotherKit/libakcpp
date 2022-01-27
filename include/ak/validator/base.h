/**
 * validator/base.h - validator base components.
 *
 * AnotherKit validator is an declarative, intuitive but slow validator. It uses ak::equals as the equality comparator.
 * @example ak::validator::expect("Hello World").toMatch(R"(Hel+o\s\w+)");
 * @example ak::validator::expect({ 1926, 817 }).toInclude(1926).butNot().toInclude(233);
 */

#ifndef AK_LIB_VALIDATOR_BASE_H_
#define AK_LIB_VALIDATOR_BASE_H_

#include "ak/base.h"

namespace ak::validator {

/// thrown when input not fully validated.
class ValidationException : public Exception {
 public:
  explicit ValidationException () : Exception("validation failed") {}
};

} // namespace ak::validator

#endif
