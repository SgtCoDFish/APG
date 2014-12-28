/*
 * ShaderProgram.hpp
 * Copyright (C) 2014 Ashley Davis (SgtCoDFish)
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

#include <string>
#include <sstream>
#include <fstream>
#include <new>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "ShaderProgram.hpp"
#include "ErrorBase.hpp"

APG::ShaderProgram::ShaderProgram(const std::string &vertexShaderFilename,
		const std::string &fragmentShaderFilename) {
	loadShader(vertexShaderFilename, GL_VERTEX_SHADER);

	if (hasError()) {
		return;
	}

	loadShader(fragmentShaderFilename, GL_FRAGMENT_SHADER);

	if (hasError()) {
		return;
	}

	combineProgram();
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
		int32_t stride, GLvoid *offset, bool normalize) {
	use();
	auto attributeLocation = glGetAttribLocation(shaderProgram, attributeName);

	if (glGetError() != GL_NO_ERROR) {
		setErrorState("Couldn't get attribute location.");
		return;
	}

	glEnableVertexAttribArray(attributeLocation);
	glVertexAttribPointer(attributeLocation, valueCount, GL_FLOAT, (normalize ? GL_TRUE : GL_FALSE),
			stride, offset);

	const auto error = glGetError();

	if (error != GL_NO_ERROR) {
		std::stringstream ss;
		const char *errStr = (const char *) gluErrorString(error);
		ss << "Error while setting float attribute \"" << attributeName << "\":\n" << errStr;
		setErrorState(ss.str());
		return;
	}
}

void APG::ShaderProgram::loadShader(const std::string &shaderFilename, uint32_t type) {
	uint32_t *source = validateTypeAndGet(type);

	if (source == nullptr) {
		return;
	}

	std::ifstream inStream(shaderFilename, std::ios::in);

	if (!inStream.is_open()) {
		setErrorState(std::string("Could not open ") + shaderFilename);
		return;
	}

	std::stringstream ss;
	std::string tempString;
	while (std::getline(inStream, tempString)) {
		ss << tempString << "\n";
	}

	inStream.close();

	*source = glCreateShader(type);

	const char *csource = ss.str().c_str();
	glShaderSource(*source, 1, &csource, nullptr);
	glCompileShader(*source);

	std::stringstream statusStream;
	GLint status;
	glGetShaderiv(*source, GL_COMPILE_STATUS, &status);

	if (status != GL_TRUE) {
		statusStream << "Compilation error in " << shaderFilename << ".\n";
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
