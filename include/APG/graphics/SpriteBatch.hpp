#ifndef INCLUDE_APG_GRAPHICS_SPRITEBATCH_HPP_
#define INCLUDE_APG_GRAPHICS_SPRITEBATCH_HPP_

#ifndef APG_NO_GL

#include <cstdint>

#include <memory>

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "APG/graphics/Buffer.hpp"
#include "APG/graphics/Texture.hpp"
#include "APG/graphics/ShaderProgram.hpp"
#include "APG/graphics/VAO.hpp"
#include "APG/graphics/Mesh.hpp"
#include "APG/graphics/VertexBufferObject.hpp"
#include "APG/graphics/IndexBufferObject.hpp"
#include "APG/graphics/SpriteBase.hpp"

namespace APG {

class Sprite;

class SpriteBatch {
private:
	static const char * const POSITION_ATTRIBUTE;
	static const char * const COLOR_ATTRIBUTE;
	static const char * const TEXCOORD_ATTRIBUTE;

	const uint32_t bufferSize;

	bool drawing = false;

	VAO vao;
	VertexBufferObject vertexBuffer;
	IndexBufferObject indexBuffer;

	std::vector<float> vertices;

	std::unique_ptr<APG::ShaderProgram> ownedShaderProgram;
	ShaderProgram *program = nullptr;

	uint64_t idx = 0;

	Texture * lastTexture = nullptr;
	void switchTexture(Texture * newTexture);

	glm::vec4 color;
	glm::mat4 projectionMatrix;
	glm::mat4 transformMatrix;
	glm::mat4 combinedMatrix;
	void setupMatrices();

public:
	explicit SpriteBatch(const std::unique_ptr<ShaderProgram> &program, uint32_t bufferSize = DEFAULT_BUFFER_SIZE) :
			        SpriteBatch(program.get(), bufferSize) {
	}

	explicit SpriteBatch(ShaderProgram * program = nullptr, uint32_t bufferSize = DEFAULT_BUFFER_SIZE);
	~SpriteBatch() = default;

	void begin();
	void end();

	void flush();

	inline void draw(const std::unique_ptr<Texture> &image, float x, float y, uint32_t width, uint32_t height,
	        float srcX, float srcY, uint32_t srcWidth, uint32_t srcHeight) {
		draw(image.get(), x, y, width, height, srcX, srcY, srcWidth, srcHeight);
	}
	void draw(Texture * image, float x, float y, uint32_t width, uint32_t height, float srcX, float srcY,
	        uint32_t srcWidth, uint32_t srcHeight);

	inline void draw(const std::unique_ptr<SpriteBase> &sprite, float x, float y) {
		draw(sprite.get(), x, y);
	}

	void draw(SpriteBase * sprite, float x, float y);

	glm::vec4 getColor() const {
		return color;
	}

	inline void setColor(const glm::vec4 &newColor) {
		color = glm::vec4(newColor);
	}

	inline void setColor(float r, float g, float b, float a) {
		color.r = r;
		color.g = g;
		color.b = b;
		color.a = a;
	}

	void setProjectionMatrix(const glm::mat4 &matrix);

	static std::unique_ptr<APG::ShaderProgram> createDefaultShader();
	static const uint32_t DEFAULT_BUFFER_SIZE;
};

}

#endif

#endif /* SPRITEBATCH_HPP_ */
