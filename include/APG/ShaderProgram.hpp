/*
 * ShaderProgram.hpp
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

#ifndef SHADERPROGRAM_HPP_
#define SHADERPROGRAM_HPP_

#include <cstdint>

#include <string>

#include "ErrorBase.hpp"

namespace APG {

class ShaderProgram : public ErrorBase {
private:
	uint32_t vertexShader, fragmentShader;
	uint32_t shaderProgram;

	uint32_t *validateType(uint32_t shaderType);

	std::string shaderInfoLog, linkInfoLog;

	void loadShader(const std::string &vertexShaderFilename, uint32_t shaderType);
	void combineProgram();

public:
	ShaderProgram(const std::string &vertexShaderFilename,
			const std::string &fragmentShaderFileName);

	void use();

	std::string getShaderInfoLog() const {
		return shaderInfoLog;
	}

	std::string getLinkInfoLog() const {
		return linkInfoLog;
	}
};

}

#endif /* SHADERPROGRAM_HPP_ */
