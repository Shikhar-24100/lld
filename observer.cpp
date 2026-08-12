#include <iostream>
#include <vector>
#include <algorithm>

// 1. Observer Interface
class IObserver {
    public:
        virtual ~IObserver() = default;
        virtual void update(float temperature) = 0;
};

// 2. Subject Interface
class ISubject {
public:
    virtual ~ISubject() = default;
    virtual void attach(IObserver* observer) = 0;
    virtual void detach(IObserver* observer) = 0;
    virtual void notify() = 0;
};


// 3. Concrete Subject
class WeatherStation : public ISubject {
private:
    std::vector<IObserver*> observers;
    float temperature = 0.0f;

public:
    void attach(IObserver* observer) override {
        observers.push_back(observer);
    }

    void detach(IObserver* observer) override {
        observers.erase(
            std::remove(observers.begin(), observers.end(), observer),
            observers.end()
        );
    }

    void notify() override {
        for (IObserver* observer : observers) {
            observer->update(temperature);
        }
    }

    void setTemperature(float temp) {
        temperature = temp;
        notify(); // Automatic broadcast on state change
    }
};


// 4. Concrete Observers
class PhoneDisplay : public IObserver {
public:
    void update(float temperature) override {
        std::cout << "[Phone Display] Current Temp: " << temperature << "°C\n";
    }
};

class WindowDisplay : public IObserver {
public:
    void update(float temperature) override {
        std::cout << "[Window Display] Current Temp: " << temperature << "°C\n";
    }
};



int main() {
    WeatherStation station;
    PhoneDisplay phone;
    WindowDisplay window;

    station.attach(&phone);
    station.attach(&window);

    // Triggers update to both phone and window
    station.setTemperature(24.5f);

    // Detach one observer
    station.detach(&window);

    // Triggers update to phone only
    station.setTemperature(27.0f);
}


