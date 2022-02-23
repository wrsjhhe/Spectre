#include "ShaderTool.h"
#include <shaderc/shaderc.hpp>
#include "LogManager.h"
USING_NAMESPACE(Spectre)

static std::vector<uint32_t> compile_file(const std::string& source_name,
	shaderc_shader_kind kind,
	const std::string& source,
	bool optimize = false) {
	shaderc::Compiler compiler;
	shaderc::CompileOptions options;

	// Like -DMY_DEFINE=1
	//options.AddMacroDefinition("MY_DEFINE", "1");
	if (optimize) options.SetOptimizationLevel(shaderc_optimization_level_size);

	shaderc::SpvCompilationResult module =
		compiler.CompileGlslToSpv(source, kind, source_name.c_str(), options);

	if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
		EXP_CHECK(module.GetCompilationStatus() == shaderc_compilation_status_success, module.GetErrorMessage());
		return std::vector<uint32_t>();
	}

	return { module.cbegin(), module.cend() };
}

std::vector<uint32_t> ShaderTool::Compile_glsl(std::string strGlsl, ShaderType type)
{
	switch (type)
	{
	case Spectre::ShaderTool::Vertex:
		return compile_file("shader_src", shaderc_glsl_vertex_shader,
			strGlsl.c_str(), true);
	case Spectre::ShaderTool::Fragment:
		return compile_file("shader_src", shaderc_glsl_fragment_shader,
			strGlsl.c_str(), true);
	case Spectre::ShaderTool::Compute:
		return compile_file("shader_src", shaderc_glsl_compute_shader,
			strGlsl.c_str(), true);
	case Spectre::ShaderTool::Geometry:
		return compile_file("shader_src", shaderc_glsl_geometry_shader,
			strGlsl.c_str(), true);
	case Spectre::ShaderTool::Tess_control:
		return compile_file("shader_src", shaderc_tess_control_shader,
			strGlsl.c_str(),  true);
	case Spectre::ShaderTool::Tess_evaluation:
		return compile_file("shader_src", shaderc_tess_evaluation_shader,
			strGlsl.c_str(),  true);
	default:
		return std::vector<uint32_t>();
	}
}