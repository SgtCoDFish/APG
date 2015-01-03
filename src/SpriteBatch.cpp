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
#include <new>
#include <utility>
#include <memory>

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
		bufferSize(bufferSize), vertexBuffer(BufferType::ARRAY, DrawType::DYNAMIC_DRAW), indexBuffer(
				BufferType::ELEMENT_ARRAY, DrawType::STATIC_DRAW) {
	if (program == nullptr) {
		this->ownedShaderProgram = SpriteBatch::createDefaultShader();
		this->program = ownedShaderProgram.get();
	} else {
		this->program = program;
	}

	vao = std::make_unique<VAO>();

	vertices = std::unique_ptr<float[]>(new float[bufferSize * APG::VERTEX_SIZE]);

	const int indicesLength = bufferSize * 6;
	uint16_t *indices = nullptr;

	try {
		indices = new uint16_t[indicesLength];
	} catch (std::bad_alloc &ba) {
		setErrorState("Couldn't allocate for index array.");
		return;
	}

	uint16_t j = 0;
	for (int i = 0; i < indicesLength; i += 6, j += 4) {
		indices[i + 0] = j;
		indices[i + 1] = j + 1;
		indices[i + 2] = j + 2;
		indices[i + 3] = j + 2;
		indices[i + 4] = j + 3;
		indices[i + 5] = j;
	}

	mesh = std::unique_ptr<Mesh>(new Mesh{VertexAttribute(POSITION_ATTRIBUTE, AttributeUsage::POSITION, 2)});
}

APG::SpriteBatch::~SpriteBatch() {
}

void APG::SpriteBatch::draw(APG::Texture * const image, float x, float y, uint32_t width,
		uint32_t height, float srcX, float srcY, uint32_t srcWidth, uint32_t srcHeight) {
	if (hasError()) {
		return;
	} else if (!drawing) {
		setErrorState("Trying to draw without beginning.");
		return;
	}

	const float x1 = x;
	const float y1 = y;

	const float x2 = x + width;
	const float y2 = y + height;

	const float u1 = image->getInvWidth() * srcX;
	const float v1 = image->getInvHeight() * srcY;

	const float u2 = image->getInvWidth() * (srcX + srcWidth);
	const float v2 = image->getInvHeight() * (srcY + srcHeight);

	const float color = 1.0f;

	Vertex verticest[4];

	verticest[0].x = x1;
	verticest[0].y = y1;
	verticest[0].c = color;
	verticest[0].u = u1;
	verticest[0].v = v1;

	verticest[1].x = x1;
	verticest[1].y = y2;
	verticest[1].c = color;
	verticest[1].u = u1;
	verticest[1].v = v2;

	verticest[2].x = x2;
	verticest[2].y = y2;
	verticest[2].c = color;
	verticest[2].u = u2;
	verticest[2].v = v2;

	verticest[3].x = x2;
	verticest[3].y = y1;
	verticest[3].c = color;
	verticest[3].u = u2;
	verticest[3].v = v1;

	float verticesf[20];

	for (int i = 0; i < 4; i++) {
		verticesf[i * 5 + 0] = verticest[i].x;
		verticesf[i * 5 + 1] = verticest[i].y;
		verticesf[i * 5 + 2] = verticest[i].c;
		verticesf[i * 5 + 3] = verticest[i].u;
		verticesf[i * 5 + 4] = verticest[i].v;
	}
}

void APG::SpriteBatch::draw(APG::Sprite *sprite, float x, float y) {
	const float color = 1.0f;
	Vertex verticest[4];

	verticest[0].x = x;
	verticest[0].y = y;
	verticest[0].c = color;
	verticest[0].u = sprite->getU();
	verticest[0].v = sprite->getV();

	verticest[1].x = x;
	verticest[1].y = y + sprite->getHeight();
	verticest[1].c = color;
	verticest[1].u = sprite->getU();
	verticest[1].v = sprite->getV2();

	verticest[2].x = x + sprite->getWidth();
	verticest[2].y = y + sprite->getHeight();
	verticest[2].c = color;
	verticest[2].u = sprite->getU2();
	verticest[2].v = sprite->getV2();

	verticest[3].x = x + sprite->getWidth();
	verticest[3].y = y;
	verticest[3].c = color;
	verticest[3].u = sprite->getU2();
	verticest[3].v = sprite->getV();

	float verticesf[20];

	for (int i = 0; i < 4; i++) {
		verticesf[i * 5 + 0] = verticest[i].x;
		verticesf[i * 5 + 1] = verticest[i].y;
		verticesf[i * 5 + 2] = verticest[i].c;
		verticesf[i * 5 + 3] = verticest[i].u;
		verticesf[i * 5 + 4] = verticest[i].v;
	}
}

void APG::SpriteBatch::flush() {
	vertexBuffer.upload();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

std::unique_ptr<APG::ShaderProgram> APG::SpriteBatch::createDefaultShader() {
	std::stringstream vertexShaderStream, fragmentShaderStream;

	vertexShaderStream << "#version 150 core\n" //
			<< "in vec2 " << POSITION_ATTRIBUTE << ";\n" //
			<< "in vec4 " << COLOR_ATTRIBUTE << ";\n" //
			<< "in vec2 " << TEXCOORD_ATTRIBUTE << ";\n" //
			<< "out vec2 frag_texcoord;\n" //
			<< "out vec4 frag_color;\n" //
			<< "void main() {" //
			<< "frag_color = " << COLOR_ATTRIBUTE << ";\n" //
			<< "frag_texcoord = " << TEXCOORD_ATTRIBUTE << ";\n" //
			<< "gl_Position = vec4(" << POSITION_ATTRIBUTE << ", 0.0, 1.0);" //
			<< "}\n\n";

	fragmentShaderStream << "#version 150 core\n" //
			<< "in vec4 frag_color;\n"  //
			<< "in vec2 frag_texcoord;\n"  //
			<< "out vec4 outColor;"  //
			<< "uniform sampler2D texture;"  //
			<< "void main() {\n"  //
			<< "outColor = color * texture(texture, frag_texcoord);"  //
			<< "}\n\n";

	const auto vertexShader = vertexShaderStream.str();
	const auto fragmentShader = fragmentShaderStream.str();

	return ShaderProgram::fromSource(vertexShader, fragmentShader);
}

void APG::SpriteBatch::begin() {
	if (hasError()) {
		return;
	}

	drawing = true;
	vao->bind();
	indexBuffer.upload();
	vertexBuffer.bind();
	program->use();
	program->setFloatAttribute(POSITION_ATTRIBUTE, 2, 5, 0);
	program->setFloatAttribute(COLOR_ATTRIBUTE, 1, 5, 2);
	program->setFloatAttribute(TEXCOORD_ATTRIBUTE, 2, 5, 3);
}

void APG::SpriteBatch::end() {
	if (hasError()) {
		return;
	} else if (!drawing) {
		setErrorState("Trying to end before a begin.");
		return;
	}

	drawing = false;
}
