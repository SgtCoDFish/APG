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

#ifndef SHADERPROGRAM_HPP_
#define SHADERPROGRAM_HPP_

#include <cstdint>

#include <string>

#include <glm/glm.hpp>

#include "ErrorBase.hpp"

namespace APG {

class ShaderProgram : public ErrorBase {
private:
	uint32_t vertexShader, fragmentShader;
	uint32_t shaderProgram;

	uint32_t *validateTypeAndGet(uint32_t shaderType);

	std::string shaderInfoLog, linkInfoLog;

	void loadShader(const std::string &vertexShaderFilename, uint32_t shaderType);
	void combineProgram();

public:
	ShaderProgram(const std::string &vertexShaderFilename,
			const std::string &fragmentShaderFileName);
	~ShaderProgram();

	void use();

	void setFloatAttribute(const char * const attributeName, int32_t valueCount,
			int32_t strideInElements, int32_t offsetInElements, bool normalize = false);

	void setUniformf(const char * const uniformName, std::initializer_list<float> vals);
	void setUniformf(const char * const uniformName, float val);
	void setUniformf(const char * const uniformName, glm::vec2 vals);
	void setUniformf(const char * const uniformName, glm::vec3 vals);
	void setUniformf(const char * const uniformName, glm::vec4 vals);

	uint32_t getProgramID() const {
		return shaderProgram;
	}

	std::string getShaderInfoLog() const {
		return shaderInfoLog;
	}

	std::string getLinkInfoLog() const {
		return linkInfoLog;
	}
	/*
	 void setFloatAttribute(const std::string &attributeName, int32_t valueCount, int32_t stride,
	 GLvoid *offset, bool normalize = false) {
	 setFloatAttribute(attributeName.c_str(), valueCount, stride, offset, normalize);
	 }
	 void setUniformf(const std::string &uniformName, std::initializer_list<float> vals) {
	 setUniformf(uniformName.c_str(), vals);
	 }
	 */
};

}

#endif /* SHADERPROGRAM_HPP_ */
