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

#include "spdlog/spdlog.h"

namespace APG {
class JSONCommon {
public:
	explicit JSONCommon() : logger{spdlog::get("APG")} {
		writer = std::make_unique<writerType>(buffer);
	}

	~JSONCommon() = default;

protected:
#ifdef APG_JSON_PRETTY
	using writerType = rapidjson::PrettyWriter<rapidjson::StringBuffer>;
#else
	using writerType = rapidjson::Writer<rapidjson::StringBuffer>;
#endif

	rapidjson::StringBuffer buffer;

	std::unique_ptr<rapidjson::Writer<rapidjson::StringBuffer>> writer;

	std::shared_ptr<spdlog::logger> logger;
};

/**
 * Should be specialised for each seraializable type T to override the
 * fromJSON and toJSON methods.
 *
 * APG provides a python auto-generation framework for doing exactly this for each type you want.
 */
template<typename T> class JSONSerializer : public JSONCommon {
public:
	explicit JSONSerializer() :
			        JSONCommon() {

	}

	~JSONSerializer() = default;

	T fromJSON(const char *json) {
		if (!std::is_default_constructible<T>::value) {
			logger->critical(
			        "Default fromJSON requires types to be default constructible. You probably need to specialize the JSONSerializer<T> class. Type: {}",
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

		logger->warn(
		        "No sensible implementation available for JSONSerializer::fromJSON for type \"{}\", JSON input was ignored.",
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

protected:
	constexpr static const bool is_integral = std::is_scalar<T>::value;
	constexpr static const bool is_floating_point = std::is_floating_point<T>::value;
};

}

#endif /* INCLUDE_APG_S11N_JSON_HPP_ */
