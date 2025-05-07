#include <iostream>
#include <memory>
#include <mutex>

// Базовый интерфейс состояния
class NetworkState {
public:
    virtual void connect() = 0;
    virtual void disconnect() = 0;
    virtual ~NetworkState() = default;
};

// Контекст, использующий состояния
class NetworkConnection {
    std::unique_ptr<NetworkState> currentState;
    std::mutex stateMutex;

public:
    explicit NetworkConnection(std::unique_ptr<NetworkState> initialState);
    
    void changeState(std::unique_ptr<NetworkState> newState) {
        std::lock_guard<std::mutex> lock(stateMutex);
        currentState = std::move(newState);
    }

    void establishConnection() {
        std::lock_guard<std::mutex> lock(stateMutex);
        currentState->connect();
    }

    void terminateConnection() {
        std::lock_guard<std::mutex> lock(stateMutex);
        currentState->disconnect();
    }
};

// Конкретные состояния
class DisconnectedState : public NetworkState {
public:
    void connect() override {
        std::cout << "Establishing new connection...\n";
        // Логика подключения
    }
    
    void disconnect() override {
        std::cout << "Already disconnected\n";
    }
};

class ConnectedState : public NetworkState {
public:
    void connect() override {
        std::cout << "Already connected\n";
    }
    
    void disconnect() override {
        std::cout << "Terminating connection...\n";
        // Логика отключения
    }
};

NetworkConnection::NetworkConnection(std::unique_ptr<NetworkState> initialState)
    : currentState(std::move(initialState)) {}

int main() {
    auto connection = NetworkConnection(std::make_unique<DisconnectedState>());
    
    connection.establishConnection(); // Подключаемся
    connection.establishConnection(); // Уже подключены
    
    connection.changeState(std::make_unique<ConnectedState>());
    connection.terminateConnection(); // Отключаемся
}
