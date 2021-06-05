#include "VKShaders.h"

#include <fstream>

namespace BR{

static std::vector<char> readFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error(std::string("failed to open file!") + filename );
	}

	size_t fileSize = (size_t) file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

ShaderStage::ShaderStage(const VkDevice &device_, const std::vector<char> &code) : device(device_){
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = (uint32_t*) code.data();

	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}
}

ShaderStage::~ShaderStage(){
	vkDestroyShaderModule(device, shaderModule, nullptr);
}

ShaderStage::operator VkShaderModule &(){
	return shaderModule;
}

ShaderStore::ShaderStore(const VkDevice &device) :
	vertTri (device, readFile("shader/vertTri.spv")),
	fragTri (device, readFile("shader/fragTri.spv")),
	vertTex  (device, readFile("shader/vertTex.spv")),
	fragTex  (device, readFile("shader/fragTex.spv"))
{}

ShaderStore::~ShaderStore(){
}

}
