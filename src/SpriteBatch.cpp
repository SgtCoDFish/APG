/*
 * SpriteBatch.cpp
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

#include <cstdint>

#include <string>
#include <sstream>
#include <utility>
#include <memory>
#include <vector>

#include <glm/vec2.hpp>

#include "ErrorBase.hpp"
#include "SpriteBatch.hpp"
#include "ShaderProgram.hpp"
#include "Buffer.hpp"
#include "APGCommon.hpp"
#include "Sprite.hpp"
#include "Mesh.hpp"
#include "VertexAttribute.hpp"

const char * const APG::SpriteBatch::POSITION_ATTRIBUTE = "position";
const char * const APG::SpriteBatch::COLOR_ATTRIBUTE = "color";
const char * const APG::SpriteBatch::TEXCOORD_ATTRIBUTE = "texcoord";
const uint32_t APG::SpriteBatch::DEFAULT_BUFFER_SIZE = 1000;

APG::SpriteBatch::SpriteBatch(uint32_t bufferSize, ShaderProgram * const program) :
		ErrorBase(), //
		bufferSize(bufferSize), //
		vao(), //
		vertexBuffer( { //
				VertexAttribute(POSITION_ATTRIBUTE, AttributeUsage::POSITION, 2), //
				VertexAttribute(COLOR_ATTRIBUTE, AttributeUsage::COLOR, 4), //
				VertexAttribute(TEXCOORD_ATTRIBUTE, AttributeUsage::TEXCOORD, 2) //
				}),//

		indexBuffer(false), //
		vertices(bufferSize, 0.0f), //
		color(1.0f, 1.0f, 1.0f, 1.0f), //
		projectionMatrix(glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f, -1.0f, 1.0f)), //
		transformMatrix(1.0f), //
		combinedMatrix(1.0f) {
	if (program == nullptr) {
		this->ownedShaderProgram = SpriteBatch::createDefaultShader();

		if (ownedShaderProgram->hasError()) {
			setErrorState(
					std::string("Couldn't create default SpriteBatch shader: ")
							+ ownedShaderProgram->getErrorMessage());
			return;
		}

		this->program = ownedShaderProgram.get();
	} else {
		this->program = program;
	}

	const unsigned int indicesLength = bufferSize * 6;
	std::vector<uint16_t> indices(indicesLength, 0);

	uint16_t j = 0;
	for (unsigned int i = 0; i < indicesLength; i += 6, j += 4) {
		indices[i + 0] = j;
		indices[i + 1] = j + 1;
		indices[i + 2] = j + 2;
		indices[i + 3] = j + 2;
		indices[i + 4] = j + 3;
		indices[i + 5] = j;
	}

	indexBuffer.setData(indices, indices.size());
}

APG::SpriteBatch::~SpriteBatch() {
}

void APG::SpriteBatch::switchTexture(APG::Texture * const newTexture) {
	flush();
	lastTexture = newTexture;
}

void APG::SpriteBatch::setupMatrices() {
	combinedMatrix = projectionMatrix * transformMatrix;
	program->setUniformf("projTrans", combinedMatrix);
//	program->setUniformf("tex0", 0);
}

void APG::SpriteBatch::draw(APG::Texture * const image, float x, float y, uint32_t width,
		uint32_t height, float srcX, float srcY, uint32_t srcWidth, uint32_t srcHeight) {
//	if (hasError()) {
//		return;
//	} else if (!drawing) {
//		setErrorState("Trying to draw without beginning.");
//		return;
//	}
//
//	const float x1 = x;
//	const float y1 = y;
//
//	const float x2 = x + width;
//	const float y2 = y + height;
//
//	const float u1 = image->getInvWidth() * srcX;
//	const float v1 = image->getInvHeight() * srcY;
//
//	const float u2 = image->getInvWidth() * (srcX + srcWidth);
//	const float v2 = image->getInvHeight() * (srcY + srcHeight);
//
//	const float color = 1.0f;
//
//	Vertex verticest[4];
//
//	verticest[0].x = x1;
//	verticest[0].y = y1;
//	verticest[0].c = color;
//	verticest[0].u = u1;
//	verticest[0].v = v1;
//
//	verticest[1].x = x1;
//	verticest[1].y = y2;
//	verticest[1].c = color;
//	verticest[1].u = u1;
//	verticest[1].v = v2;
//
//	verticest[2].x = x2;
//	verticest[2].y = y2;
//	verticest[2].c = color;
//	verticest[2].u = u2;
//	verticest[2].v = v2;
//
//	verticest[3].x = x2;
//	verticest[3].y = y1;
//	verticest[3].c = color;
//	verticest[3].u = u2;
//	verticest[3].v = v1;
//
//	float verticesf[20];
//
//	for (int i = 0; i < 4; i++) {
//		verticesf[i * 5 + 0] = verticest[i].x;
//		verticesf[i * 5 + 1] = verticest[i].y;
//		verticesf[i * 5 + 2] = verticest[i].c;
//		verticesf[i * 5 + 3] = verticest[i].u;
//		verticesf[i * 5 + 4] = verticest[i].v;
//	}
}

void APG::SpriteBatch::draw(APG::Sprite * const sprite, float x, float y) {
	if (sprite->getTexture() != lastTexture) {
		switchTexture(sprite->getTexture());
	}

	vertices[idx++] = x;
	vertices[idx++] = y;
	vertices[idx++] = color.r;
	vertices[idx++] = color.g;
	vertices[idx++] = color.b;
	vertices[idx++] = color.a;
	vertices[idx++] = sprite->getU();
	vertices[idx++] = sprite->getV();

	vertices[idx++] = x;
	vertices[idx++] = y + sprite->getHeight();
	vertices[idx++] = color.r;
	vertices[idx++] = color.g;
	vertices[idx++] = color.b;
	vertices[idx++] = color.a;
	vertices[idx++] = sprite->getU();
	vertices[idx++] = sprite->getV2();

	vertices[idx++] = x + sprite->getWidth();
	vertices[idx++] = y + sprite->getHeight();
	vertices[idx++] = color.r;
	vertices[idx++] = color.g;
	vertices[idx++] = color.b;
	vertices[idx++] = color.a;
	vertices[idx++] = sprite->getU2();
	vertices[idx++] = sprite->getV2();

	vertices[idx++] = x + sprite->getWidth();
	vertices[idx++] = y;
	vertices[idx++] = color.r;
	vertices[idx++] = color.g;
	vertices[idx++] = color.b;
	vertices[idx++] = color.a;
	vertices[idx++] = sprite->getU2();
	vertices[idx++] = sprite->getV();
}

void APG::SpriteBatch::flush() {
	if (idx == 0 || hasError()) {
		return;
	}

//	for (unsigned int i = 0; i < idx; i++) {
//		std::cout << "vertices[" << i << "] = " << vertices[i] << std::endl;
//	}

	vao.bind();
	lastTexture->bind();
	program->use();
	vertexBuffer.setData(vertices, idx);
	vertexBuffer.bind(program);
	indexBuffer.bind();

	if (program->hasError()) {
		setErrorState(std::string("Error in shader: ") + program->getErrorMessage());
		idx = 0;
		return;
	}

	//TODO: Fix hacky sizeof.
	const int strideInBytes = (vertexBuffer.getAttributes().getStride() * sizeof(GLfloat));
	const int spriteCount = idx / strideInBytes;
	const int indexCount = spriteCount * 6;

	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, 0);

	idx = 0;
}

void APG::SpriteBatch::begin() {
	if (hasError()) {
		return;
	}

	drawing = true;
	setupMatrices();
}

void APG::SpriteBatch::end() {
	if (hasError()) {
		return;
	} else if (!drawing) {
		setErrorState("Trying to end before a begin.");
		return;
	}

	drawing = false;

	if (idx > 0) {
		flush();
	}
}

std::unique_ptr<APG::ShaderProgram> APG::SpriteBatch::createDefaultShader() {
	std::stringstream vertexShaderStream, fragmentShaderStream;

	vertexShaderStream << "#version 150 core\n" //
			<< "in vec2 " << POSITION_ATTRIBUTE << ";\n" //
			<< "in vec4 " << COLOR_ATTRIBUTE << ";\n" //
			<< "in vec2 " << TEXCOORD_ATTRIBUTE << ";\n" //
			<< "out vec2 frag_texcoord;\n" //
			<< "out vec4 frag_color;\n" //
			<< "uniform mat4 projTrans;\n" //
//			<< "uniform sampler2D tex0;\n" //
			<< "void main() {\n" //
			<< "frag_color = " << COLOR_ATTRIBUTE << ";\n" //
			<< "frag_texcoord = " << TEXCOORD_ATTRIBUTE << ";\n" //
			<< "gl_Position = projTrans * vec4(" << POSITION_ATTRIBUTE << ", 0.0, 1.0);" //
			<< "}\n\n";

	fragmentShaderStream << "#version 150 core\n" //
			<< "in vec4 frag_color;\n"  //
			<< "in vec2 frag_texcoord;\n"  //
			<< "out vec4 outColor;\n"  //
			<< "uniform sampler2D tex0;\n"  //
			<< "void main() {\n"  //
			<< "outColor = frag_color * texture(tex0, frag_texcoord);\n"  //
			<< "}\n\n";

	const auto vertexShader = vertexShaderStream.str();
	const auto fragmentShader = fragmentShaderStream.str();

	return ShaderProgram::fromSource(vertexShader, fragmentShader);
}
