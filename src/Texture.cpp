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

#include <cstdint>

#include <atomic>
#include <string>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "APG/Texture.hpp"
#include "APG/SXXDL.hpp"
#include "APG/ShaderProgram.hpp"

#include "APG/internal/Assert.hpp"

std::atomic<uint32_t> APG::Texture::availableTextureUnit(0);
uint32_t APG::Texture::TEXTURE_TARGETS[] = { GL_TEXTURE0, GL_TEXTURE1, GL_TEXTURE2, GL_TEXTURE3,
GL_TEXTURE4, GL_TEXTURE5, GL_TEXTURE6, GL_TEXTURE7, GL_TEXTURE8, GL_TEXTURE9,
GL_TEXTURE10, GL_TEXTURE11, GL_TEXTURE12, GL_TEXTURE13, GL_TEXTURE14, GL_TEXTURE15,
GL_TEXTURE16, GL_TEXTURE17, GL_TEXTURE18, GL_TEXTURE19, GL_TEXTURE20, GL_TEXTURE21,
GL_TEXTURE22, GL_TEXTURE23, GL_TEXTURE24, GL_TEXTURE25, GL_TEXTURE26, GL_TEXTURE27,
GL_TEXTURE28, GL_TEXTURE29, GL_TEXTURE30, GL_TEXTURE31 };

APG::Texture::Texture(const std::string &fileName, bool preserveSurface) :
		fileName { fileName }, //
		preserveSurface { preserveSurface }, //
		preservedSurface { SXXDL::make_surface_ptr(nullptr) }, //
		sWrap { APG::TextureWrapType::CLAMP_TO_EDGE }, //
		tWrap { APG::TextureWrapType::CLAMP_TO_EDGE }, //
		minFilter { APG::TextureFilterType::LINEAR }, //
		magFilter { APG::TextureFilterType::LINEAR } {
	textureUnitInt = availableTextureUnit++;
	textureUnitGL = TEXTURE_TARGETS[textureUnitInt];

	generateTextureID();
	loadTexture();
}

APG::Texture::~Texture() {
	glDeleteTextures(1, &textureID);
}

void APG::Texture::generateTextureID() {
	glGenTextures(1, &textureID);
}

void APG::Texture::loadTexture() {
	const auto logger = el::Loggers::getLogger("default");
	auto surface = SXXDL::make_surface_ptr(IMG_Load(fileName.c_str()));

	if (surface == nullptr) {
		logger->fatal("Couldn't load %v, SDL_image error: %v", fileName, IMG_GetError());
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
		logger->fatal("Invalid bytes per color value in %v (%v is invalid, only 4 or 3 supported.)", fileName, numberOfColors);
		return;
	}

	// loaded successfully, upload to graphics card

	tempBind();

	glTexImage2D(GL_TEXTURE_2D, 0, glFormat, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

	rebind();

	auto glError = glGetError();

	if (glError != GL_NO_ERROR) {
		while (glError != GL_NO_ERROR) {
			logger->fatal("GL error while uploading texture: %v", gluErrorString(glError));

			glError = glGetError();
		}

		return;
	}

	width = surface->w;
	height = surface->h;

	invWidth = 1.0f / width;
	invHeight = 1.0f / height;

	if (preserveSurface) {
		preservedSurface = std::move(surface);
	}

	logger->info("Loaded texture \"%v\" at unit GL_TEXTURE%v\"", fileName, textureUnitInt);
}

void APG::Texture::tempBind() {
	// TODO: consider using the EXT_direct_state_access extension if available
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

	REQUIRE(
	        magFilter != TextureFilterType::LINEAR_MIPMAP_LINEAR
	                && magFilter != TextureFilterType::NEAREST_MIPMAP_LINEAR
	                && magFilter != TextureFilterType::LINEAR_MIPMAP_NEAREST
	                && magFilter != TextureFilterType::NEAREST_MIPMAP_NEAREST,
	        "Can't set mipmap filter for mag filter.");

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

void APG::Texture::attachToShader(const char * const uniformName, ShaderProgram * const program) const {
	program->setUniformi(uniformName, textureUnitInt);
}
