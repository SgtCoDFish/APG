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
#include "Buffer.hpp"
#include "Texture.hpp"

const char *APG::APGGLRenderTest::vertexShaderFilename = "pass_vertex.glslv";
const char * APG::APGGLRenderTest::fragmentShaderFilename = "red_frag.glslf";

GLfloat vertices[] = { -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Top-left
		0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top-right
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Bottom-right
		-0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f  // Bottom-left
		};

GLuint elements[] = { 0, 1, 2, 2, 3, 0 };

bool APG::APGGLRenderTest::init() {
	if (hasError()) {
		std::cerr << "Failed to initialise APGGLRenderTest.\n";
		return false;
	}

	vao = std::make_unique<VAO>();
	vao->bind();

	vertexBuffer = std::make_unique<Buffer<>>(APG::BufferType::ARRAY, APG::DrawType::STATIC_DRAW,
			vertices, 28);

	if (vertexBuffer->hasError()) {
		std::cerr << "Error initialising vertex buffer: " << vertexBuffer->getErrorMessage()
				<< std::endl;
		return false;
	}

	vertexBuffer->bind();

	elementBuffer = std::make_unique<Buffer<uint32_t, GL_UNSIGNED_INT>>(
			APG::BufferType::ELEMENT_ARRAY, APG::DrawType::STATIC_DRAW, elements, 6);

	shaderProgram = std::make_unique<ShaderProgram>(vertexShaderFilename, fragmentShaderFilename);

	if (shaderProgram->hasError()) {
		std::cout << "Shader Info Log\n---------------\n" << shaderProgram->getShaderInfoLog();
		std::cout << "\nLink Info Log\n-------------\n" << shaderProgram->getLinkInfoLog()
				<< std::endl;
		std::cerr << "Couldn't create shader:\n" << shaderProgram->getErrorMessage() << std::endl;
		return false;
	}

	shaderProgram->setFloatAttribute("position", 2, 7, 0, false);
	shaderProgram->setFloatAttribute("color", 3, 7, 2, false);
	shaderProgram->setFloatAttribute("texcoord", 2, 7, 5, false);

	if (shaderProgram->hasError()) {
		std::cerr << "Couldn't setup attributes:\n" << shaderProgram->getErrorMessage()
				<< std::endl;
		return false;
	}

	texture1 = std::make_unique<Texture>("assets/world1tileset.png");

	if (texture1->hasError()) {
		std::cerr << "Couldn't load texture:\n" << texture1->getErrorMessage() << std::endl;

		return false;
	}

	texture1->setFilter(TextureFilterType::LINEAR, TextureFilterType::LINEAR);
	texture1->setWrapType(TextureWrapType::CLAMP_TO_EDGE, TextureWrapType::CLAMP_TO_EDGE);

	texture2 = std::make_unique<Texture>("assets/npctileset.png");

	if (texture2->hasError()) {
		std::cerr << "Couldn't load texture:\n" << texture2->getErrorMessage() << std::endl;

		return false;
	}

	texture2->setFilter(TextureFilterType::LINEAR, TextureFilterType::LINEAR);
	texture2->setWrapType(TextureWrapType::CLAMP_TO_EDGE, TextureWrapType::CLAMP_TO_EDGE);

	texture1->attachToShader("tex1", shaderProgram.get());
	texture2->attachToShader("tex2", shaderProgram.get());

	texture1->bind();

	renderer = std::make_unique<GLTmxRenderer>();

	auto glError = glGetError();
	if (glError != GL_NO_ERROR) {
		while (glError != GL_NO_ERROR) {
			std::cerr << "Error in gl: " << gluErrorString(glError) << std::endl;
			glError = glGetError();
		}

		return false;
	}

	return true;
}

void APG::APGGLRenderTest::render(float deltaTime) {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glDrawElements(GL_TRIANGLES, 6, elementBuffer->getGLType(), 0);

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

	return EXIT_SUCCESS;
}
#endif
