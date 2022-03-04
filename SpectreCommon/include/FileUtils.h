#include "SpectreDef.h"
#include <filesystem>
#include <string>
BEGIN_NAMESPACE_SPECTRE

class FileUtils
{
public:
	static std::string ReadFile(const std::string& filename,bool isRes = true);
};

END_NAMESPACE_SPECTRE