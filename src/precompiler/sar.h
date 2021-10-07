
#ifndef SAR_H
#define SAR_H

#include <string>
#include <regex>

// Simple SAR, no group targeting
std::string replace(std::string input, std::string pattern, std::string replace = "")
{
  return regex_replace(input, std::regex(pattern), replace);
}

#endif // SAR_H