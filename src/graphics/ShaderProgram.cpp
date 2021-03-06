#ifndef APG_NO_GL

#include <cstdint>

#include <stdexcept>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <new>
#include <memory>

#include "APG/GL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "APG/graphics/ShaderProgram.hpp"
#include "APG/graphics/VertexAttribute.hpp"
#include "APG/graphics/VertexAttributeList.hpp"
#include "APG/internal/Assert.hpp"
#include "APG/graphics/GLError.hpp"

APG::ShaderProgram::ShaderProgram(const std::string &vertexShaderSource, const std::string &fragmentShaderSource) :
	logger {spdlog::get("APG")} {
	loadShader(vertexShaderSource, GL_VERTEX_SHADER);
	loadShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

	combineProgram();
}

std::unique_ptr<APG::ShaderProgram> APG::ShaderProgram::fromSource(const std::string &vertexShaderSource,
																   const std::string &fragmentShaderSource) {
	return std::make_unique<APG::ShaderProgram>(vertexShaderSource, fragmentShaderSource);
}

std::unique_ptr<APG::ShaderProgram> APG::ShaderProgram::fromFiles(const std::string &vertexShaderFilename,
																  const std::string &fragmentShaderFilename) {
	const auto vertexSource = ShaderProgram::loadSourceFromFile(vertexShaderFilename);
	const auto fragmentSource = ShaderProgram::loadSourceFromFile(fragmentShaderFilename);

	return std::make_unique<APG::ShaderProgram>(vertexSource, fragmentSource);
}

APG::ShaderProgram::~ShaderProgram() {
	glDeleteProgram(shaderProgram);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
}

void APG::ShaderProgram::use() {
	glUseProgram(shaderProgram);
}

void APG::ShaderProgram::setVertexAttribute(const APG::VertexAttribute &vertexAttribute, uint16_t stride) {
	setFloatAttribute(vertexAttribute.getAlias().c_str(), vertexAttribute.getComponentCount(), stride,
					  vertexAttribute.getOffset(), false);
}

void APG::ShaderProgram::setVertexAttributes(const VertexAttributeList &attributeList) {
	for (const auto &attribute : attributeList.getAttributes()) {
		setVertexAttribute(attribute, attributeList.getStride());
	}
}

void APG::ShaderProgram::setFloatAttribute(const char *const attributeName, uint8_t valueCount,
										   uint32_t strideInElements, uint32_t offsetInElements, bool normalize) {
	const auto attributeLocation = glGetAttribLocation(shaderProgram, attributeName);

	if (attributeLocation == -1) {
		logger->error("Couldn't get attribute location \"{}\"", attributeName);
		return;
	}

	glEnableVertexAttribArray(static_cast<GLuint>(attributeLocation));
	glVertexAttribPointer(static_cast<GLuint>(attributeLocation), valueCount, GL_FLOAT,
						  static_cast<GLboolean>((normalize ? GL_TRUE : GL_FALSE)),
						  strideInElements * sizeof(float), (void *) (sizeof(float) * offsetInElements));

	const auto error = glGetError();

	if (error != GL_NO_ERROR) {
		logger->error("Error while setting float attribute: {}.", prettyGLError(error));
		return;
	}
}

void APG::ShaderProgram::setUniformf(const char *const uniformName, std::initializer_list<float> vals) {
	const auto paramCount = vals.size();

	REQUIRE(paramCount >= 1 && paramCount <= 4, "Invalid parameter count in setUniformf");

	std::vector<float> vec;

	for (const auto &f : vals) {
		vec.emplace_back(f);
	}

	const auto uniLoc = glGetUniformLocation(shaderProgram, uniformName);

	switch (paramCount) {
		case 1:
			glUniform1f(uniLoc, vec[0]);
			break;

		case 2:
			glUniform2f(uniLoc, vec[0], vec[1]);
			break;

		case 3:
			glUniform3f(uniLoc, vec[0], vec[1], vec[2]);
			break;

		case 4:
			glUniform4f(uniLoc, vec[0], vec[1], vec[2], vec[3]);
			break;

		default:
			logger->critical("Invalid parameter count in setUniformf");
			throw std::runtime_error("Invalid parameter count in setUniformf");
			return;
	}
}

void APG::ShaderProgram::setUniformf(const char *const uniformName, float val) {
	const auto uniLoc = glGetUniformLocation(shaderProgram, uniformName);
	glUniform1f(uniLoc, val);
}

void APG::ShaderProgram::setUniformf(const char *const uniformName, const glm::vec2 &vals) {
	const auto uniLoc = glGetUniformLocation(shaderProgram, uniformName);
	glUniform2f(uniLoc, vals.x, vals.y);
}

void APG::ShaderProgram::setUniformf(const char *const uniformName, const glm::vec3 &vals) {
	const auto uniLoc = glGetUniformLocation(shaderProgram, uniformName);
	glUniform3f(uniLoc, vals.x, vals.y, vals.z);
}

void APG::ShaderProgram::setUniformf(const char *const uniformName, const glm::vec4 &vals) {
	const auto uniLoc = glGetUniformLocation(shaderProgram, uniformName);
	glUniform4f(uniLoc, vals.x, vals.y, vals.z, vals.w);
}

void APG::ShaderProgram::setUniformf(const char *const uniformName, const glm::mat4 &mat) {
	const auto uniLoc = glGetUniformLocation(shaderProgram, uniformName);
	glUniformMatrix4fv(uniLoc, 1, GL_FALSE, glm::value_ptr(mat));
}

