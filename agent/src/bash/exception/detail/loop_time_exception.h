#ifndef LOOP_TIME_EXCEPTION_H
#define LOOP_TIME_EXCEPTION_H

#include "src/bash/exception/bash_exception.h"

namespace bash
{

namespace exception
{

namespace detail
{

class LoopTimeException : public ::bash::exception::BashException {
 public:
  inline char const* what() const throw ();
};

char const* LoopTimeException::what() const throw () {
  return "Time failed.";
}

}

}

}

#endif /* LOOP_TIME_EXCEPTION_H */
