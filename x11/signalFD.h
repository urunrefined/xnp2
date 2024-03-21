#ifndef SIGNALFD_H
#define SIGNALFD_H

#include <signal.h>

namespace BR {

class SignalBlock {
  public:
    sigset_t mask;

    SignalBlock();
    ~SignalBlock();
};

class SignalFD {
    SignalBlock block;
    int sfd;

  public:
    SignalFD();
    bool isTriggered();
    ~SignalFD();
};

} // namespace BR

#endif // SIGNALFD_H
