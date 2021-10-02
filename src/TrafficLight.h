#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

//@brief Red or Green
enum TrafficLightPhase
{
    red,
    green
};

template <class T>
class MessageQueue
{
public:
    void Send(T &&message);
    T Receive();

private:
    std::deque<TrafficLightPhase> _deque;
    std::condition_variable _cond;
    std::mutex _mutex;    
};

class TrafficLight : public TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();
    ~TrafficLight() {};

    // getters / setters
    TrafficLightPhase GetCurrentPhase();

    // typical behaviour methods
    void WaitForGreen();
    void simulate();

private:
    // typical behaviour methods
    void CycleThroughPhases();

    MessageQueue<TrafficLightPhase> _messageQueue;

    TrafficLightPhase _currentPhase;
    std::condition_variable _condition;
    std::mutex _mutex;
};



#endif