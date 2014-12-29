/*
 * APGGLRenderTest.cpp
 * Copyright (C) 2014 Ashley Davis (SgtCoDFish)
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

#include <cstdint>
#include <cstdlib>

#include <iostream>
#include <memory>
#include <chrono>
#include <vector>
#include <numeric>

#include <GL/glew.h>
#include <GL/gl.h>

#include "APGGLRenderTest.hpp"
#include "Game.hpp"
#include "SDLGame.hpp"
#include "APGCommon.hpp"
#include "SXXDL.hpp"
#include "VertexBuffer.hpp"

const char *APG::APGGLRenderTest::vertexShaderFilename = "pass_vertex.glslv";
const char * APG::APGGLRenderTest::fragmentShaderFilename = "red_frag.glslf";

GLfloat vertices[] = { 0.0f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f };
GLuint vao;

bool APG::APGGLRenderTest::init() {
	if (hasError()) {
		std::cerr << "Failed to initialise APGGLRenderTest.\n";
		return false;
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	vertexBuffer = std::make_unique<VertexBuffer>(APG::BufferType::ARRAY,
			APG::DrawType::STATIC_DRAW, vertices, 6);

	if (vertexBuffer->hasError()) {
		std::cerr << "Error initialising vertex buffer: " << vertexBuffer->getErrorMessage()
				<< std::endl;
		return false;
	}

	vertexBuffer->bind();

	shaderProgram = std::make_unique<ShaderProgram>(vertexShaderFilename, fragmentShaderFilename);

	if (shaderProgram->hasError()) {
		std::cout << "Shader Info Log\n---------------\n" << shaderProgram->getShaderInfoLog();
		std::cout << "\nLink Info Log\n-------------\n" << shaderProgram->getLinkInfoLog()
				<< std::endl;
		std::cerr << "Couldn't create shader:\n" << shaderProgram->getErrorMessage() << std::endl;
		return false;
	}

	shaderProgram->setFloatAttribute("position", 2, 0, nullptr, false);
	shaderProgram->setUniformf("triangleColor", {1.0f, 1.0f, 0.0f});

	if (shaderProgram->hasError()) {
		std::cerr << "Couldn't setup attributes:\n" << shaderProgram->getErrorMessage()
				<< std::endl;
		return false;
	}

	renderer = std::make_unique<GLTmxRenderer>();

	if (glGetError() != GL_NO_ERROR) {
		std::cerr << "Error in gl.\n";
		return false;
	}

	return true;
}

void APG::APGGLRenderTest::render(float deltaTime) {
	glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	SDL_GL_SwapWindow(window.get());
}

#ifndef APG_TEST_SDL
int main(int argc, char *argv[]) {
	APG::SDLGame::sdlWindowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;
	auto game = std::make_unique<APG::APGGLRenderTest>();

	if (!game->init()) {
		return EXIT_FAILURE;
	}

	bool done = false;

	auto startTime = std::chrono::high_resolution_clock::now();
	std::vector<float> timesTaken;
	while (!done) {
		auto timeNow = std::chrono::high_resolution_clock::now();
		float deltaTime =
				std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - startTime).count()
						/ 1000.0f;

		startTime = timeNow;
		timesTaken.push_back(deltaTime);

		done = game->update(deltaTime);

		if (timesTaken.size() >= 1000) {
			const float sum = std::accumulate(timesTaken.begin(), timesTaken.end(), 0.0f);

			const float fps = 1 / (sum / timesTaken.size());

			std::cout << "FPS: " << fps << std::endl;

			timesTaken.clear();
		}
	}

	glDeleteVertexArrays(1, &vao);

	return EXIT_SUCCESS;
}
#endif
