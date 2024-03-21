#ifndef PCCORECALLBACKS_H
#define PCCORECALLBACKS_H

#include "vk/Input.h"
#include "vk/VKTexture.h"

namespace BR {

struct CallbackContext {
    VulkanTexture *texture;
    Input *input;
};

} // namespace BR

#endif // PCCORECALLBACKS_H
