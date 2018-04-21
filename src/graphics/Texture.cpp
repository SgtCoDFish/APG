#ifndef APG_NO_SDL
#ifndef APG_NO_GL

#include <cstdint>

#include <atomic>
#include <string>

#include "APG/GL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "APG/SDL.hpp"

#include "APG/SXXDL.hpp"
#include "APG/graphics/Texture.hpp"
#include "APG/graphics/GLError.hpp"
#include "APG/graphics/ShaderProgram.hpp"
#include "APG/internal/Assert.hpp"

#include "easylogging++.h"

namespace APG {
std::atomic<uint32_t> Texture::availableTextureUnit(0);
uint32_t Texture::TEXTURE_TARGETS[] = {
		GL_TEXTURE0, GL_TEXTURE1, GL_TEXTURE2, GL_TEXTURE3,
		GL_TEXTURE4, GL_TEXTURE5, GL_TEXTURE6, GL_TEXTURE7, GL_TEXTURE8,
		GL_TEXTURE9,
		GL_TEXTURE10, GL_TEXTURE11, GL_TEXTURE12, GL_TEXTURE13, GL_TEXTURE14,
		GL_TEXTURE15,
		GL_TEXTURE16, GL_TEXTURE17, GL_TEXTURE18, GL_TEXTURE19, GL_TEXTURE20,
		GL_TEXTURE21,
		GL_TEXTURE22, GL_TEXTURE23, GL_TEXTURE24, GL_TEXTURE25, GL_TEXTURE26,
		GL_TEXTURE27,
		GL_TEXTURE28, GL_TEXTURE29, GL_TEXTURE30, GL_TEXTURE31
};

Texture::Texture(const std::string &fileName) :
		fileName{fileName},
		sWrap{APG::TextureWrapType::CLAMP_TO_EDGE},
		tWrap{APG::TextureWrapType::CLAMP_TO_EDGE},
		minFilter{APG::TextureFilterType::LINEAR},
		magFilter{APG::TextureFilterType::LINEAR} {
	auto surface = IMG_Load(fileName.c_str());

	if (surface == nullptr) {
		el::Loggers::getLogger("APG")->fatal("Couldn't load %v, SDL_image error: %v", fileName, IMG_GetError());
		return;
	}

	generateTextureID();
	loadTexture(surface);
}

Texture::Texture(SDL_Surface *surface) :
		fileName{"from SDL_Surface"},
		sWrap{APG::TextureWrapType::CLAMP_TO_EDGE},
		tWrap{APG::TextureWrapType::CLAMP_TO_EDGE},
		minFilter{APG::TextureFilterType::LINEAR},
		magFilter{APG::TextureFilterType::LINEAR} {
	REQUIRE(surface != nullptr, "Can't create texture from null surface ptr.");

	generateTextureID();

	loadTexture(surface);
}

Texture::Texture() :
		fileName{"raw derived texture"},
		sWrap{APG::TextureWrapType::CLAMP_TO_EDGE},
		tWrap{APG::TextureWrapType::CLAMP_TO_EDGE},
		minFilter{APG::TextureFilterType::LINEAR},
		magFilter{APG::TextureFilterType::LINEAR} {
	generateTextureID();
}

Texture::~Texture() {
	glDeleteTextures(1, &textureID);
}

void Texture::generateTextureID() {
	textureUnitInt = availableTextureUnit++;
	textureUnitGL = TEXTURE_TARGETS[textureUnitInt];

	glGenTextures(1, &textureID);
}

void Texture::loadTexture(SDL_Surface *surface, bool andPreserve) {
	const auto logger = el::Loggers::getLogger("APG");

	if (surface == nullptr) {
		logger->fatal("Call to loadTexture with null surface");
		return;
	}

	const auto numberOfColors = surface->format->BytesPerPixel;
	GLenum glFormat = 0;

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
		logger->fatal(
				"Invalid bytes per pixel value in %v (%v is invalid, only 4 or 3 supported.)",
				fileName,
				(uint32_t) numberOfColors
		);
		return;
	}

	// loaded successfully, upload to graphics card
	logger->info("Uploading texture: %vx%v", surface->w, surface->h);

	tempBind();

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, surface->w, surface->h, 0, glFormat, GL_UNSIGNED_BYTE, surface->pixels);

	rebind();

	auto glError = glGetError();

	if (glError != GL_NO_ERROR) {
		while (glError != GL_NO_ERROR) {
			logger->fatal("GL error while uploading texture: %v", prettyGLError(glError));

			glError = glGetError();
		}

		return;
	}

	width = surface->w;
	height = surface->h;

	invWidth = 1.0f / width;
	invHeight = 1.0f / height;

	logger->info("Loaded texture \"%v\" at unit GL_TEXTURE%v", fileName, textureUnitInt);

	if (andPreserve) {
		preservedSurface = SXXDL::make_surface_ptr(surface);
	}
}

void Texture::tempBind() {
	// TODO: consider using the EXT_direct_state_access extension if available
	// using glTextureParameteri(id, texType, paramName, paramVal)
	// http://www.opengl.org/registry/specs/EXT/direct_state_access.txt

	glGetIntegerv(GL_TEXTURE_BINDING_2D, reinterpret_cast<GLint *>(&tempBindID));
	glGetIntegerv(GL_ACTIVE_TEXTURE, reinterpret_cast<GLint *>(&tempUnit));
	glActiveTexture(textureUnitGL);
	glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::rebind() {
	glActiveTexture(tempUnit);
	glBindTexture(GL_TEXTURE_2D, tempBindID);
	tempBindID = tempUnit = 0;
}

void Texture::bind() const {
	glActiveTexture(textureUnitGL);
	glBindTexture(GL_TEXTURE_2D, textureID);
	uploadFilter();
	uploadWrapType();
}

void Texture::setWrapType(TextureWrapType sWrap, TextureWrapType tWrap) {
	tempBind();

	this->sWrap = sWrap;
	this->tWrap = tWrap;

	uploadWrapType();

	rebind();
}

void Texture::uploadWrapType() const {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrap);
}

void Texture::setColor(glm::vec4 &color) {
	tempBind();

	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(color));

	rebind();
}

void Texture::setFilter(TextureFilterType minFilter, TextureFilterType magFilter) {
	tempBind();

	REQUIRE(
			magFilter != TextureFilterType::LINEAR_MIPMAP_LINEAR &&
			magFilter != TextureFilterType::NEAREST_MIPMAP_LINEAR &&
			magFilter != TextureFilterType::LINEAR_MIPMAP_NEAREST &&
			magFilter != TextureFilterType::NEAREST_MIPMAP_NEAREST,
			"Can't set mipmap filter for mag filter.");

	this->minFilter = minFilter;
	this->magFilter = magFilter;

	uploadFilter();

	rebind();
}

void Texture::uploadFilter() const {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
}

void Texture::generateMipMaps() {
	tempBind();

	glGenerateMipmap(GL_TEXTURE_2D);

	rebind();
}

void Texture::attachToShader(const char *const uniformName, ShaderProgram *const program) const {
	program->setUniformi(uniformName, textureUnitInt);
}

Sprite Texture::makeSprite(const SDL_Rect &rect) {
	return Sprite(this, rect.x, rect.y, rect.w, rect.h);
}

std::unique_ptr<Sprite> Texture::makeSpritePtr(const SDL_Rect &rect) {
	return std::make_unique<Sprite>(this, rect.x, rect.y, rect.w, rect.h);
}

}

#endif
#endif
