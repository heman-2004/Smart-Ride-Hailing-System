#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <vector>
#include "core/Graph.h"
#include "models/Order.h"
#include "models/Driver.h"
#include "core/Dispatcher.h"
#include "core/Scheduler.h"
#include "patterns/Factory.h"
#include "utils/Logger.h"

// Simple observer to print surge updates
struct ConsoleSurgeObserver : public SurgeObserver {
    void onSurgeUpdate(double multiplier) override {
        Logger::info(std::string("Surge multiplier now: x") + std::to_string(multiplier));
    }
};

int main(){
    Graph g(6);
    g.addEdge(0,1,5);
    g.addEdge(1,2,3);
    g.addEdge(2,3,4);
    g.addEdge(1,4,2);
    g.addEdge(4,5,6);

    // create drivers
    auto d1 = DriverFactory::create(401, 0);
    auto d2 = DriverFactory::create(402, 3);
    auto d3 = DriverFactory::create(403, 5);

    Dispatcher::instance().registerDriver(d1);
    Dispatcher::instance().registerDriver(d2);
    Dispatcher::instance().registerDriver(d3);

    // add observer for surge updates
    auto obs = std::make_shared<ConsoleSurgeObserver>();
    Dispatcher::instance().addObserver(obs);

    // set strategy
    auto strat = std::make_shared<NearestStrategy>();
    Dispatcher::instance().setStrategy(strat);

    // submit orders
    Dispatcher::instance().submitOrder(Order(5001, 4, 5, 12.0));
    Dispatcher::instance().submitOrder(Order(5002, 2, 3, 8.0));
    Dispatcher::instance().submitOrder(Order(5003, 1, 5, 10.0));

    // dispatch
    Dispatcher::instance().runDispatch(g);

    std::this_thread::sleep_for(std::chrono::seconds(4));

    // simulate more demand
    Dispatcher::instance().submitOrder(Order(6001, 0, 2, 9.0));
    Dispatcher::instance().submitOrder(Order(6002, 2, 5, 11.0));
    Dispatcher::instance().submitOrder(Order(6003, 3, 0, 7.0));
    Dispatcher::instance().submitOrder(Order(6004, 1, 4, 6.0));

    // switch to LoadBalanced strategy
    Dispatcher::instance().setStrategy(std::make_shared<LoadBalancedStrategy>());
    Dispatcher::instance().runDispatch(g);

    std::this_thread::sleep_for(std::chrono::seconds(2));

    // simulate passenger cancellation
    Dispatcher::instance().cancelOrder(6003);

    std::this_thread::sleep_for(std::chrono::seconds(2));

    // simulate driver cancellation for one driver (they will cancel current job randomly)
    d2->simulateCancelCurrent();

    std::this_thread::sleep_for(std::chrono::seconds(6));

    // switch to RatingPriority strategy
    Dispatcher::instance().setStrategy(std::make_shared<RatingPriorityStrategy>());

    // submit final batch and dispatch
    Dispatcher::instance().submitOrder(Order(7001, 2, 0, 10.0));
    Dispatcher::instance().submitOrder(Order(7002, 5, 1, 9.0));
    Dispatcher::instance().runDispatch(g);

    std::this_thread::sleep_for(std::chrono::seconds(6));

    // cleanup
    Dispatcher::instance().unregisterDriver(401);
    Dispatcher::instance().unregisterDriver(402);
    Dispatcher::instance().unregisterDriver(403);

    std::cout<<"Simulation end."<<std::endl;
    return 0;
}
