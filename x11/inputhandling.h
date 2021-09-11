#ifndef INPUTHANDLING_H
#define INPUTHANDLING_H

#include "vk/Input.h"

enum class ViewPortMode : uint8_t {
	ASPECT = 0,
	STRETCH,
	INTEGER,
	END
};

namespace BR {
	void handleInput(Input& input, ViewPortMode& mode, bool& showLog);
}

#endif // INPUTHANDLING_H
