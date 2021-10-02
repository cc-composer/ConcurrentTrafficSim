#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */
template <typename T>
T MessageQueue<T>::Receive()
{
    std::unique_lock<std::mutex> uLck(_mutex);
    _cond.wait(uLck, [this] { return !_deque.empty(); });

    T message = std::move(_deque.back());
    _deque.pop_back();

    return message;
}

template <typename T>
void MessageQueue<T>::Send(T &&message)
{
    std::lock_guard<std::mutex> lck(_mutex);
    _deque.emplace_back(message);
    _cond.notify_one();
}

/* Implementation of class "TrafficLight" */
TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::WaitForGreen()
{
    while(_currentPhase == red)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        _messageQueue.Receive();
    }
}

TrafficLightPhase TrafficLight::GetCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    threads.emplace_back(std::thread(&TrafficLight::CycleThroughPhases, this)); 
}

void TrafficLight::CycleThroughPhases()
{
    std::chrono::time_point<std::chrono::system_clock> timerStart;
    
    //Create a random wait time between 4000ms and 6000ms
    std::random_device random;
    std::uniform_int_distribution<int> range(4000, 6000);
    double randWaitTime = range(random);

    //Starting timer
    timerStart = std::chrono::system_clock::now();
    
    while(true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        long timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - timerStart).count();
        
        if (timeSinceLastUpdate >= randWaitTime)
        {
            if(_currentPhase == red) 
            { 
                _currentPhase = green;
                std::cout << "The light is now green.\n";
            }
            else 
            { 
                _currentPhase = red;
                std::cout << "The light is now red.\n";
            }

            _messageQueue.Send(std::move(_currentPhase));
            timerStart = std::chrono::system_clock::now();
        }
    }
}