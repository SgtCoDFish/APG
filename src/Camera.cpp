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

#include <cstdint>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "APG/Camera.hpp"

const glm::mat4 APG::Camera::IDENTITY;

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
