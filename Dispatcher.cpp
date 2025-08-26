#include "core/Dispatcher.h"
#include "../utils/Logger.h"

void Dispatcher::registerDriver(std::shared_ptr<Driver> drv){
    std::lock_guard<std::mutex> lg(mtx);
    drivers[drv->id] = drv;
    drv->start();
    Logger::info("Registered Driver " + std::to_string(drv->id));
}

void Dispatcher::unregisterDriver(int id){
    std::lock_guard<std::mutex> lg(mtx);
    auto it = drivers.find(id);
    if(it!=drivers.end()){
        it->second->stop();
        drivers.erase(it);
        Logger::info("Unregistered Driver " + std::to_string(id));
    }
}

void Dispatcher::submitOrder(const Order &o){
    std::lock_guard<std::mutex> lg(mtx);
    queueOrders.push_back(o);
    Logger::info("Order queued " + std::to_string(o.id));
    computeSurgeAndNotify();
}

void Dispatcher::cancelOrder(int orderId){
    std::lock_guard<std::mutex> lg(mtx);
    for(auto it = queueOrders.begin(); it != queueOrders.end(); ++it){
        if(it->id == orderId){
            it->status = OrderStatus::CANCELLED;
            Logger::info("Passenger cancelled Order " + std::to_string(orderId));
            queueOrders.erase(it);
            computeSurgeAndNotify();
            return;
        }
    }
    Logger::info("Order " + std::to_string(orderId) + " not found in queue for cancellation.");
}

void Dispatcher::notifyOrderCancelled(int orderId, int driverId){
    std::lock_guard<std::mutex> lg(mtx);
    Logger::info("Dispatcher received cancellation for Order " + std::to_string(orderId) + " from Driver " + std::to_string(driverId));
    // requeue the order (set status to queued)
    // In a full system we'd retrieve order details from DB; here we recreate minimal Order with same id and default nodes (demo)
    Order o(orderId, 0, 0, 10.0);
    o.status = OrderStatus::QUEUED;
    queueOrders.push_back(o);
    computeSurgeAndNotify();
}

void Dispatcher::notifyOrderCompleted(int orderId, int driverId, int rating){
    std::lock_guard<std::mutex> lg(mtx);
    Logger::info("Order " + std::to_string(orderId) + " completed by Driver " + std::to_string(driverId) + ". Rating=" + std::to_string(rating));
    // update driver rating
    auto it = drivers.find(driverId);
    if(it!=drivers.end()){
        it->second->addRating(rating);
    }
    computeSurgeAndNotify();
}

void Dispatcher::setStrategy(std::shared_ptr<AssignmentStrategy> strat){
    std::lock_guard<std::mutex> lg(mtx);
    strategy = strat;
    Logger::info("Strategy updated.");
}

void Dispatcher::addObserver(std::shared_ptr<SurgeObserver> obs){
    std::lock_guard<std::mutex> lg(mtx);
    observers.push_back(obs);
}

// compute simple surge: multiplier = 1 + max(0, (queued - available)/available * 0.1)
void Dispatcher::computeSurgeAndNotify(){
    int queued = (int)queueOrders.size();
    int available = (int)drivers.size();
    double mult = 1.0;
    if(available>0 && queued>available){
        mult = 1.0 + (double)(queued - available)/available * 0.1;
    }
    if(std::abs(mult - lastSurge) > 1e-6){
        lastSurge = mult;
        for(auto &o: observers){
            if(o) o->onSurgeUpdate(mult);
        }
        Logger::info("Surge updated: x" + std::to_string(mult));
    }
}

void Dispatcher::runDispatch(const Graph &g){
    std::lock_guard<std::mutex> lg(mtx);
    if(!strategy) return;
    if(queueOrders.empty() || drivers.empty()) return;
    // snapshot drivers into vector (preserve order for indexing)
    std::vector<std::shared_ptr<Driver>> drvVec;
    std::vector<int> drvIds;
    for(auto &p: drivers){
        drvVec.push_back(p.second);
        drvIds.push_back(p.first);
    }
    auto assigns = strategy->assign(queueOrders, drvVec, g);
    // perform assignments (map driver index to id)
    for(auto &pr: assigns){
        int orderId = pr.first;
        int driverIdx = pr.second;
        if(driverIdx<0 || driverIdx >= (int)drvIds.size()) continue;
        int driverId = drvIds[driverIdx];
        auto it = drivers.find(driverId);
        if(it==drivers.end()) continue;
        // find order by id
        for(auto it2 = queueOrders.begin(); it2 != queueOrders.end(); ++it2){
            if(it2->id == orderId){
                it->second->assignOrder(*it2);
                it2->status = OrderStatus::ASSIGNED;
                Logger::info("Assigned Order " + std::to_string(orderId) + " -> Driver " + std::to_string(driverId));
                queueOrders.erase(it2);
                break;
            }
        }
    }
    computeSurgeAndNotify();
}
