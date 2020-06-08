#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */




template <typename T>
void MessageQueue<T>::send(T &&msg)
{
	std::lock_guard<std::mutex> lock(_mtx);
	_queue.emplace_back(std::move(msg));
	_cond.notify_one();
}

template <typename T>
T MessageQueue<T>::receive()
{
	std::unique_lock<std::mutex> lock(_mtx);
	_cond.wait(lock, [this]() {return !_queue.empty(); });
	T res = std::move(_queue.back());
	_queue.pop_back();
	return res;
}
//

/* Implementation of class "TrafficLight" */
TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
    _messageQ = std::make_shared<MessageQueue<TrafficLightPhase>>();
}

void TrafficLight::waitForGreen()
{
    while (true){
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        auto msg = _messageQ->receive();
        if (msg == TrafficLightPhase :: green ) {
            return;
        }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
	threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> dis(4, 6);
    
    int cycleDuration = dis(gen);
    
    int cycleTime;
    auto lastTime = std::chrono::system_clock::now();
    auto timeElapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - lastTime).count();
    
    while(true){
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        timeElapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - lastTime).count();
        if (timeElapsed >= cycleDuration)
        {
			std::unique_lock<std::mutex> lock(_mtx);
			/* Toggle current phase of traffic light */
            if (_currentPhase == TrafficLightPhase :: red)
            {
                _currentPhase = TrafficLightPhase :: green;
            }
            else
            {
                _currentPhase = TrafficLightPhase :: red;
            }
            /* Send an update to the message queue and wait for it to be sent */
            auto currentPhase = _currentPhase;
            auto ftr = std::async(std::launch::async, &MessageQueue<TrafficLightPhase>::send, _messageQ, std::move(currentPhase));
			ftr.wait();
            /* Randomly choose the cycle duration for the next cycle */
            cycleDuration = dis(gen);
            /* Reset stop watch for next cycle */
            lastTime = std::chrono::system_clock::now();
        }
        
    }
    
}


