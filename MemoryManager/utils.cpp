#include "utils.h"
#include <iostream>
#include <iomanip>
#include <filesystem>

std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d_%H-%M-%S")
       << "-" << std::setw(3) << std::setfill('0') << milliseconds.count();

    return ss.str();
}

void writeToFile(const std::string& folder, const std::string& filename, const std::string& content) {
    try {
        std::filesystem::create_directories(folder);
        std::ofstream file(folder + "/" + filename);
        if (file.is_open()) {
            file << content;
            file.close();
        } else {
            std::cerr << "Error: No se pudo abrir el archivo " << filename << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error al escribir en archivo: " << e.what() << std::endl;
    }
}
