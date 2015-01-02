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

#include <glm/vec2.hpp>

#include "ErrorBase.hpp"
#include "SpriteBatch.hpp"
#include "ShaderProgram.hpp"
#include "Buffer.hpp"
#include "APGCommon.hpp"

const char * const APG::SpriteBatch::POSITION_ATTRIBUTE = "position";
const char * const APG::SpriteBatch::COLOR_ATTRIBUTE = "color";
const char * const APG::SpriteBatch::TEXCOORD_ATTRIBUTE = "texcoord";
const uint32_t APG::SpriteBatch::DEFAULT_BUFFER_SIZE = 1000;

uint32_t APG::SpriteBatch::indices[6] = {0, 1, 2, 2, 3, 0};

APG::SpriteBatch::SpriteBatch(uint32_t bufferSize, ShaderProgram * const program) :
		bufferSize(bufferSize), vertexBuffer(BufferType::ARRAY, DrawType::DYNAMIC_DRAW), indexBuffer(BufferType::ELEMENT_ARRAY, DrawType::STATIC_DRAW) {
	if (program == nullptr) {
		this->ownedShaderProgram = SpriteBatch::createDefaultShader();
		this->program = ownedShaderProgram.get();
	} else {
		this->program = program;
	}

	vao = std::make_unique<VAO>();

	indexBuffer.setData(indices, 6);
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

	Vertex vertices[4];

	vertices[0].x = x1;
	vertices[0].y = y1;
	vertices[0].c = color;
	vertices[0].u = u1;
	vertices[0].v = v1;

	vertices[1].x = x1;
	vertices[1].y = y2;
	vertices[1].c = color;
	vertices[1].u = u1;
	vertices[1].v = v2;

	vertices[2].x = x2;
	vertices[2].y = y2;
	vertices[2].c = color;
	vertices[2].u = u2;
	vertices[2].v = v2;

	vertices[3].x = x2;
	vertices[3].y = y1;
	vertices[3].c = color;
	vertices[3].u = u2;
	vertices[3].v = v1;

	float verticesf[20];

	for(int i = 0; i < 4; i++) {
		verticesf[i * 5 + 0] = vertices[i].x;
		verticesf[i * 5 + 1] = vertices[i].y;
		verticesf[i * 5 + 2] = vertices[i].c;
		verticesf[i * 5 + 3] = vertices[i].u;
		verticesf[i * 5 + 4] = vertices[i].v;
	}

	vertexBuffer.setData(verticesf, 20);
	image->bind();
	flush();
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
