/*
 * Copyright (c) 2015 Ashley Davis (SgtCoDFish)
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

#ifndef INCLUDE_APG_CAMERA_HPP_
#define INCLUDE_APG_CAMERA_HPP_

#include <cstdint>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "APG/Game.hpp"

namespace APG {

/**
 * An orthographic camera, heavily inspired by LibGDX
 *
 * https://github.com/libgdx/libgdx/blob/master/gdx/src/com/badlogic/gdx/graphics/OrthographicCamera.java
 */
class Camera {
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

public:
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 up;

	glm::mat4 projectionMatrix;
	glm::mat4 transformMatrix;
	glm::mat4 combinedMatrix;

	explicit Camera(float viewportWidth = Game::screenWidth, float viewportHeight = Game::screenHeight);

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
};

}

#endif /* INCLUDE_APG_CAMERA_HPP_ */
