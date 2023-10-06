#include "exception.h"

namespace BR {

Exception::Exception(const char *msg_) : msg(msg_) {}

CException::CException(const char *msg_, int osErrno_)
    : msg(msg_), osErrno(osErrno_) {}

} // namespace BR
