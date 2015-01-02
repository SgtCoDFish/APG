/*
 * ShaderProgram.hpp
 * Copyright (C) 2014, 2015 Ashley Davis (SgtCoDFish)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <new>
#include <memory>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <glm/glm.hpp>

#include "ShaderProgram.hpp"
#include "ErrorBase.hpp"

APG::ShaderProgram::ShaderProgram(const std::string &vertexShaderSource,
		const std::string &fragmentShaderSource) {
	loadShader(vertexShaderSource, GL_VERTEX_SHADER);

	if (hasError()) {
		return;
	}

	loadShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

	if (hasError()) {
		return;
	}

	combineProgram();
}

std::unique_ptr<APG::ShaderProgram> APG::ShaderProgram::fromSource(
		const std::string &vertexShaderSource, const std::string &fragmentShaderSource) {
	return std::make_unique<APG::ShaderProgram>(vertexShaderSource, vertexShaderSource);
}

std::unique_ptr<APG::ShaderProgram> APG::ShaderProgram::fromFiles(
		const std::string &vertexShaderFilename, const std::string &fragmentShaderFilename) {
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
	if (hasError()) {
		return;
	}

	glUseProgram(shaderProgram);
}

void APG::ShaderProgram::setFloatAttribute(const char * const attributeName, int32_t valueCount,
		int32_t strideInElements, int32_t offsetInElements, bool normalize) {
	const auto attributeLocation = glGetAttribLocation(shaderProgram, attributeName);

	if (attributeLocation == -1 || glGetError() != GL_NO_ERROR) {
		setErrorState(std::string("Couldn't get attribute location: ") + attributeName);
		return;
	}

	glEnableVertexAttribArray(attributeLocation);
	glVertexAttribPointer(attributeLocation, valueCount, GL_FLOAT, (normalize ? GL_TRUE : GL_FALSE),
			strideInElements * sizeof(float), (void *) (sizeof(float) * offsetInElements));

	const auto error = glGetError();

	if (error != GL_NO_ERROR) {
		std::stringstream ss;
		const char *errStr = (const char *) gluErrorString(error);
		ss << "Error while setting float attribute \"" << attributeName << "\":\n" << errStr;
		setErrorState(ss.str());
		return;
	}
}

void APG::ShaderProgram::setUniformf(const char * const uniformName,
		std::initializer_list<float> vals) {
	const auto paramCount = vals.size();

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
		setErrorState("Call to setUniformf with invalid number of values.");
		return;
	}
}

void APG::ShaderProgram::setUniformf(const char * const uniformName, float val) {
	const auto uniLoc = glGetUniformLocation(shaderProgram, uniformName);
	glUniform1f(uniLoc, val);
}

void APG::ShaderProgram::setUniformf(const char * const uniformName, glm::vec2 vals) {
	const auto uniLoc = glGetUniformLocation(shaderProgram, uniformName);
	glUniform2f(uniLoc, vals.x, vals.y);
}

void APG::ShaderProgram::setUniformf(const char * const uniformName, glm::vec3 vals) {
	const auto uniLoc = glGetUniformLocation(shaderProgram, uniformName);
	glUniform3f(uniLoc, vals.x, vals.y, vals.z);
}

void APG::ShaderProgram::setUniformf(const char * const uniformName, glm::vec4 vals) {
	const auto uniLoc = glGetUniformLocation(shaderProgram, uniformName);
	glUniform4f(uniLoc, vals.x, vals.y, vals.z, vals.w);
}

void APG::ShaderProgram::setUniformi(const char * const uniformName,
		std::initializer_list<int32_t> vals) {
	const auto paramCount = vals.size();

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
		setErrorState("Call to setUniformf with invalid number of values.");
		return;
	}
}

void APG::ShaderProgram::setUniformi(const char * const uniformName, int32_t val) {
	const auto uniLoc = glGetUniformLocation(shaderProgram, uniformName);
	glUniform1i(uniLoc, val);
}

void APG::ShaderProgram::setUniformi(const char * const uniformName, glm::ivec2 vals) {
	const auto uniLoc = glGetUniformLocation(shaderProgram, uniformName);
	glUniform2i(uniLoc, vals.x, vals.y);
}

void APG::ShaderProgram::setUniformi(const char * const uniformName, glm::ivec3 vals) {
	const auto uniLoc = glGetUniformLocation(shaderProgram, uniformName);
	glUniform3i(uniLoc, vals.x, vals.y, vals.z);
}

void APG::ShaderProgram::setUniformi(const char * const uniformName, glm::ivec4 vals) {
	const auto uniLoc = glGetUniformLocation(shaderProgram, uniformName);
	glUniform4i(uniLoc, vals.x, vals.y, vals.z, vals.w);
}

std::string APG::ShaderProgram::loadSourceFromFile(const std::string &filename) {
	std::ifstream inStream(filename, std::ios::in);

	if (!inStream.is_open()) {
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

	const auto csource = shaderSource.c_str();
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
		char *buffer = nullptr;

		try {
			buffer = new char[logLength];
		} catch (std::bad_alloc &ba) {
			setErrorState("Couldn't allocate buffer for shader info log.");
			return;
		}

		glGetShaderInfoLog(*source, logLength, NULL, buffer);

		statusStream << "Info log:\n" << buffer;

		delete[] buffer;

		statusStream << "Source:\n" << shaderSource << "\n\n";
	}

	shaderInfoLog = shaderInfoLog + statusStream.str();

	if (status != GL_TRUE) {
		setErrorState(shaderInfoLog);
		glDeleteShader(*source);
		return;
	}
}

void APG::ShaderProgram::combineProgram() {
	if (hasError()) {
		return;
	}

	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glBindFragDataLocation(shaderProgram, 0, "outColor");
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
		char *buffer = nullptr;

		try {
			buffer = new char[infoLogLength];
		} catch (std::bad_alloc &ba) {
			setErrorState("Could not allocate buffer for link info log.");
			return;
		}

		glGetProgramInfoLog(shaderProgram, infoLogLength, nullptr, buffer);

		linkStatusStream << buffer;
		delete[] buffer;
	}

	GLenum glerror;
	while ((glerror = glGetError()) != GL_NO_ERROR) {
		status = GL_FALSE;

		linkStatusStream << "glGetError(): " << gluErrorString(glerror) << "\n";
	}

	linkInfoLog = linkInfoLog + linkStatusStream.str();

	if (status != GL_TRUE) {
		setErrorState(linkInfoLog);
		glDeleteProgram(shaderProgram);
		return;
	}

	glUseProgram(shaderProgram);
}

uint32_t *APG::ShaderProgram::validateTypeAndGet(uint32_t type) {
	switch (type) {
	case GL_VERTEX_SHADER: {
		return &vertexShader;
		break;
	}

	case GL_FRAGMENT_SHADER: {
		return &fragmentShader;
		break;
	}

	case GL_GEOMETRY_SHADER:
	case GL_TESS_CONTROL_SHADER:
	case GL_TESS_EVALUATION_SHADER:
		setErrorState(
				"Geometry/Tesselation Evaluation/Tesselation Control shaders not supported by APG.");
		return nullptr;

	default: {
		setErrorState("Invalid type passed to validateType.");
		return nullptr;
	}
	}
}
