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

#ifndef APGGLTEXTURE_HPP_
#define APGGLTEXTURE_HPP_

#include <cstdint>

#include <atomic>
#include <string>
#include <memory>

#include <GL/glew.h>
#include <glm/vec4.hpp>

#include "ErrorBase.hpp"
#include "SXXDL.hpp"

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

	// the x at the end of the GL_TEXTUREx
	uint32_t textureUnitInt = 0;

	// the actual value of GL_TEXTUREx for some x
	uint32_t textureUnitGL = 0;

	void generateTextureID();
	void loadTexture(const std::string &fileName);

	int32_t width = 0;
	int32_t height = 0;

	float invWidth = 0.0f;
	float invHeight = 0.0f;

	bool preserveSurface = false;
	SXXDL::surface_ptr preservedSurface;

	int32_t tempBindID = 0;
	int32_t tempUnit = 0;
	void tempBind();
	void rebind();

	TextureWrapType sWrap;
	TextureWrapType tWrap;
	void uploadWrapType() const;

	TextureFilterType minFilter;
	TextureFilterType magFilter;
	void uploadFilter() const;

public:
	explicit Texture(const char * const fileName, bool preserveSurface = false) :
			Texture(std::string(fileName), preserveSurface) {
	}

	explicit Texture(const std::string &fileName, bool preserveSurface = false);
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

	inline int32_t getWidth() const {
		return width;
	}

	inline int32_t getHeight() const {
		return height;
	}

	inline float getInvWidth() const {
		return invWidth;
	}

	inline float getInvHeight() const {
		return invHeight;
	}

	SDL_Surface *getPreservedSurface() const {
		return (preserveSurface ? preservedSurface.get() : nullptr);
	}

	inline uint32_t getGLTextureUnit() const {
		return textureUnitInt;
	}
};

using texture_ptr = std::unique_ptr<Texture>;

}

#endif /* TEXTURE_HPP_ */
