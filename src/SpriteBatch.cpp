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

const char * const APG::SpriteBatch::POSITION_ATTRIBUTE = "position";
const char * const APG::SpriteBatch::COLOR_ATTRIBUTE = "color";
const char * const APG::SpriteBatch::TEXCOORD_ATTRIBUTE = "texcoord";
const uint32_t APG::SpriteBatch::DEFAULT_BUFFER_SIZE = 1000;

APG::SpriteBatch::SpriteBatch(uint32_t bufferSize, ShaderProgram * const program) :
		bufferSize(bufferSize), vertexBuffer(BufferType::ARRAY, DrawType::DYNAMIC_DRAW) {
	if (program == nullptr) {
		this->ownedShaderProgram = SpriteBatch::createDefaultShader();
		this->program = ownedShaderProgram.get();
	} else {
		this->program = program;
	}
}

APG::SpriteBatch::~SpriteBatch() {
}

void APG::SpriteBatch::draw(APG::Texture * const image, glm::vec2::type x, glm::vec2::type y,
		glm::vec2::type width, glm::vec2::type height, glm::vec2::type srcX, glm::vec2::type srcY,
		glm::vec2::type srcWidth, glm::vec2::type srcHeight) {

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
			<< "outColor = frag_color * texture(texture, frag_texcoord);"  //
			<< "}\n\n";

	const auto vertexShader = vertexShaderStream.str();
	const auto fragmentShader = fragmentShaderStream.str();

	return ShaderProgram::fromSource(vertexShader, fragmentShader);
}
