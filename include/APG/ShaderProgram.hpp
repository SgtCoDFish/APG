/*
 * Copyright (c) 2014, 2015 Ashley Davis (SgtCoDFish)
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the name of the <organization> nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SHADERPROGRAM_HPP_
#define SHADERPROGRAM_HPP_

#include <cstdint>

#include <string>
#include <memory>
#include <vector>

#include <glm/glm.hpp>

namespace APG {

class VertexAttribute;
class VertexAttributeList;

class ShaderProgram final {
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

	virtual ~ShaderProgram();

	void use();

	void setVertexAttribute(const VertexAttribute &vertexAttribute, uint16_t stride);
	void setVertexAttributes(const VertexAttributeList &attributeList);

	/**
	 * Sets a float attribute directly. Can make calculations a little more complicated,
	 * better to set via a VertexAttribute.
	 * @param attributeName the alias of the attribute, e.g. "position"
	 * @param valueCount the amount of floats this attribute has, between 1-4 inclusive.
	 * @param strideInElements the stride in elements (the total number of elements passed in)
	 * @param offsetInElements the offset in elements (the number of elements before this one)
	 * @param normalize whether to normalize between -1.0f - 1.0f, default false.
	 */
	void setFloatAttribute(const char * const attributeName, uint8_t valueCount, uint32_t strideInElements,
	        uint32_t offsetInElements, bool normalize = false);

	void setUniformf(const char * const uniformName, std::initializer_list<float> vals);
	void setUniformf(const char * const uniformName, float val);
	void setUniformf(const char * const uniformName, glm::vec2 vals);
	void setUniformf(const char * const uniformName, glm::vec3 vals);
	void setUniformf(const char * const uniformName, glm::vec4 vals);
	void setUniformf(const char * const uniformName, glm::mat4 mat);

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
