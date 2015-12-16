#include <iostream>
#include <sstream>

#include <APG/APGS11N.hpp>

class hello {
public:
	hello(int i_ = 5) : i { i_ } {}
	~hello() = default;
	
	int i;
};

namespace APG {

template<> class JSONSerializer<hello> {
public:
	hello fromJSON(const char *json) {
		return hello();	
	}

	std::string toJSON(const hello &h) {
		std::stringstream ss;
		ss << "{ \"hello\": " << h.i << " }";
		return ss.str();
	}
};

}

int main() {
	APG::JSONSerializer<hello> hs;
	APG::JSONSerializer<int> is;
	APG::JSONSerializer<std::string> ss;

	const hello h, a(10);
	const std::string hi("hi");
	const int ka = 5;

	std::cout << hs.toJSON(h) << std::endl;
	std::cout << hs.toJSON(a) << std::endl;
	std::cout << ss.toJSON(hi) << std::endl;
	std::cout << is.toJSON(ka) << std::endl;

	return 0;
}
