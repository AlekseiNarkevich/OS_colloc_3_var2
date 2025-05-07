#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>

// Абстрактный участник системы
class Airplane {
public:
    virtual void notify(const std::string& message) = 0;
    virtual void send(const std::string& message) = 0;
    virtual ~Airplane() = default;
};

// Интерфейс посредника
class ControlTower {
public:
    virtual void registerPlane(Airplane* plane) = 0;
    virtual void relayMessage(Airplane* sender, const std::string& message) = 0;
    virtual ~ControlTower() = default;
};

// Конкретная реализация посредника
class AirportControlTower : public ControlTower {
    std::vector<Airplane*> planes;
    
public:
    void registerPlane(Airplane* plane) override {
        planes.push_back(plane);
    }
    
    void relayMessage(Airplane* sender, const std::string& message) override {
        for (auto* plane : planes) {
            if (plane != sender) {
                plane->notify(message);
            }
        }
    }
};

// Конкретный участник
class CommercialFlight : public Airplane {
    ControlTower* tower;
    std::string flightNumber;
    
public:
    CommercialFlight(ControlTower* twr, std::string number)
        : tower(twr), flightNumber(std::move(number)) {
        tower->registerPlane(this);
    }
    
    void notify(const std::string& message) override {
        std::cout << "Flight " << flightNumber << " received: " << message << "\n";
    }
    
    void send(const std::string& message) override {
        std::cout << "Flight " << flightNumber << " sending: " << message << "\n";
        tower->relayMessage(this, message);
    }
};

int main() {
    auto tower = std::make_unique<AirportControlTower>();
    
    CommercialFlight flight1(tower.get(), "SU-100");
    CommercialFlight flight2(tower.get(), "AA-200");
    CommercialFlight flight3(tower.get(), "LH-300");
    
    flight1.send("Requesting landing clearance");
    flight2.send("Reporting altitude 5000m");
}