void APG::ShaderProgram::setUniformi(const char *const uniformName, std::initializer_list<int32_t> vals) {
	const auto paramCount = vals.size();

	REQUIRE(paramCount >= 1 && paramCount <= 4, "Invalid parameter count in setUniformf");

	std::vector<int32_t> vec;

	for (const auto &f : vals) {
		vec.emplace_back(f);
	}

	const auto uniLoc = glGetUniformLocation(shaderProgram, uniformName);

	switch (paramCount) {
		case 1:
			glUniform1i(uniLoc, vec[0]);
			break;

		case 2:
			glUniform2i(uniLoc, vec[0], vec[1]);
			break;

		case 3:
			glUniform3i(uniLoc, vec[0], vec[1], vec[2]);
			break;

		case 4:
			glUniform4i(uniLoc, vec[0], vec[1], vec[2], vec[3]);
			break;

		default:
			logger->critical("Invalid parameter count in setUniformf");
			throw std::runtime_error("Invalid parameter count in setUniformf");
			return;
	}
}

void APG::ShaderProgram::setUniformi(const char *const uniformName, int32_t val) {
	const auto uniLoc = glGetUniformLocation(shaderProgram, uniformName);
	glUniform1i(uniLoc, val);
}

void APG::ShaderProgram::setUniformi(const char *const uniformName, const glm::ivec2 &vals) {
	const auto uniLoc = glGetUniformLocation(shaderProgram, uniformName);
	glUniform2i(uniLoc, vals.x, vals.y);
}

void APG::ShaderProgram::setUniformi(const char *const uniformName, const glm::ivec3 &vals) {
	const auto uniLoc = glGetUniformLocation(shaderProgram, uniformName);
	glUniform3i(uniLoc, vals.x, vals.y, vals.z);
}

void APG::ShaderProgram::setUniformi(const char *const uniformName, const glm::ivec4 &vals) {
	const auto uniLoc = glGetUniformLocation(shaderProgram, uniformName);
	glUniform4i(uniLoc, vals.x, vals.y, vals.z, vals.w);
}

std::string APG::ShaderProgram::loadSourceFromFile(const std::string &filename) {
	std::ifstream inStream(filename, std::ios::in);

	if (!inStream.is_open()) {
		auto logger = spdlog::get("APG");
		logger->critical("Couldn't find shader file: %v", filename);
		throw std::runtime_error("Couldn't find shader file");
		return "";
	}

	std::stringstream ss;
	std::string tempString;
	while (std::getline(inStream, tempString)) {
		ss << tempString << "\n";
	}

	inStream.close();

	return ss.str();
}

void APG::ShaderProgram::loadShader(const std::string &shaderSource, uint32_t type) {
	uint32_t *source = validateTypeAndGet(type);

	if (source == nullptr) {
		return;
	}

	*source = glCreateShader(type);

	auto csource = shaderSource.c_str();
	glShaderSource(*source, 1, &csource, nullptr);
	glCompileShader(*source);

	std::stringstream statusStream;
	GLint status;
	glGetShaderiv(*source, GL_COMPILE_STATUS, &status);

	if (status != GL_TRUE) {
		statusStream << "Compilation error in shader.\n";
	}

	GLint logLength;
	glGetShaderiv(*source, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 1) {
		auto buffer = std::make_unique<char[]>(static_cast<size_t>(logLength));

		glGetShaderInfoLog(*source, logLength, nullptr, buffer.get());

		statusStream << "Info log:\n" << buffer.get();
		statusStream << "Source:\n" << shaderSource << "\n\n";
	}

	shaderInfoLog = shaderInfoLog + statusStream.str();

	if (status != GL_TRUE) {
		logger->error("Shader error log: {}", shaderInfoLog);
		glDeleteShader(*source);
		return;
	}
}

void APG::ShaderProgram::combineProgram() {
	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	//glBindFragDataLocation(shaderProgram, 0, "outColor");
	glLinkProgram(shaderProgram);

	GLint status;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);

	std::stringstream linkStatusStream;

	if (status != GL_TRUE) {
		linkStatusStream << "Error while linking shader program.\n";
	}

	GLint infoLogLength;
	glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);

	if (infoLogLength > 1) {
		auto buffer = std::make_unique<char[]>(static_cast<size_t>(infoLogLength));

		glGetProgramInfoLog(shaderProgram, infoLogLength, nullptr, buffer.get());

		linkStatusStream << buffer.get();
	}

	GLenum glError;
	while ((glError = glGetError()) != GL_NO_ERROR) {
		status = GL_FALSE;

		linkStatusStream << "glGetError(): " << prettyGLError(glError) << "\n";
	}

	linkInfoLog = linkInfoLog + linkStatusStream.str();

	if (status != GL_TRUE) {
		logger->error("Link error log: {}", linkInfoLog);
		glDeleteProgram(shaderProgram);
		return;
	}

	glUseProgram(shaderProgram);
}

uint32_t *APG::ShaderProgram::validateTypeAndGet(uint32_t type) {
	switch (type) {
		case GL_VERTEX_SHADER: {
			return &vertexShader;
		}

		case GL_FRAGMENT_SHADER: {
			return &fragmentShader;
		}

		case GL_GEOMETRY_SHADER:
		case GL_TESS_CONTROL_SHADER:
		case GL_TESS_EVALUATION_SHADER:
			logger->critical("Geometry/Tessellation Evaluation/Tessellation Control shaders not supported by APG.");
			return nullptr;

		default: {
			logger->critical("Invalid type passed to validateType for shader program.");
			return nullptr;
		}
	}
}

#endif
