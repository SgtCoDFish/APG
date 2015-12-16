/*
 * Copyright (c) 2014, 2015 See AUTHORS file.
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

#ifndef INCLUDE_APG_S11N_JSON_HPP_
#define INCLUDE_APG_S11N_JSON_HPP_

#define APG_JSON_PRETTY

#include <string>
#include <sstream>
#include <memory>
#include <typeindex>
#include <typeinfo>
#include <type_traits>

#include <rapidjson/document.h>
#ifdef APG_JSON_PRETTY
#include <rapidjson/prettywriter.h>
#else
#include <rapidjson/writer.h>
#endif

#include "APG/core/APGeasylogging.hpp"

namespace APG {

/**
 * Should be specialised for each seraializable type T to override the
 * fromJSON and toJSON methods.
 *
 * APG provides a python autogeneration framework for doing exactly this for each type you want.
 */
template<typename T> class JSONSerializer {
public:
	explicit JSONSerializer() {
		writer = std::make_unique<writerType>(buffer);
	}

	~JSONSerializer() = default;

	T fromJSON(const char *json) {
		if (!std::is_default_constructible<T>::value) {
			el::Loggers::getLogger("APG")->fatal(
			        "Default fromJSON requires types to be default constructible. You probably need to specialize the JSONSerializer<T> class. Type: %v",
			        std::type_index(typeid(T)).name());
		}

		rapidjson::Document d;
		d.Parse(json);

		if (is_integral) {
			if (d.HasMember("value") && d["value"].IsInt()) {
				return d["value"].GetInt();
			}
		} else if (is_floating_point) {
			if (d.HasMember("value") && d["value"].IsDouble()) {
				return d["value"].GetDouble();
			}
		}

		el::Loggers::getLogger("APG")->warn(
		        "No sensible implementation available for JSONSerializer::fromJSON for type \"%v\", JSON input was ignored.",
		        std::type_index(typeid(T)).name());

		return T();
	}

	std::string toJSON(const T &t) {
		buffer.Clear();

		writer->StartObject();

		writer->String("value");
		std::stringstream ss;
		ss << t;
		writer->String(ss.str().c_str());

		writer->EndObject();

		return std::string(buffer.GetString());
	}

private:
#ifdef APG_JSON_PRETTY
	using writerType = rapidjson::PrettyWriter<rapidjson::StringBuffer>;
#else
	using writerType = rapidjson::Writer<rapidjson::StringBuffer>;
#endif

	rapidjson::StringBuffer buffer;

	std::unique_ptr<rapidjson::Writer<rapidjson::StringBuffer>> writer;

	constexpr static const bool is_integral = std::is_scalar<T>::value;
	constexpr static const bool is_floating_point = std::is_floating_point<T>::value;
};

}

#endif /* INCLUDE_APG_S11N_JSON_HPP_ */
