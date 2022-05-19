#include "SpectreDef.h"
#include <filesystem>
#include <string>
BEGIN_NAMESPACE_SPECTRE

class FileUtils
{
public:
	static std::string ReadFile(const std::string& filename);

	static unsigned char* ReadImageFile(const std::string& filename, int* width, int* height);
};

END_NAMESPACE_SPECTRE