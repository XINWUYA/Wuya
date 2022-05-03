#include "Pch.h"
#include "OpenGLShader.h"

#include <fstream>
#include <glm/gtc/type_ptr.hpp>

namespace Wuya
{
	static const char* GetShaderCacheDirectory()
	{
		return "assets/cache/shader/opengl";
	}

	static void CreateShaderCacheDirectoryIfNeed()
	{
		const std::string shader_cache_dir = GetShaderCacheDirectory();
		if (!std::filesystem::exists(shader_cache_dir))
			std::filesystem::create_directories(shader_cache_dir);
	}

	OpenGLShader::OpenGLShader(const std::string& filepath) : m_FilePath(filepath)
	{
		PROFILE_FUNCTION();

		CreateShaderCacheDirectoryIfNeed();

		const std::string shader_src = ReadFile(filepath);
		PreProcessShaderSrc(shader_src, m_OpenGLSourceCodes);

		CompileShaders();
		CreateShaderProgram();

		// Extract name from filepath
		auto last_slash = filepath.find_last_of("/\\");
		last_slash = (last_slash == std::string::npos) ? 0 : last_slash + 1;
		const auto last_dot = filepath.rfind('.');
		const auto count = (last_dot == std::string::npos) ? filepath.size() - last_slash : last_dot - last_slash;
		m_Name = filepath.substr(last_slash, count);
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertex_src, const std::string& pixel_src) : m_Name(name)
	{
		PROFILE_FUNCTION();

		// Preprocess shader src
		m_OpenGLSourceCodes[GL_VERTEX_SHADER] = vertex_src;
		m_OpenGLSourceCodes[GL_FRAGMENT_SHADER] = pixel_src;

		CompileShaders();
		CreateShaderProgram();
	}

	OpenGLShader::~OpenGLShader()
	{
		PROFILE_FUNCTION();

		glDeleteProgram(m_ProgramID);
	}

	void OpenGLShader::Bind()
	{
		PROFILE_FUNCTION();

		glUseProgram(m_ProgramID);
	}

	void OpenGLShader::Unbind()
	{
		PROFILE_FUNCTION();

		glUseProgram(0);
	}

	void OpenGLShader::SetInt(const std::string& name, int value)
	{
		PROFILE_FUNCTION();

		const GLint location = glGetUniformLocation(m_ProgramID, name.c_str());
		glUniform1i(location, value);
	}

	void OpenGLShader::SetIntArray(const std::string& name, int* values, uint32_t count)
	{
		PROFILE_FUNCTION();

		const GLint location = glGetUniformLocation(m_ProgramID, name.c_str());
		glUniform1iv(location, count, values);
	}

	void OpenGLShader::SetFloat(const std::string& name, float value)
	{
		PROFILE_FUNCTION();

		const GLint location = glGetUniformLocation(m_ProgramID, name.c_str());
		glUniform1f(location, value);
	}

	void OpenGLShader::SetFloat2(const std::string& name, const glm::vec2& value)
	{
		PROFILE_FUNCTION();

		const GLint location = glGetUniformLocation(m_ProgramID, name.c_str());
		glUniform2f(location, value.x, value.y);
	}

	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		PROFILE_FUNCTION();

