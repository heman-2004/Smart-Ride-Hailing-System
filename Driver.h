#pragma once
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <iostream>
#include <optional>
#include <vector>
#include "Order.h"

class Driver {
public:
    int id;
    std::atomic<int> location;
    std::atomic<bool> active;

    Driver(int id_, int loc);
    ~Driver();

    void start();
    void stop();

    // assign an order to driver (thread-safe)
    void assignOrder(const Order &o);

    // driver current pending count
    int pending();

    // rating update (called by dispatcher after delivery)
    void addRating(int r);
    double getRating() const;

    // try to cancel an order currently being processed (for simulation)
    void simulateCancelCurrent();

private:
    std::thread worker;
    mutable std::mutex mtx;
    std::condition_variable cv;
    std::queue<Order> tasks;
    std::atomic<bool> running{false};
    std::atomic<bool> busy{false};

    // rating stats
    std::atomic<int> ratingSum{0};
    std::atomic<int> ratingCount{0};

    void loop();
    void simulateTravel(long long millis);
    // notify dispatcher on cancellation or completion by calling singleton
    void notifyCancellation(int orderId);
    void notifyCompletion(int orderId, int rating);
};
