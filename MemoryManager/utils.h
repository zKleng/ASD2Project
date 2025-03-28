#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <chrono>
#include <ctime>
#include <fstream>
#include <sstream>

std::string getCurrentTimestamp();
void writeToFile(const std::string& folder, const std::string& filename, const std::string& content);

#endif // UTILS_H
