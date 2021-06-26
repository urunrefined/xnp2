#ifndef SIGNALFD_H
#define SIGNALFD_H

namespace BR {


class SignalFD
{
	int sfd;

public:
	SignalFD();
	bool isTriggered();
	~SignalFD();
};

}

#endif // SIGNALFD_H
