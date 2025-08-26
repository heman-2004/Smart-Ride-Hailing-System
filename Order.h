#pragma once
#include <chrono>
#include <atomic>

enum class OrderStatus { QUEUED, ASSIGNED, IN_PROGRESS, COMPLETED, CANCELLED };

struct Order {
    int id;
    int pickup;
    int dropoff;
    double fare; // base fare (will be multiplied by surge)
    int passengerId;
    OrderStatus status;
    std::chrono::system_clock::time_point created;

    Order(int id_, int p_, int d_, double fare_, int pid=0)
        : id(id_), pickup(p_), dropoff(d_), fare(fare_), passengerId(pid), status(OrderStatus::QUEUED),
          created(std::chrono::system_clock::now()) {}
};
