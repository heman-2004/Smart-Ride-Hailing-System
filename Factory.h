#pragma once
#include <memory>
#include "../models/Driver.h"

struct DriverFactory {
    static std::shared_ptr<Driver> create(int id, int startLoc){
        return std::make_shared<Driver>(id, startLoc);
    }
};
