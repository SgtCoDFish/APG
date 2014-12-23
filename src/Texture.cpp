/*
 * Texture.hpp
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

#include <cstdint>

#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Texture.hpp"

APG::Texture::Texture(const std::string &fileName) {
	generateTextureID();
	loadTexture(fileName);
}

void APG::Texture::generateTextureID() {
	glGenTextures(1, &textureID);
}

void APG::Texture::loadTexture(const std::string &fileName) {

}

void APG::Texture::bind() const {
	if(hasError()) {
		return;
	}

	glBindTexture(GL_TEXTURE_2D, textureID);
}

void APG::Texture::setWrapType(TextureWrapType sWrap, TextureWrapType tWrap) const {
	bind();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrap);
}

void APG::Texture::setColor(glm::vec4 &color) const {
	bind();

	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(color));
}

void APG::Texture::setFilter(TextureFilterType minFilter, TextureFilterType magFilter) const {
	bind();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
}

void APG::Texture::generateMipMaps() const {
	bind();

	glGenerateMipmap(GL_TEXTURE_2D);
}
