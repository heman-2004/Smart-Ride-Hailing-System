#pragma once
#include <vector>
#include <map>
#include <mutex>
#include <memory>
#include <functional>
#include "models/Order.h"
#include "models/Driver.h"
#include "core/Graph.h"
#include "core/Scheduler.h"
#include "../utils/Logger.h"

// Observer interface for surge/price updates
struct SurgeObserver {
    virtual void onSurgeUpdate(double multiplier) = 0;
};

class Dispatcher {
public:
    static Dispatcher& instance(){
        static Dispatcher d;
        return d;
    }

    void registerDriver(std::shared_ptr<Driver> drv);
    void unregisterDriver(int id);

    void submitOrder(const Order &o);
    void cancelOrder(int orderId); // external cancel (passenger cancels)
    void notifyOrderCancelled(int orderId, int driverId); // driver notifies cancellation
    void notifyOrderCompleted(int orderId, int driverId, int rating);

    void setStrategy(std::shared_ptr<AssignmentStrategy> strat);
    void addObserver(std::shared_ptr<SurgeObserver> obs);

    void runDispatch(const Graph &g);

private:
    Dispatcher() {}
    std::mutex mtx;
    std::map<int, std::shared_ptr<Driver>> drivers;
    std::vector<Order> queueOrders;
    std::shared_ptr<AssignmentStrategy> strategy;
    std::vector<std::shared_ptr<SurgeObserver>> observers;

    // compute surge multiplier and notify observers
    void computeSurgeAndNotify();
    double lastSurge{1.0};
};
