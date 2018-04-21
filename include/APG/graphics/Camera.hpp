#ifndef INCLUDE_APG_GRAPHICS_CAMERA_HPP_
#define INCLUDE_APG_GRAPHICS_CAMERA_HPP_

#include <cstdint>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace APG {

/**
 * An orthographic camera, heavily inspired by LibGDX
 *
 * https://github.com/libgdx/libgdx/blob/master/gdx/src/com/badlogic/gdx/graphics/OrthographicCamera.java
 */
class Camera {
public:
	explicit Camera(float viewportWidth, float viewportHeight);

	~Camera() = default;

	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 up;

	glm::mat4 projectionMatrix;
	glm::mat4 transformMatrix;
	glm::mat4 combinedMatrix;

	void update();

	void setToOrtho(bool yDown, float viewportWidth, float viewportHeight);

	Camera &setNearPlane(float nearPlane) {
		this->nearPlane = nearPlane;
		return *this;
	}

	float getNearPlane() const {
		return nearPlane;
	}

	Camera &setFarPlane(float farPlane) {
		this->farPlane = farPlane;
		return *this;
	}

	float getFarPlane() const {
		return farPlane;
	}

	Camera &setZoom(float zoom) {
		this->zoom = zoom;
		return *this;
	}

	float getZoom() const {
		return zoom;
	}

	glm::vec3 unproject(const glm::vec3 &position) const;

private:
	static const glm::mat4 IDENTITY;

	/*
	 * Interesting note: some header on windows defines "near" and "far"" to be something,
	 * which makes our definitions useless in some circumstances. This caused a compile-time error
	 * and a lot of head scratching. The end result is these variables had their names changed.
	 */
	float nearPlane = 0.0f, farPlane = 100.0f;
	float viewportWidth, viewportHeight;

	float zoom = 1.0f;
};

}

#endif /* INCLUDE_APG_CAMERA_HPP_ */
