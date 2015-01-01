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

#ifndef TEXTURE_HPP_
#define TEXTURE_HPP_

#include <cstdint>

#include <atomic>
#include <string>

#include <GL/glew.h>
#include <glm/vec4.hpp>

#include "ErrorBase.hpp"

namespace APG {

enum TextureWrapType {
	REPEAT = GL_REPEAT,
	CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER,
	CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
	MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
};

enum TextureFilterType {
	NEAREST = GL_NEAREST,
	LINEAR = GL_LINEAR,
	NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
	LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
	NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
	LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR,
};

class ShaderProgram;

class Texture : public ErrorBase {
private:
	static uint32_t TEXTURE_TARGETS[];
	static std::atomic<uint32_t> availableTextureUnit;

	uint32_t textureID = 0;
	uint32_t textureUnit = 0;
	uint32_t glTextureID = 0;

	void generateTextureID();
	void loadTexture(const std::string &fileName);

	int32_t tempBindID = 0;
	int32_t tempUnit = 0;
	void tempBind();
	void rebind();

public:
	explicit Texture(const char * const fileName) :
			Texture(std::string(fileName)) {
	}

	explicit Texture(const std::string &fileName);
	~Texture();

	void bind() const;

	void setWrapType(TextureWrapType sWrap, TextureWrapType tWrap);
	void setColor(glm::vec4 &color);
	void setFilter(TextureFilterType minFilter, TextureFilterType magFilter);

	void generateMipMaps();

	void attachToShader(const char * const uniformName, ShaderProgram * const program) const;
	void attachToShader(const std::string &uniformName, ShaderProgram * const program) const {
		attachToShader(uniformName.c_str(), program);
	}
};

}

#endif /* TEXTURE_HPP_ */
