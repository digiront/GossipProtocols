#include <random>
#include "RumorMember.h"

namespace RRS {

RumorMember::RumorMember(int id, const std::unordered_set<int>& m_peers)
: m_id(id)
, m_networkState(m_peers.size())
, m_peers(m_peers)
, m_rumors()
, m_mutex()
{
    m_networkState.statistics()["numMembers"] += 1;
}

RumorMember::RumorMember(const RumorMember& other)
: m_id(other.m_id)
, m_networkState(other.m_networkState)
, m_peers(other.m_peers)
, m_rumors(other.m_rumors)
, m_mutex()
{
}

RumorMember::RumorMember(RumorMember&& other)
: m_id(std::move(other.m_id))
, m_networkState(std::move(other.m_networkState))
, m_peers(std::move(other.m_peers))
, m_rumors(std::move(other.m_rumors))
, m_mutex()
{
}

bool RumorMember::addRumor(int rumorId)
{
    std::lock_guard<std::mutex> guard(m_mutex); // critical section
    return m_rumors.insert(std::make_pair(rumorId, &m_networkState)).second;
}

std::pair<int, std::vector<Message>> RumorMember::receivedMessage(const Message& message, int fromPeer)
{
    std::lock_guard<std::mutex> guard(m_mutex); // critical section

    bool isNewPeer = m_peersInCurrentRound.insert(fromPeer).second;
    m_networkState.statistics()["numMessagesReceived"] += 1;

    std::vector<Message> pullMessages;
    if (isNewPeer && message.type() == Message::PUSH) {
        for (auto& kv : m_rumors) {
            RumorStateMachine& stateMach = kv.second;
            if (stateMach.currentRound() >= 0) {
                pullMessages.push_back({ Message::PULL, kv.first, kv.second.currentRound() });
            }
        }

        // No PULL messages to sent i.e. no rumors received yet
        if (pullMessages.empty()) {
            pullMessages.push_back({ Message::PULL, -1, 0 });
            m_networkState.statistics()["numEmptyPullMessages"] += 1;
        }
    }

    // An empty response from a peer that was sent a PUSH
    const int receivedRumorId = message.rumorId();
    const int theirRound = message.round();
    if (receivedRumorId >= 0) {
        if (m_rumors.count(receivedRumorId) > 0) {
            m_rumors[receivedRumorId].rumorReceived(fromPeer, message.round());
        }
        else {
            m_rumors[receivedRumorId] = RumorStateMachine(&m_networkState, fromPeer, theirRound);
        }
    }

    return std::make_pair(fromPeer, pullMessages);
}

std::pair<int, std::vector<Message>> RumorMember::advanceRound()
{
    std::lock_guard<std::mutex> guard(m_mutex); // critical section

    m_networkState.statistics()["rounds"] += 1;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, m_peers.size());
    int randomIndex = dis(gen);

    std::vector<Message> pushMessages;

    for (auto& kv : m_rumors) {
        RumorStateMachine& stateMach = kv.second;
        stateMach.advanceRound(m_peersInCurrentRound);
        if (stateMach.currentRound() >= 0) {
            pushMessages.push_back({ Message::PUSH, kv.first, kv.second.currentRound() });
        }
    }
    m_networkState.statistics()["numPushMessages"] += pushMessages.size();

    // No PUSH messages but still want to sent a response to peer.
    if (pushMessages.empty()) {
        pushMessages.push_back({ Message::PUSH, -1, 0 });
        m_networkState.statistics()["numEmptyPushMessages"] += 1;
    }

    m_peersInCurrentRound.clear();
    return std::make_pair(*m_peers.find(randomIndex), pushMessages);
}

int RumorMember::id() const
{
    return m_id;
}

bool RumorMember::operator==(const RumorMember& other) const
{
    return m_id == other.m_id;
}

size_t MemberHash::operator()(const RRS::RumorMember& obj) const
{
    return obj.id();
}

} // project namespace