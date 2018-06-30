#ifndef RANDOMIZEDRUMORSPREADING_SIM_H
#define RANDOMIZEDRUMORSPREADING_SIM_H

#include <chrono>
#include <functional>
#include <map>

using namespace std::chrono;

class Sim {
public:

    using Time = steady_clock::time_point;

    using Action = std::function<void(Time)>;

private:
    using Actions = std::multimap<Time, Action>;

public:
    Sim();

    void at(Time time, const Action &what);

    void timer(Time start, seconds period, const Action &what);

    void runTo(Time end);

private:
    Actions m_todo;
};


#endif //RANDOMIZEDRUMORSPREADING_SIM_H
