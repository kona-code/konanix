#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <mutex>

class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    void log(const std::string& message) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_logFile << message << std::endl;
        //std::cout << message << std::endl; // optionally print to console
    }

    void logError(const std::string& message) {
        log("[ERROR] " + message);
    }

    void logInfo(const std::string& message) {
        log("[INFO] " + message);
    }

private:
    Logger() : m_logFile("konanix.log", std::ios::out | std::ios::app) {
        if (!m_logFile.is_open()) {
            MessageBox(NULL, L"Failed to open log file.", L"Konanix - Error", MB_OK | MB_ICONERROR);
            throw std::runtime_error("Failed to open log file.");
        }
    }

    ~Logger() {
        if (m_logFile.is_open()) {
            m_logFile.close();
        }
    }

    std::ofstream m_logFile;
    std::mutex m_mutex;
};
