#pragma once
#include <vector>
#include <utility>
#include "Order.h"
#include "Graph.h"

// Strategy interface
struct AssignmentStrategy {
    virtual ~AssignmentStrategy() = default;
    virtual std::vector<std::pair<int,int>> assign(const std::vector<Order>& orders, const std::vector<std::shared_ptr<class Driver>>& drivers, const Graph &graph) = 0;
};

// Nearest: choose driver with smallest distance to pickup (ignores load and rating)
struct NearestStrategy : public AssignmentStrategy {
    std::vector<std::pair<int,int>> assign(const std::vector<Order>& orders, const std::vector<std::shared_ptr<Driver>>& drivers, const Graph &graph) override;
};

// LoadBalanced: prefer drivers with fewer pending tasks (load-aware)
struct LoadBalancedStrategy : public AssignmentStrategy {
    std::vector<std::pair<int,int>> assign(const std::vector<Order>& orders, const std::vector<std::shared_ptr<Driver>>& drivers, const Graph &graph) override;
};

// RatingPriority: favor higher-rated drivers (rating-aware)
struct RatingPriorityStrategy : public AssignmentStrategy {
    std::vector<std::pair<int,int>> assign(const std::vector<Order>& orders, const std::vector<std::shared_ptr<Driver>>& drivers, const Graph &graph) override;
};
