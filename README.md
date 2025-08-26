🚖 Smart Ride-Hailing System (Mini Uber Clone)
A multithreaded C++ project simulating a ride-hailing service, designed using Object-Oriented Programming (OOP) concepts, Design Patterns, and Data Structures for realistic problem-solving.

✨ Features
Passenger & Driver Entities – Classes encapsulating rider/driver behavior.
Thread-Safe Dispatch – Concurrent ride matching handled with std::mutex.
Driver-Rider Matching – Optimized allocation using Dijkstra’s algorithm for shortest path and priority queues for scheduling.
Design Patterns
Singleton – Centralized dispatcher.
Factory – Dynamic vehicle creation (Car, Bike, Auto).
Strategy – Flexible driver assignment strategies.
Observer – Logging and monitoring system.
Scalability – Modular and extensible architecture to add new features (e.g., pricing models, maps, vehicle types).

🛠️ Tech Stack
Language: C++
Core Concepts: OOP, Threads, Mutex, Data Structures, Algorithms
Patterns Used: Singleton, Factory, Strategy, Observer
Data Structures: Graphs, Priority Queue (Heap), STL Containers

📂 Project Structure
smart_ride_hailing/
|   ├── Passenger.h
│   ├── Driver.h
│   ├── VehicleFactory.h
│   ├── Dispatcher.h
│   ├── Strategy.h
│   ├── Observer.h
│   ├── Graph.h
│   ├── Passenger.cpp
│   ├── Driver.cpp
│   ├── VehicleFactory.cpp
│   ├── Dispatcher.cpp
│   ├── Strategy.cpp
│   ├── Observer.cpp
│   ├── Graph.cpp
│   ├── main.cpp


🚀 How It Works
Passengers request rides → Dispatcher receives requests.
Dispatcher searches drivers using Dijkstra’s shortest path + priority queue scheduling.
Thread-safe assignment ensures safe concurrent dispatch.
Observers log events (ride requests, driver assignments, completions).
Simulation runs concurrently with multiple passenger/driver threads.

🔑 Key Learning Outcomes
Practical application of OOP principles (encapsulation, inheritance, polymorphism).
Integration of multithreading & synchronization in C++.
Use of Design Patterns to build scalable architectures.
Implementation of real-world DSA (Graphs & Heaps) in a simulation.

📈 Future Improvements
Add real-time pricing models (surge pricing).
Implement geo-location clustering for faster matching.
Integrate database or file storage for persistent data.
Expand to support different ride types (pooling, rentals).
[LOG]: Ride started: P1 with D3
[LOG]: Ride completed: P1 reached destination
