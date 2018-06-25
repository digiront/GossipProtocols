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
        UNKNOWN,   // initial state where the peer 'v' doesn't know about the rumor 'r'
        NEW,       // the peer 'v' knows 'r' and counter(v,r) = m
        KNOWN,     // cooling state, stay in this state for a 'm_maxRounds' rounds
        OLD,       // final state, member stops participating in rumor spreading
        NUM_STATES
    };

  private:
    // MEMBERS
    State                        m_state;
    const NetworkConfig*         m_networkConfigPtr;
    int                          m_currentRound;
    int                          m_roundsInB;
    int                          m_roundsInC;
    std::unordered_map<int, int> m_memberRounds; // Member ID --> round

    // METHODS
    void advanceNew(const std::unordered_set<int>& membersInRound);

    void advanceKnown();

    void advanceOld();

  public:
    // CONSTRUCTORS
    // Default constructor. The returned state machine instance will be in an invalid state.
    RumorStateMachine();

    // Construct a new instance using the specified 'networkConfigPtr'.
    RumorStateMachine(const NetworkConfig* networkConfigPtr);

    // Construct a new instance using the specified 'networkConfigPtr', 'fromMember' and
    // 'theirRound' parameters.
    RumorStateMachine(const NetworkConfig* networkConfigPtr, int fromMember, int theirRound);

    // Copy constructor.
    RumorStateMachine(const RumorStateMachine& other) = default;

    // Move constructor.
    RumorStateMachine(RumorStateMachine&& other) = default;

    // OPERATORS
    // Assignment operator.
    RumorStateMachine& operator=(const RumorStateMachine& other) = default;

    // Move assignment operator.
    RumorStateMachine& operator=(RumorStateMachine&& other) = default;

    // METHODS
    void rumorReceived(int memberId, int theirRound);

    void advanceRound(const std::unordered_set<int>& peersInCurrentRound);

    // CONST METHODS
    const State state() const;

    const int currentRound() const;
};

} // project namespace

#endif //RANDOMIZEDRUMORSPREADING_MESSAGESTATE_H
