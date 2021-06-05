#ifndef VKSHADERS_H_
#define VKSHADERS_H_

#include <vector>

#include "VKUtil.h"

namespace BR {

class ShaderStage {
public:
	const VkDevice& device;

	VkShaderModule shaderModule;

	ShaderStage(const VkDevice& device_, const std::vector<char>& code);
	~ShaderStage();
	
	operator VkShaderModule& ();
	
	ShaderStage(const ShaderStage& that) = delete;
};

class ShaderStore {
public:
	ShaderStage vertTri;
	ShaderStage fragTri;

	ShaderStage vertTex;
	ShaderStage fragTex;

	ShaderStore(const VkDevice& device);
	
	~ShaderStore();
};

} // namespace BR

#endif
