#ifndef RANDOMIZEDRUMORSPREADING_MESSAGESTATE_H
#define RANDOMIZEDRUMORSPREADING_MESSAGESTATE_H

#include <array>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include "NetworkConfig.h"

namespace RRS {

class RumorStateMachine {
  public:
    // ENUMS
    enum State {
        UNKNOWN, // initial state where the peer 'v' doesn't know about the rumor 'r'
        NEW,     // the peer 'v' knows 'r' and counter(v,r) = m
        KNOWN,   // cooling state, stay in this state for a 'm_maxRounds' rounds
        OLD,     // final state, member stops participating in rumor spreading
        NUM_STATES
    };

  private:
    // MEMBERS
    State                        m_state;
    const NetworkConfig&         m_networkConfigRef;
    int                          m_currentRound;
    int                          m_roundsInB;
    int                          m_roundsInC;
    std::unordered_map<int, int> m_memberRounds; // Member ID --> round

    // METHODS
    void newHandler(const std::unordered_set<int>& membersInRound);

    void knownHandler();

  public:
    RumorStateMachine(const NetworkConfig& networkConfigRef);

    void rumorReceived(int memberId, int theirRound);

    void advanceRound(const std::unordered_set<int>& peersInCurrentRound);

    const State state() const;
};

} // project namespace

#endif //RANDOMIZEDRUMORSPREADING_MESSAGESTATE_H
