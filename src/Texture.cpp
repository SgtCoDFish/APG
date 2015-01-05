/*
 * Texture.hpp
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

#include <atomic>
#include <string>
#include <sstream>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Texture.hpp"
#include "SXXDL.hpp"
#include "ShaderProgram.hpp"

std::atomic<uint32_t> APG::Texture::availableTextureUnit(0);
uint32_t APG::Texture::TEXTURE_TARGETS[] = { GL_TEXTURE0, GL_TEXTURE1, GL_TEXTURE2, GL_TEXTURE3,
GL_TEXTURE4, GL_TEXTURE5, GL_TEXTURE6, GL_TEXTURE7, GL_TEXTURE8, GL_TEXTURE9,
GL_TEXTURE10, GL_TEXTURE11, GL_TEXTURE12, GL_TEXTURE13, GL_TEXTURE14, GL_TEXTURE15,
GL_TEXTURE16, GL_TEXTURE17, GL_TEXTURE18, GL_TEXTURE19, GL_TEXTURE20, GL_TEXTURE21,
GL_TEXTURE22, GL_TEXTURE23, GL_TEXTURE24, GL_TEXTURE25, GL_TEXTURE26, GL_TEXTURE27,
GL_TEXTURE28, GL_TEXTURE29, GL_TEXTURE30, GL_TEXTURE31 };

APG::Texture::Texture(const std::string &fileName, bool preserveSurface) :
		preserveSurface(preserveSurface), //
		preservedSurface(SXXDL::make_surface_ptr(nullptr)), //
		sWrap(APG::TextureWrapType::CLAMP_TO_EDGE), //
		tWrap(APG::TextureWrapType::CLAMP_TO_EDGE), //
		minFilter(APG::TextureFilterType::LINEAR), //
		magFilter(APG::TextureFilterType::LINEAR) {
	textureUnitInt = availableTextureUnit++;
	textureUnitGL = TEXTURE_TARGETS[textureUnitInt];

	generateTextureID();
	loadTexture(fileName);
}

APG::Texture::~Texture() {
	glDeleteTextures(1, &textureID);
}

void APG::Texture::generateTextureID() {
	glGenTextures(1, &textureID);
}

void APG::Texture::loadTexture(const std::string &fileName) {
	auto surface = SXXDL::make_surface_ptr(IMG_Load(fileName.c_str()));

	if (surface == nullptr) {
		setErrorState(std::string("Could not load ") + fileName);
		return;
	}

	const auto numberOfColors = surface->format->BytesPerPixel;
	int32_t glFormat = 0;

	if (numberOfColors == 4) {
		if (surface->format->Rmask == 0x000000ff) {
			glFormat = GL_RGBA;
		} else {
			glFormat = GL_BGRA;
		}
	} else if (numberOfColors == 3) {
		if (surface->format->Rmask == 0x000000ff) {
			glFormat = GL_RGB;
		} else {
			glFormat = GL_BGR;
		}
	} else {
		std::stringstream errStream;

		errStream << "Bytes per color in " << fileName << " is invalid (" << numberOfColors << ").";
		setErrorState(errStream.str());
		return;
	}

	// loaded successfully, upload to graphics card

	tempBind();

	glTexImage2D(GL_TEXTURE_2D, 0, glFormat, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
			surface->pixels);

	rebind();

	auto glError = glGetError();

	if (glError != GL_NO_ERROR) {
		std::stringstream errStream;
		errStream << "Couldn't upload " << fileName << ": OpenGL Error:\n";

		while (glError != GL_NO_ERROR) {
			errStream << gluErrorString(glError);

			glError = glGetError();
		}

		setErrorState(errStream.str());
		return;
	}

	width = surface->w;
	height = surface->h;

	invWidth = 1.0f / width;
	invHeight = 1.0f / height;

	if (preserveSurface) {
		preservedSurface = std::move(surface);
	}
}

void APG::Texture::tempBind() {
	// NOTE: consider using the EXT_direct_state_access extension if available
	// using glTextureParameteri(id, texType, paramName, paramVal)
	// http://www.opengl.org/registry/specs/EXT/direct_state_access.txt

	glGetIntegerv(GL_TEXTURE_BINDING_2D, (int32_t *) &tempBindID);
	glGetIntegerv(GL_ACTIVE_TEXTURE, (int32_t *) &tempUnit);
	glActiveTexture(textureUnitGL);
	glBindTexture(GL_TEXTURE_2D, textureID);
}

void APG::Texture::rebind() {
	glActiveTexture(tempUnit);
	glBindTexture(GL_TEXTURE_2D, tempBindID);
	tempBindID = tempUnit = 0;
}

void APG::Texture::bind() const {
	if (hasError()) {
		return;
	}

	glActiveTexture(textureUnitGL);
	glBindTexture(GL_TEXTURE_2D, textureID);
	uploadFilter();
	uploadWrapType();
}

void APG::Texture::setWrapType(TextureWrapType sWrap, TextureWrapType tWrap) {
	tempBind();

	this->sWrap = sWrap;
	this->tWrap = tWrap;

	uploadWrapType();

	rebind();
}

void APG::Texture::uploadWrapType() const {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrap);
}

void APG::Texture::setColor(glm::vec4 &color) {
	tempBind();

	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(color));

	rebind();
}

void APG::Texture::setFilter(TextureFilterType minFilter, TextureFilterType magFilter) {
	tempBind();

	if (magFilter == TextureFilterType::LINEAR_MIPMAP_LINEAR
			|| magFilter == TextureFilterType::NEAREST_MIPMAP_LINEAR
			|| magFilter == TextureFilterType::LINEAR_MIPMAP_NEAREST
			|| magFilter == TextureFilterType::NEAREST_MIPMAP_NEAREST) {
		setErrorState("Can't set mipmap filter for mag filter.");
		rebind();
		return;
	}

	this->minFilter = minFilter;
	this->magFilter = magFilter;

	uploadFilter();

	rebind();
}

void APG::Texture::uploadFilter() const {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
}

void APG::Texture::generateMipMaps() {
	tempBind();

	glGenerateMipmap(GL_TEXTURE_2D);

	rebind();
}

void APG::Texture::attachToShader(const char * const uniformName,
		ShaderProgram * const program) const {
	program->setUniformi(uniformName, textureUnitInt);
}
