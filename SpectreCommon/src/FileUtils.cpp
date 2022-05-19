#include "FileUtils.h"
#include <fstream>
#include "stb_image.h"
USING_NAMESPACE(Spectre)

const std::string g_resourecsDir = RESOURCES_DIR;

std::string FileUtils::ReadFile(const std::string& filename)
{
	std::string fullPath = g_resourecsDir + filename;
	std::ifstream file(fullPath, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("failed to open file!");
	}
	size_t fileSize = (size_t)file.tellg();
	std::string buffer;
	buffer.resize(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

unsigned char* FileUtils::ReadImageFile(const std::string& filename, int* width, int* height)
{
	std::string fullPath = g_resourecsDir + filename;
	int texChannels;
	stbi_uc* pixels = stbi_load(fullPath.c_str(), width, height, &texChannels, STBI_rgb_alpha);
	//stbi_uc* pixels = stbi_load_from_memory((stbi_uc*)data.c_str(), data.size(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	return pixels;
}
