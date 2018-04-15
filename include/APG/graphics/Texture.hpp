#ifndef INCLUDE_APG_GRAPHICS_TEXTURE_HPP_
#define INCLUDE_APG_GRAPHICS_TEXTURE_HPP_

#ifndef APG_NO_SDL
#ifndef APG_NO_GL

#include <cstdint>

#include <atomic>
#include <string>
#include <memory>

#include <GL/glew.h>
#include <glm/vec4.hpp>

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

class Texture {
public:
	explicit Texture(const char * const fileName, bool preserveSurface = false) :
			        Texture(std::string(fileName), preserveSurface) {
	}

	explicit Texture(const std::string &fileName, bool preserveSurface = false);
	explicit Texture(SDL_Surface * surface, bool preserveSurface = false);

	virtual ~Texture();

	void bind() const;

	void setWrapType(TextureWrapType sWrap, TextureWrapType tWrap);
	void setColor(glm::vec4 &color);
	void setFilter(TextureFilterType minFilter, TextureFilterType magFilter);

	void generateMipMaps();

	void attachToShader(const char * uniformName, ShaderProgram * program) const;
	void attachToShader(const std::string &uniformName, ShaderProgram * program) const {
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

	const std::string &getFileName() const {
		return fileName;
	}

private:
	std::string fileName;

	static uint32_t TEXTURE_TARGETS[];
	static std::atomic<uint32_t> availableTextureUnit;

	uint32_t textureID = 0;

	// the x at the end of the GL_TEXTUREx
	uint32_t textureUnitInt = 0;

	// the actual value of GL_TEXTUREx for some x
	uint32_t textureUnitGL = 0;

	void generateTextureID();
	void loadTexture(SDL_Surface * surface);

	int width = 0;
	int height = 0;

	float invWidth = 0.0f;
	float invHeight = 0.0f;

	bool preserveSurface = false;
	SXXDL::surface_ptr preservedSurface = SXXDL::make_surface_ptr(nullptr);

	GLuint tempBindID = 0;
	GLenum tempUnit = 0;
	void tempBind();
	void rebind();

	TextureWrapType sWrap;
	TextureWrapType tWrap;
	void uploadWrapType() const;

	TextureFilterType minFilter;
	TextureFilterType magFilter;
	void uploadFilter() const;
};

using texture_ptr = std::unique_ptr<Texture>;

}

#endif
#endif

#endif /* TEXTURE_HPP_ */
