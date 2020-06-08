#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include <memory>
#include <future>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

template <class T>
class MessageQueue
{
public:
	void send(T&& t);
	T receive();
private:
	std::mutex _mtx;
	std::condition_variable _cond;
	std::deque<T> _queue;
};

enum class TrafficLightPhase {    red = 0, green = 1 };
// The TrafficLight class has the public methods „void waitForGreen()“ and „void simulate()“
// as well as „TrafficLightPhase getCurrentPhase()“, where TrafficLightPhase is an enum that
// can be either „red“ or „green“. Also, add the private method „void cycleThroughPhases()“.

class TrafficLight final: public TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();
    ~TrafficLight(){};
    // getters / setters
    TrafficLightPhase getCurrentPhase();
    // typical behaviour methods
    void waitForGreen();
    void simulate();
    
private:

    void cycleThroughPhases();
    TrafficLightPhase _currentPhase;
    std::condition_variable _condition;
    std::mutex _mutex;
    std::shared_ptr<MessageQueue<TrafficLightPhase>> _messageQ;
};

#endif

