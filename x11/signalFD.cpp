#include "signalFD.h"

#include <sys/signalfd.h>
#include <poll.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "exception.h"

namespace BR {

SignalBlock::SignalBlock(){
	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask, SIGQUIT);

	if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1){
		throw CException("sigprocmask failed", errno);
	}
};

SignalBlock::~SignalBlock(){
	sigprocmask(SIG_UNBLOCK, &mask, NULL);
};

SignalFD::SignalFD()
{
	sfd = signalfd(-1, &(block.mask), O_CLOEXEC);

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
