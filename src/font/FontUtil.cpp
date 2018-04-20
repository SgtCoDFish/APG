#include "APG/font/FontUtil.hpp"

namespace APG {

namespace util {

std::vector<std::string> splitString(const std::string &text, const std::string &delimiter) {
	std::vector<std::string> stringVector;

	std::string::size_type begin = 0U;
	auto end = text.find(delimiter);

	while (end != std::string::npos) {
		stringVector.emplace_back(text.substr(begin, end - begin));
		begin = end + delimiter.length();
		end = text.find(delimiter, begin);
	}

	return stringVector;
}

}

}