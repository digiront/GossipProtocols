#include "Sim.h"

Sim::Sim()
        : m_todo() {
}

void Sim::at(Time time, const Action &what) {
    m_todo.insert(make_pair(time, what));
}

void Sim::timer(Time start, seconds period, const Action &what) {
    at(start + period, [=](Time now) {
        what(now);
        timer(now, period, what);
    });
}

void Sim::runTo(Time end) {
    while (true) {
        if (m_todo.empty()) {
            return;
        }
        auto timeAndAction = *m_todo.begin();
        m_todo.erase(m_todo.begin());
        auto& now = timeAndAction.first;
        if (now >= end) {
            return;
        }
        timeAndAction.second(now);
    }
}