#ifndef EXCEPTION_H
#define EXCEPTION_H

namespace BR {

class Exception {
  public:
    const char *msg;

    Exception(const char *msg_);
};

class CException {
  public:
    const char *msg;
    int osErrno;

    CException(const char *msg_, int osErrno);
};

} // namespace BR

#endif // EXCEPTION_H
