#include "signalFD.h"

#include <sys/signalfd.h>
#include <poll.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

namespace BR {

SignalFD::SignalFD()
{
	sigset_t mask;

	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask, SIGQUIT);

	sfd = signalfd(-1, &mask, O_CLOEXEC);

	if(sfd == -1){
		perror("signalfd failed");
		throw -128;
	}

	if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1){
		perror("sigprocmask failed");
		close(sfd);
		throw -127;
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
