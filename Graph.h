#pragma once
#include <vector>
#include <queue>
#include <limits>
#include <utility>

struct Graph {
    int n;
    std::vector<std::vector<std::pair<int,int>>> adj; // to, weight
    Graph(int n_=0): n(n_), adj(n_) {}
    void addEdge(int u,int v,int w){
        if(u<0||v<0||u>=n||v>=n) return;
        adj[u].push_back({v,w});
        adj[v].push_back({u,w});
    }
    std::vector<long long> dijkstra(int src) const {
        const long long INF = std::numeric_limits<long long>::max()/4;
        std::vector<long long> dist(n, INF);
        using P = std::pair<long long,int>;
        std::priority_queue<P, std::vector<P>, std::greater<P>> pq;
        dist[src]=0; pq.push({0,src});
        while(!pq.empty()){
            auto [d,u]=pq.top(); pq.pop();
            if(d!=dist[u]) continue;
            for(auto &e: adj[u]){
                int v=e.first, w=e.second;
                if(dist[v] > d + w){
                    dist[v] = d + w;
                    pq.push({dist[v], v});
                }
            }
        }
        return dist;
    }
};
