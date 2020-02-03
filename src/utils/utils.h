#pragma once

#include <ctime>
#include <iomanip>
#include <string>

namespace utils {
// get current data and time
inline const std::string currentDateTime() {
  time_t timePtr = time(0);
  tm *timeInfo = localtime(&timePtr);
  char buffer[50];
  strftime(buffer, 50, "%c", timeInfo);
  return std::string(buffer);
}
} // namespace utils
