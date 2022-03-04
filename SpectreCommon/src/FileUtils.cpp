#include "FileUtils.h"
#include <fstream>
USING_NAMESPACE(Spectre)

const std::string g_resourecsDir = RESOURCES_DIR;

std::string FileUtils::ReadFile(const std::string& filename, bool isRes)
{
	std::string fullPath = filename;
	if (isRes)
		fullPath = g_resourecsDir + filename;
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
