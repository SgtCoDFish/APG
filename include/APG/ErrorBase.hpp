/*
 * APGErrorBase.hpp
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

#ifndef APGERRORBASE_HPP_
#define APGERRORBASE_HPP_

#include <string>

namespace APG {

class ErrorBase {
private:
	bool hasError_;
	std::string message_;

public:
	ErrorBase() : hasError_{false}, message_{} {}

	void setErrorState(const char *message);
	void setErrorState(const std::string &message);

	void unsetErrorState();

	bool hasError() const {
		return hasError_;
	}

	const std::string &getErrorMessage() {
		return message_;
	}
};

}

#endif /* APGERRORBASE_HPP_ */
