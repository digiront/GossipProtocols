#include "RumorStateMachine.h"

namespace RRS {

// PRIVATE METHODS
void RumorStateMachine::newHandler(const std::unordered_set<int>& membersInRound)
{
    m_roundsInB++;
    if (m_currentRound >= m_networkConfigRef.maxRoundsTotal()) {
        m_state = OLD;
        return;
    }

    for (auto id : membersInRound) {
        if (m_memberRounds.count(id) <= 0) m_memberRounds[id] = 0;
    }

    int numLess = 0;
    int numGreaterOrEqual = 0;
    for (const auto entry : m_memberRounds) {
        int theirRound = entry.second;

        if (theirRound < m_currentRound) {
            numLess++;
        }
        else if (theirRound >= m_networkConfigRef.maxRoundsInB()) {
            m_state = KNOWN;
        }
        else {
            numGreaterOrEqual++;
        }
    }

    if (numGreaterOrEqual > numLess) {
        m_roundsInB++;
    }

    if (m_roundsInB >= m_networkConfigRef.maxRoundsInB()) {
        m_state = KNOWN;
    }
    m_memberRounds.clear();
}

void RumorStateMachine::knownHandler()
{
    m_roundsInC++;
    if (m_currentRound >= m_networkConfigRef.maxRoundsTotal() ||
        m_roundsInC >= m_networkConfigRef.maxRoundsInC()) {
        m_state = OLD;
    }

}

// CONSTRUCTORS
RumorStateMachine::RumorStateMachine(const NetworkConfig& networkConfigRef)
: m_state(State::UNKNOWN)
, m_networkConfigRef(networkConfigRef)
, m_currentRound(0)
, m_roundsInB(0)
, m_roundsInC(0)
, m_memberRounds()
{

}

void RumorStateMachine::rumorReceived(int memberId, int theirRound)
{
    // Maximum number of rounds reached
    if (theirRound > m_networkConfigRef.maxRoundsTotal()) {
        m_state = State::OLD;
        m_currentRound = 0;
        m_memberRounds.clear();
        return;
    }

    // Stay in B-m state
    m_state = State::NEW;

    if (m_memberRounds.count(memberId) > 0) {
        //TODO: handle unexpected case
    }
    m_memberRounds[memberId] = theirRound;
}

void RumorStateMachine::advanceRound(const std::unordered_set<int>& peersInCurrentRound)
{
    m_currentRound++;
    switch(m_state) {
        case NEW:
            newHandler(peersInCurrentRound);
            return;
        case KNOWN:
            knownHandler();
            return;
        default:
            // TODO: decide error handling strategy
            return;
    }
}

const RumorStateMachine::State RumorStateMachine::state() const
{
    return m_state;
}

} // project namespace