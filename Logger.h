#pragma once
#include <mutex>
#include <iostream>
#include <string>

struct Logger {
    static std::mutex m;
    static void info(const std::string &s){
        std::lock_guard<std::mutex> lg(m);
        std::cout << "[INFO] " << s << std::endl;
    }
};
std::mutex Logger::m;
