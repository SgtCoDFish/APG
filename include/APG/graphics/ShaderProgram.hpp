#ifndef INCLUDE_APG_GRAPHICS_SHADERPROGRAM_HPP_
#define INCLUDE_APG_GRAPHICS_SHADERPROGRAM_HPP_

#ifndef APG_NO_GL

#include <cstdint>

#include <string>
#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "spdlog/spdlog.h"

namespace APG {

class VertexAttribute;
class VertexAttributeList;

class ShaderProgram final {
public:
	static std::unique_ptr<APG::ShaderProgram> fromSource(const std::string &vertexShaderSource,
	        const std::string &fragmentShaderSource);
	static std::unique_ptr<APG::ShaderProgram> fromFiles(const std::string &vertexShaderFilename,
	        const std::string &fragmentShaderFilename);

	explicit ShaderProgram(const std::string &vertexShaderSource, const std::string &fragmentShaderSource);
	virtual ~ShaderProgram();

	void use();

	void setVertexAttribute(const VertexAttribute &vertexAttribute, uint16_t stride);
	void setVertexAttributes(const VertexAttributeList &attributeList);

	/**
	 * Sets a float attribute directly. Can make calculations a little more complicated,
	 * better to set via a VertexAttribute.
	 * @param attributeName the alias of the attribute, e.g. "position"
	 * @param valueCount the amount of floats this attribute has, between 1-4 inclusive.
	 * @param strideInElements the stride in elements (the total number of elements passed in)
	 * @param offsetInElements the offset in elements (the number of elements before this one)
	 * @param normalize whether to normalize between -1.0f - 1.0f, default false.
	 */
	void setFloatAttribute(const char * const attributeName, uint8_t valueCount, uint32_t strideInElements,
	        uint32_t offsetInElements, bool normalize = false);

	void setUniformf(const char * const uniformName, std::initializer_list<float> vals);
	void setUniformf(const char * const uniformName, float val);
	void setUniformf(const char * const uniformName, const glm::vec2 &vals);
	void setUniformf(const char * const uniformName, const glm::vec3 &vals);
	void setUniformf(const char * const uniformName, const glm::vec4 &vals);
	void setUniformf(const char * const uniformName, const glm::mat4 &mat);

	void setUniformi(const char * const uniformName, std::initializer_list<int32_t> vals);
	void setUniformi(const char * const uniformName, int32_t val);
	void setUniformi(const char * const uniformName, const glm::ivec2 &vals);
	void setUniformi(const char * const uniformName, const glm::ivec3 &vals);
	void setUniformi(const char * const uniformName, const glm::ivec4 &vals);

	uint32_t getProgramID() const {
		return shaderProgram;
	}

	std::string getShaderInfoLog() const {
		return shaderInfoLog;
	}

	std::string getLinkInfoLog() const {
		return linkInfoLog;
	}

	ShaderProgram() = delete;
	ShaderProgram(ShaderProgram &other) = delete;
	ShaderProgram(const ShaderProgram &other) = delete;
	ShaderProgram &operator=(ShaderProgram &other) = delete;
	ShaderProgram &operator=(const ShaderProgram &other) = delete;

private:
	static std::string loadSourceFromFile(const std::string &filename);

	uint32_t vertexShader, fragmentShader;
	uint32_t shaderProgram;

	uint32_t *validateTypeAndGet(uint32_t shaderType);

	std::string shaderInfoLog, linkInfoLog;

	std::shared_ptr<spdlog::logger> logger;

	void loadShader(const std::string &vertexShaderFilename, uint32_t shaderType);
	void combineProgram();
};

}

#endif

#endif
