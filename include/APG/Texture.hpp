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

#ifndef APGGLTEXTURE_HPP_
#define APGGLTEXTURE_HPP_

#include <cstdint>

#include <atomic>
#include <string>
#include <memory>

#include <GL/glew.h>
#include <glm/vec4.hpp>

#include "APG/ErrorBase.hpp"
#include "APG/SXXDL.hpp"

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

	uint32_t width = 0;
	uint32_t height = 0;

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
	virtual ~Texture();

	void bind() const;

	void setWrapType(TextureWrapType sWrap, TextureWrapType tWrap);
	void setColor(glm::vec4 &color);
	void setFilter(TextureFilterType minFilter, TextureFilterType magFilter);

	void generateMipMaps();

	void attachToShader(const char * const uniformName, ShaderProgram * const program) const;
	void attachToShader(const std::string &uniformName, ShaderProgram * const program) const {
		attachToShader(uniformName.c_str(), program);
	}

	inline uint32_t getWidth() const {
		return width;
	}

	inline uint32_t getHeight() const {
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