		const GLint location = glGetUniformLocation(m_ProgramID, name.c_str());
		glUniform3f(location, value.x, value.y, value.z);
	}

	void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		PROFILE_FUNCTION();

		const GLint location = glGetUniformLocation(m_ProgramID, name.c_str());
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
	{
		PROFILE_FUNCTION();

		const GLint location = glGetUniformLocation(m_ProgramID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

	std::string OpenGLShader::ReadFile(const std::string& filepath)
	{
		PROFILE_FUNCTION();

		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary); // ifstream closes itself due to RAII
		if (in)
		{
			in.seekg(0, std::ios::end);
			size_t size = in.tellg();
			if (size != -1)
			{
				result.resize(size);
				in.seekg(0, std::ios::beg);
				in.read(&result[0], size);
			}
			else
			{
				CORE_LOG_ERROR("Could not read from file '{0}'", filepath);
				ASSERT(false);
			}
		}
		else
		{
			CORE_LOG_ERROR("Could not open file '{0}'", filepath);
			ASSERT(false);
		}

		return result;
	}

	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;

		ASSERT(false, "Unknown shader type!");
		return 0;
	}

	void OpenGLShader::PreProcessShaderSrc(const std::string& source, std::unordered_map<GLenum, std::string>& shader_sources)
	{
		PROFILE_FUNCTION();

		const char* type_token = "#type";
		const size_t type_token_length = strlen(type_token);
		size_t pos = source.find(type_token, 0); // Start of shader type declaration line
		while (pos != std::string::npos)
		{
			const size_t eol = source.find_first_of("\r\n", pos); // End of shader type declaration line
			ASSERT(eol != std::string::npos, "Syntax error");
			const size_t begin = pos + type_token_length + 1; // Start of shader type name (after "#type " keyword)
			std::string type = source.substr(begin, eol - begin);
			ASSERT(ShaderTypeFromString(type), "Invalid shader type specified");

			const size_t next_line_pos = source.find_first_not_of("\r\n", eol); // Start of shader code after shader type declaration line
			ASSERT(next_line_pos != std::string::npos, "Syntax error");
			pos = source.find(type_token, next_line_pos); // Start of next shader type declaration line

			shader_sources[ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(next_line_pos) : source.substr(next_line_pos, pos - next_line_pos);
		}
	}

	void OpenGLShader::CompileShaders()
	{
		//std::vector<GLuint> compiled_shaders;
		//for (auto& source_code : m_OpenGLSourceCodes)
		//{
		//	// Create shader
		//	GLuint shader = glCreateShader(source_code.first);
		//	const auto* code = (const GLchar*)source_code.second.c_str();
		//	glShaderSource(shader, 1, &code, 0);

		//	// Compile Shader
		//	glCompileShader(shader);

		//	GLint is_compiled = 0;
		//	glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);
		//	if (is_compiled == GL_FALSE)
		//	{
		//		GLint max_length = 0;
		//		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);
		//		std::vector<GLchar> info_log(max_length);
		//		glGetShaderInfoLog(shader, max_length, &max_length, &info_log[0]);

		//		// Delete current shader
		//		glDeleteShader(shader);

		//		// Delete compiled shaders
		//		for(auto it : compiled_shaders)
		//			glDeleteShader(it);

		//		break;
		//	}
		//	compiled_shaders.emplace_back(shader);
		//}
	}

	void OpenGLShader::CreateShaderProgram()
	{
		PROFILE_FUNCTION();

		// Compile shaders
		std::vector<GLuint> compiled_shaders;
		for (auto& source_code : m_OpenGLSourceCodes)
		{
			// Create shader
			GLuint shader = glCreateShader(source_code.first);
			const auto* code = (const GLchar*)source_code.second.c_str();
			glShaderSource(shader, 1, &code, 0);

			// Compile shader
			glCompileShader(shader);

			GLint is_compiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);
			if (is_compiled == GL_FALSE)
			{
				GLint max_length = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);
				std::vector<GLchar> info_log(max_length);
				glGetShaderInfoLog(shader, max_length, &max_length, &info_log[0]);
				CORE_LOG_ERROR("Shader compiled failed ({0}):\n{1}", m_FilePath, info_log.data());
				ASSERT(false);

				// Delete current shader
				glDeleteShader(shader);

				// Delete compiled shaders
				for (auto it : compiled_shaders)
					glDeleteShader(it);

				break;
			}
			compiled_shaders.emplace_back(shader);
		}

		// Create shader program
		GLuint program = glCreateProgram();

		// Attach shaders to program
		for (const auto shader : compiled_shaders)
		{
			glAttachShader(program, shader);
		}

		// Link program
		glLinkProgram(program);

		GLint is_linked;
		glGetProgramiv(program, GL_LINK_STATUS, &is_linked);
		if (is_linked == GL_FALSE)
		{
			GLint maxLength;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> info_log(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, info_log.data());
			CORE_LOG_ERROR("Shader linking failed ({0}):\n{1}", m_FilePath, info_log.data());
			ASSERT(false);

			// Delete program
			glDeleteProgram(program);

			// Delete compiled shaders
			for (const auto shader : compiled_shaders)
				glDeleteShader(shader);
		}

		// Always detach shaders after a successful link.
		for (const auto shader : compiled_shaders)
		{
			glDetachShader(program, shader);
			glDeleteShader(shader);
		}

		m_ProgramID = program;
	}
}
