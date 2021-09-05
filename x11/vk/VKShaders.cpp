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

static std::vector<char> readShaderFile(const char *filename){
	try {
		std::string shaderFilename (std::string("shader/") + filename);
		printf("Try relative shaderfilename: %s\n", shaderFilename.c_str());
		return readFile(shaderFilename);
	}  catch (...) {
		std::string shaderFilename (std::string("/usr/share/xnp2katze/") + filename);
		printf("Try global shaderfilename: %s\n", shaderFilename.c_str());
		return readFile(shaderFilename);
	}
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
	vertTri    (device, readShaderFile("vertTri.spv")),
	fragTri    (device, readShaderFile("fragTri.spv")),
	vertTexExt (device, readShaderFile("vertTexExt.spv")),
	vertTex    (device, readShaderFile("vertTex.spv")),
	fragTex    (device, readShaderFile("fragTex.spv"))
{}

ShaderStore::~ShaderStore(){
}

}
