#pragma once

#include <string>
#include <sstream>

namespace autograder {
	template<class T>
	std::string to_string(T t) {
		std::stringstream ss;
		ss << t;
		return ss.str();
	}
}