#include "Driver.h"
#include <chrono>
#include <thread>
#include <random>
#include "../core/Dispatcher.h" // circular dependency ok here due to include order in build
#include "../utils/Logger.h"

Driver::Driver(int id_, int loc): id(id_), location(loc), active(true) {}

Driver::~Driver(){ stop(); }

void Driver::start(){
    running.store(true);
    worker = std::thread(&Driver::loop, this);
}

void Driver::stop(){
    running.store(false);
    cv.notify_all();
    if(worker.joinable()) worker.join();
}

void Driver::assignOrder(const Order &o){
    {
        std::lock_guard<std::mutex> lg(mtx);
        tasks.push(o);
    }
    cv.notify_one();
}

int Driver::pending(){
    std::lock_guard<std::mutex> lg(mtx);
    return (int)tasks.size();
}

void Driver::addRating(int r){
    ratingSum += r;
    ratingCount += 1;
}

double Driver::getRating() const{
    int cnt = ratingCount.load();
    if(cnt==0) return 5.0; // default neutral high rating
    return double(ratingSum.load()) / cnt;
}

void Driver::simulateCancelCurrent(){
    // For simplicity, if driver is busy, we trigger cancellation notification
    if(busy.load()){
        std::lock_guard<std::mutex> lg(mtx);
        if(!tasks.empty()){
            // attempt to cancel front task
            Order o = tasks.front();
            tasks.pop();
            Logger::info("Driver " + std::to_string(id) + " simulated cancellation of Order " + std::to_string(o.id));
            notifyCancellation(o.id);
            return;
        }
    }
    // else nothing to cancel
    Logger::info("Driver " + std::to_string(id) + " had no active task to cancel.");
}

void Driver::notifyCancellation(int orderId){
    // use dispatcher singleton
    Dispatcher::instance().notifyOrderCancelled(orderId, id);
}

void Driver::notifyCompletion(int orderId, int rating){
    Dispatcher::instance().notifyOrderCompleted(orderId, id, rating);
}

void Driver::loop(){
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> cancelChance(0, 99);

    while(running.load()){
        Order current(0,0,0,0.0);
        {
            std::unique_lock<std::mutex> lk(mtx);
            if(tasks.empty()){
                cv.wait_for(lk, std::chrono::milliseconds(500));
                if(!running.load()) break;
                continue;
            }
            current = tasks.front();
            tasks.pop();
        }
        busy.store(true);
        // simulate travel times
        int from = location.load();
        long long d1 = std::llabs(from - current.pickup) * 200;
        long long d2 = std::llabs(current.pickup - current.dropoff) * 200;

        Logger::info("Driver " + std::to_string(id) + " assigned Order " + std::to_string(current.id));
        simulateTravel(d1);
        location.store(current.pickup);
        Logger::info("Driver " + std::to_string(id) + " picked Order " + std::to_string(current.id));

        // random chance to cancel mid-way (simulate driver cancellation)
        if(cancelChance(rng) < 10){ // 10% chance
            Logger::info("Driver " + std::to_string(id) + " cancelled Order " + std::to_string(current.id));
            notifyCancellation(current.id);
            busy.store(false);
            continue;
        }

        simulateTravel(d2);
        location.store(current.dropoff);
        Logger::info("Driver " + std::to_string(id) + " delivered Order " + std::to_string(current.id));

        // simulate passenger rating 3..5
        int rating = 3 + (rng()%3);
        addRating(rating);
        notifyCompletion(current.id, rating);
        busy.store(false);
    }
    Logger::info("Driver " + std::to_string(id) + " stopping.");
}

void Driver::simulateTravel(long long millis){
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(std::chrono::milliseconds(std::min<long long>(3000, millis)));
}
