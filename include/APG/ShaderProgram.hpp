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

#ifndef SHADERPROGRAM_HPP_
#define SHADERPROGRAM_HPP_

#include <cstdint>

#include <string>
#include <memory>

#include <glm/glm.hpp>

#include "ErrorBase.hpp"

namespace APG {

class ShaderProgram : public ErrorBase {
private:
	static std::string loadSourceFromFile(const std::string &filename);

	uint32_t vertexShader, fragmentShader;
	uint32_t shaderProgram;

	uint32_t *validateTypeAndGet(uint32_t shaderType);

	std::string shaderInfoLog, linkInfoLog;

	void loadShader(const std::string &vertexShaderFilename, uint32_t shaderType);
	void combineProgram();

public:
	ShaderProgram(const std::string &vertexShaderSource, const std::string &fragmentShaderSource);
	static std::unique_ptr<APG::ShaderProgram> fromSource(const std::string &vertexShaderSource,
			const std::string &fragmentShaderSource);
	static std::unique_ptr<APG::ShaderProgram> fromFiles(const std::string &vertexShaderFilename,
			const std::string &fragmentShaderFilename);

	~ShaderProgram();

	void use();

	void setFloatAttribute(const char * const attributeName, int32_t valueCount,
			int32_t strideInElements, int32_t offsetInElements, bool normalize = false);

	void setUniformf(const char * const uniformName, std::initializer_list<float> vals);
	void setUniformf(const char * const uniformName, float val);
	void setUniformf(const char * const uniformName, glm::vec2 vals);
	void setUniformf(const char * const uniformName, glm::vec3 vals);
	void setUniformf(const char * const uniformName, glm::vec4 vals);

	void setUniformi(const char * const uniformName, std::initializer_list<int32_t> vals);
	void setUniformi(const char * const uniformName, int32_t val);
	void setUniformi(const char * const uniformName, glm::ivec2 vals);
	void setUniformi(const char * const uniformName, glm::ivec3 vals);
	void setUniformi(const char * const uniformName, glm::ivec4 vals);

	uint32_t getProgramID() const {
		return shaderProgram;
	}

	std::string getShaderInfoLog() const {
		return shaderInfoLog;
	}

	std::string getLinkInfoLog() const {
		return linkInfoLog;
	}

	ShaderProgram() = delete;
	ShaderProgram(ShaderProgram &other) = delete;
	ShaderProgram(const ShaderProgram &other) = delete;
	ShaderProgram &operator=(ShaderProgram &other) = delete;
	ShaderProgram &operator=(const ShaderProgram &other) = delete;
};

}

#endif /* SHADERPROGRAM_HPP_ */
