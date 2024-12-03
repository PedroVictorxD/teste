#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>

class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    void log(const std::string& message) {
        auto now = std::time(nullptr);
        auto tm = *std::localtime(&now);
        
        std::ofstream logFile("game_moves.log", std::ios::app);
        logFile << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " - " << message << std::endl;
        logFile.close();

        // Also print to console
        std::cout << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " - " << message << std::endl;
    }

private:
    Logger() {} // Private constructor for singleton
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
};

#define LOG(message) Logger::getInstance().log(message)
