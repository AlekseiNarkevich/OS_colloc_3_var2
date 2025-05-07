#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <QObject>

// Реализация без Qt (стандартный C++)
class StockPublisher;

class MarketObserver {
public:
    virtual void update(double price) = 0;
    virtual ~MarketObserver() = default;
};

class StockPublisher {
    std::vector<MarketObserver*> observers;
    double currentPrice;
    
public:
    void attach(MarketObserver* observer) {
        observers.push_back(observer);
    }
    
    void detach(MarketObserver* observer) {
        observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
    }
    
    void setPrice(double price) {
        currentPrice = price;
        notifyObservers();
    }
    
private:
    void notifyObservers() {
        for (auto* observer : observers) {
            observer->update(currentPrice);
        }
    }
};

class Trader : public MarketObserver {
    std::string name;
    
public:
    Trader(std::string name) : name(std::move(name)) {}
    
    void update(double price) override {
        std::cout << name << " notified: price changed to " << price << "\n";
    }
};

// Реализация с Qt
class StockMarket : public QObject {
    Q_OBJECT
    double currentPrice;
    
signals:
    void priceChanged(double newPrice);
    
public:
    void setPrice(double price) {
        currentPrice = price;
        emit priceChanged(currentPrice);
    }
};

class QtTrader : public QObject {
    Q_OBJECT
    QString traderName;
    
public slots:
    void onPriceChanged(double price) {
        qDebug() << traderName << "received price update:" << price;
    }
};

// Для демонстрации Qt-версии
#include <QCoreApplication>
int main(int argc, char* argv[]) {
    // Стандартная реализация
    StockPublisher publisher;
    Trader trader1("John"), trader2("Alice");
    
    publisher.attach(&trader1);
    publisher.attach(&trader2);
    
    publisher.setPrice(150.75);
    publisher.detach(&trader1);
    publisher.setPrice(152.30);
    
    // Qt реализация
    QCoreApplication app(argc, argv);
    
    StockMarket market;
    QtTrader qtTrader1, qtTrader2;
    
    QObject::connect(&market, &StockMarket::priceChanged,
                    &qtTrader1, &QtTrader::onPriceChanged);
    
    market.setPrice(300.50);
    
    return app.exec();
}
