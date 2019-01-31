#pragma once

#include <string>

namespace utils
{
	bool Base64Encode(const std::string& input, std::string& output);
	bool Base64Decode(const std::string& input, std::string& output);
}