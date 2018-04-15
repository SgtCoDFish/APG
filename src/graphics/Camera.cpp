#include <cstdint>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "APG/graphics/Camera.hpp"

const glm::mat4 APG::Camera::IDENTITY {};

APG::Camera::Camera(float viewportWidth, float viewportHeight) {
	setToOrtho(false, viewportWidth, viewportHeight);
}

void APG::Camera::update() {
	projectionMatrix = glm::ortho(0.0f, viewportWidth, viewportHeight, 0.0f, nearPlane, farPlane);
	transformMatrix = glm::lookAt(position, position + direction, up);
	combinedMatrix = projectionMatrix * transformMatrix;
}

void APG::Camera::setToOrtho(bool yDown, float _viewportWidth, float _viewportHeight) {
	up.x = direction.x = direction.y = up.z = 0.0f;
	up.y = (yDown ? -1 : 1);
	direction.z = (yDown ? 1 : -1);

	position = {zoom * viewportWidth / 2.0f, zoom * viewportHeight / 2.0f, 0.0f};
	this->viewportWidth = _viewportWidth;
	this->viewportHeight = _viewportHeight;

	update();
}

glm::vec3 APG::Camera::unproject(const glm::vec3 &position) const {
	// TODO: This might not be quite right.
	return glm::unProject(position, IDENTITY, projectionMatrix, glm::vec4(0, 0, viewportWidth, viewportHeight));
}
