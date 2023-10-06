#include "VKInstance.h"

#include "string.h"
#include <stdexcept>
#include <vector>

#include "VKGLFW.h"

namespace BR {

/* TODO: This should be moved into the glfw and should be passed to the instance
 * This has no business being here */

const char *validationLayerString = "VK_LAYER_KHRONOS_validation";

static std::vector<const char *>
getRequiredExtensions(bool enableValidationLayers) {
    std::vector<const char *> extensions;

    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    for (uint32_t i = 0; i < glfwExtensionCount; i++) {
        printf("GLFW requires extension %s\n", glfwExtensions[i]);
        extensions.push_back(glfwExtensions[i]);
    }

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    }

    return extensions;
}

static bool checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const auto &layerProperties : availableLayers) {
        printf("Validation layer: %s\n", layerProperties.layerName);
    }

    for (const auto &layerProperties : availableLayers) {
        if (strcmp(validationLayerString, layerProperties.layerName) == 0) {
            return true;
        }
    }

    return false;
}

VulkanInstance::VulkanInstance(bool enableValidationLayers) {
    if (enableValidationLayers && !checkValidationLayerSupport()) {
        throw std::runtime_error(
            "validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "XNP2Katze";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "XNP2Katze Scaler";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = getRequiredExtensions(enableValidationLayers);
    createInfo.enabledExtensionCount = (uint32_t)extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = 1;
        createInfo.ppEnabledLayerNames = &validationLayerString;
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
}

VulkanInstance::~VulkanInstance() { vkDestroyInstance(instance, nullptr); }

} // namespace BR
