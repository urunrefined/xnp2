#ifndef INPUTHANDLING_H
#define INPUTHANDLING_H

#include "vk/Input.h"
#include "pulse/PulseSoundEngine.h"

enum class ViewPortMode : uint8_t {
	ASPECT = 0,
	STRETCH,
	INTEGER,
	END
};

enum class VisualScreen : uint8_t {
	MAIN = 0,
	CFG,
	END
};

namespace BR {
	void handleInput(Input& input, ViewPortMode& mode, VisualScreen& showLog, Sfx::PulseSoundEngine& soundRef);
}

#endif // INPUTHANDLING_H
