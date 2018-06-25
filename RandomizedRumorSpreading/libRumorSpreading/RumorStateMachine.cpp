#include "RumorStateMachine.h"

namespace RRS {

// PRIVATE METHODS
void RumorStateMachine::advanceNew(const std::unordered_set<int>& membersInRound)
{
    m_roundsInB++;
    if (m_currentRound >= m_networkConfigPtr->maxRoundsTotal()) {
        advanceOld();
        return;
    }

    for (auto id : membersInRound) {
        if (m_memberRounds.count(id) <= 0) m_memberRounds[id] = 0;
    }

    // Compare our round to the majority of rounds
    int numLess = 0;
    int numGreaterOrEqual = 0;
    for (const auto entry : m_memberRounds) {
        int theirRound = entry.second;
        if (theirRound < m_currentRound) {
            numLess++;
        }
        else if (theirRound >= m_networkConfigPtr->maxRoundsInB()) {
            m_state = KNOWN;
        }
        else {
            numGreaterOrEqual++;
        }
    }

    if (numGreaterOrEqual > numLess) {
        m_roundsInB++;
    }

    if (m_roundsInB >= m_networkConfigPtr->maxRoundsInB()) {
        m_state = KNOWN;
    }
    m_memberRounds.clear();
}

void RumorStateMachine::advanceKnown()
{
    m_roundsInC++;
    if (m_currentRound >= m_networkConfigPtr->maxRoundsTotal() ||
        m_roundsInC >= m_networkConfigPtr->maxRoundsInC()) {
        advanceOld();
    }

}

void RumorStateMachine::advanceOld()
{
    m_state = OLD;
    m_memberRounds.clear();
    m_currentRound = m_networkConfigPtr->maxRoundsTotal() + 1;
}

// CONSTRUCTORS
RumorStateMachine::RumorStateMachine()
: m_state(State::NEW)
  , m_networkConfigPtr(0)
  , m_currentRound(-1)
  , m_roundsInB(-1)
  , m_roundsInC(-1)
  , m_memberRounds()
{
}

RumorStateMachine::RumorStateMachine(const NetworkConfig* networkConfigPtr)
: m_state(State::NEW)
, m_networkConfigPtr(networkConfigPtr)
, m_currentRound(0)
, m_roundsInB(0)
, m_roundsInC(0)
, m_memberRounds()
{
}

RumorStateMachine::RumorStateMachine(const NetworkConfig* networkConfigPtr,
                                     int fromMember,
                                     int theirRound)
: m_state(State::NEW)
  , m_networkConfigPtr(networkConfigPtr)
  , m_currentRound(0)
  , m_roundsInB(0)
  , m_roundsInC(0)
  , m_memberRounds()
{
    // Maximum number of rounds reached
    if (theirRound > m_networkConfigPtr->maxRoundsTotal()) {
        advanceOld();
        return;
    }

    // Stay in B-m state
    m_memberRounds[fromMember] = theirRound;
}

void RumorStateMachine::rumorReceived(int memberId, int theirRound)
{
    if (m_state == NEW) {
        if (m_memberRounds.count(memberId) > 0) {
            // TODO: handle duplicate message received from 'memberId'
        }
        m_memberRounds[memberId] = theirRound;
    }
}

void RumorStateMachine::advanceRound(const std::unordered_set<int>& peersInCurrentRound)
{
    m_currentRound++;
    switch(m_state) {
        case NEW:
            advanceNew(peersInCurrentRound);
            return;
        case KNOWN:
            advanceKnown();
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

const int RumorStateMachine::currentRound() const
{
    return m_currentRound;
}

} // project namespace