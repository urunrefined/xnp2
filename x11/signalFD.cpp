#include "signalFD.h"

#include <sys/signalfd.h>
#include <poll.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "exception.h"

namespace BR {

SignalFD::SignalFD()
{
	sigset_t mask;

	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask, SIGQUIT);

	sfd = signalfd(-1, &mask, O_CLOEXEC);

	if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1){
		throw CException("sigprocmask failed", errno);
	}

	if(sfd == -1){
		throw CException("signalfd failed", errno);
	}
}

bool SignalFD::isTriggered(){
	pollfd pfd = {sfd, POLL_IN, 0};
	return poll(&pfd, 1, 0) != 0;
}

SignalFD::~SignalFD(){
	close(sfd);
}

}
