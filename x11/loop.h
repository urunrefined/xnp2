#ifndef LOOP_H
#define LOOP_H

#include "signalFD.h"
#include "np2.h"
#include "pccore.h"
#include "pulse/PulseSoundEngine.h"

namespace BR{
	void loop(SignalFD& sfd,
			  NP2CFG& cfg,
			  NP2OSCFG& oscfg,
			  Sfx::PulseSoundEngine& soundEngine);
}

#endif // LOOP_H
