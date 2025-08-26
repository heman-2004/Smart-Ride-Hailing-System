#include "Scheduler.h"
#include "../utils/Logger.h"
#include <queue>
#include <limits>
#include <algorithm>
#include <cmath>

// helper to compute driver->node distances
static std::vector<std::vector<long long>> precomputeDistances(const std::vector<std::shared_ptr<Driver>>& drivers, const Graph &graph){
    int k = (int)drivers.size();
    std::vector<std::vector<long long>> dist(k);
    for(int i=0;i<k;i++) dist[i] = graph.dijkstra(drivers[i]->location.load());
    return dist;
}

std::vector<std::pair<int,int>> NearestStrategy::assign(const std::vector<Order>& orders, const std::vector<std::shared_ptr<Driver>>& drivers, const Graph &graph){
    std::vector<std::pair<int,int>> assignments;
    if(orders.empty() || drivers.empty()) return assignments;
    auto dist = precomputeDistances(drivers, graph);
    for(const auto &o: orders){
        long long best = std::numeric_limits<long long>::max();
        int bestIdx = -1;
        for(int i=0;i<(int)drivers.size();i++){
            long long d = dist[i][o.pickup];
            if(d < best){
                best = d; bestIdx = i;
            }
        }
        if(bestIdx!=-1) assignments.push_back({o.id, bestIdx});
    }
    return assignments;
}

std::vector<std::pair<int,int>> LoadBalancedStrategy::assign(const std::vector<Order>& orders, const std::vector<std::shared_ptr<Driver>>& drivers, const Graph &graph){
    std::vector<std::pair<int,int>> assignments;
    if(orders.empty() || drivers.empty()) return assignments;
    auto dist = precomputeDistances(drivers, graph);
    for(const auto &o: orders){
        double bestScore = 1e18;
        int bestIdx = -1;
        for(int i=0;i<(int)drivers.size();i++){
            int load = drivers[i]->pending();
            double score = dist[i][o.pickup] * (1.0 + load*0.5); // penalize by load
            if(score < bestScore){ bestScore = score; bestIdx = i; }
        }
        if(bestIdx!=-1) assignments.push_back({o.id, bestIdx});
    }
    return assignments;
}

std::vector<std::pair<int,int>> RatingPriorityStrategy::assign(const std::vector<Order>& orders, const std::vector<std::shared_ptr<Driver>>& drivers, const Graph &graph){
    std::vector<std::pair<int,int>> assignments;
    if(orders.empty() || drivers.empty()) return assignments;
    auto dist = precomputeDistances(drivers, graph);
    for(const auto &o: orders){
        double bestScore = 1e18;
        int bestIdx = -1;
        for(int i=0;i<(int)drivers.size();i++){
            double rating = drivers[i]->getRating();
            double score = dist[i][o.pickup] / std::max(0.1, rating); // higher rating lowers score
            if(score < bestScore){ bestScore = score; bestIdx = i; }
        }
        if(bestIdx!=-1) assignments.push_back({o.id, bestIdx});
    }
    return assignments;
}
