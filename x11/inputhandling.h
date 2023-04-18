#ifndef INPUTHANDLING_H
#define INPUTHANDLING_H

#include "vk/Input.h"
#include "pulse/PulseSoundEngine.h"
#include "keystat.h"

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

struct KeyMappingNew {
  public:
    KeyButtons key;
    PC98Key pc98Key;
};

class InputMapper{
   std::vector<KeyMappingNew> mapping;
   
public:
   InputMapper(std::vector<KeyMappingNew> mapping_) : mapping(mapping_){}
    void handleInput(Input &input, ViewPortMode &viewPortMode,
                     VisualScreen &visualScreen, Sfx::PulseSoundEngine &soundRef);
};

std::vector<KeyMappingNew> getDefaultKeyMap();
std::vector<KeyMappingNew> readKeyMap(const char *filename);
void saveKeyMap(const char *filename, const std::vector<KeyMappingNew>& mapping);

}

#endif // INPUTHANDLING_H
