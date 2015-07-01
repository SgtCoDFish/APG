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

#ifndef SPRITEBATCH_HPP_
#define SPRITEBATCH_HPP_

#include <cstdint>

#include <memory>

#include <glm/gtc/matrix_transform.hpp>

#include "APG/ErrorBase.hpp"
#include "APG/Buffer.hpp"
#include "APG/Texture.hpp"
#include "APG/ShaderProgram.hpp"
#include "APG/VAO.hpp"
#include "APG/Mesh.hpp"
#include "APG/VertexBufferObject.hpp"
#include "APG/IndexBufferObject.hpp"
#include "APG/SpriteBase.hpp"

namespace APG {

class Sprite;

class SpriteBatch : public ErrorBase {
private:
	static const char * const POSITION_ATTRIBUTE;
	static const char * const COLOR_ATTRIBUTE;
	static const char * const TEXCOORD_ATTRIBUTE;

	const uint32_t bufferSize;

	bool drawing = false;

	VAO vao;
	VertexBufferObject vertexBuffer;
	IndexBufferObject indexBuffer;

	std::vector<float> vertices;

	std::unique_ptr<APG::ShaderProgram> ownedShaderProgram;
	ShaderProgram *program = nullptr;

	uint64_t idx = 0;

	Texture * lastTexture = nullptr;
	void switchTexture(Texture * const newTexture);

	glm::vec4 color;
	glm::mat4 projectionMatrix;
	glm::mat4 transformMatrix;
	glm::mat4 combinedMatrix;
	void setupMatrices();

public:
	static const uint32_t DEFAULT_BUFFER_SIZE;
	explicit SpriteBatch(uint32_t bufferSize = DEFAULT_BUFFER_SIZE, ShaderProgram * const program =
			nullptr);
	virtual ~SpriteBatch() = default;

	void begin();
	void end();

	void flush();

	void draw(Texture * const image, float x, float y, uint32_t width, uint32_t height, float srcX,
			float srcY, uint32_t srcWidth, uint32_t srcHeight);

	void draw(SpriteBase * const sprite, float x, float y);

	glm::vec4 getColor() const {
		return color;
	}

	void setColor(const glm::vec4 &newColor) {
		color = glm::vec4(newColor);
	}

	void setColor(float r, float g, float b, float a) {
		color.r = r;
		color.g = g;
		color.b = b;
		color.a = a;
	}

	static std::unique_ptr<APG::ShaderProgram> createDefaultShader();
};

}

#endif /* SPRITEBATCH_HPP_ */
