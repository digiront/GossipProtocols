#include "RumorMember.h"

#include <random>
#include <cassert>

#define LITERAL(s) #s

namespace RRS {

// STATIC MEMBERS
std::map<RumorMember::StatisticKey, std::string> RumorMember::s_enumKeyToString = {
    {NumPeers,             LITERAL(NumPeers)},
    {NumMessagesReceived,  LITERAL(NumMessagesReceived)},
    {Rounds,               LITERAL(Rounds)},
    {NumPushMessages,      LITERAL(NumPushMessages)},
    {NumEmptyPushMessages, LITERAL(NumEmptyPushMessages)},
    {NumPullMessages,      LITERAL(NumPullMessages)},
    {NumEmptyPullMessages, LITERAL(NumEmptyPullMessages)},
};

// PRIVATE METHODS
void RumorMember::increaseStatValue(StatisticKey key, double value)
{
    if (m_statistics.count(key) <= 0) {
        m_statistics[key] = value;
    }
    else {
        m_statistics[key] += value;
    }
}

// CONSTRUCTORS
RumorMember::RumorMember(const std::unordered_set<int>& peers, int id)
: m_id(id)
, m_networkConfig(peers.size())
, m_peers()
, m_rumors()
, m_mutex()
{
    // Copy the member ids into a vector
    for (const int p : peers) {
        if (p != id) {
            m_peers.push_back(p);
        }
    }
    increaseStatValue(StatisticKey::NumPeers, 1);
}

RumorMember::RumorMember(const std::unordered_set<int>& peers,
                         const NetworkConfig& networkConfig,
                         int id)
: m_id(id)
, m_networkConfig(networkConfig)
, m_peers()
, m_rumors()
, m_mutex()
{
    assert(networkConfig.networkSize() == peers.size());
    // Copy the member ids into a vector
    for (const int p : peers) {
        if (p != id) {
            m_peers.push_back(p);
        }
    }
    increaseStatValue(NumPeers, 1);
}

RumorMember::RumorMember(const RumorMember& other)
: m_id(other.m_id)
, m_networkConfig(other.m_networkConfig)
, m_peers(other.m_peers)
, m_rumors(other.m_rumors)
, m_mutex()
{
}

RumorMember::RumorMember(RumorMember&& other)
: m_id(other.m_id)
, m_networkConfig(other.m_networkConfig)
, m_peers(std::move(other.m_peers))
, m_rumors(std::move(other.m_rumors))
, m_mutex()
{
}

bool RumorMember::addRumor(int rumorId)
{
    std::lock_guard<std::mutex> guard(m_mutex); // critical section
    return m_rumors.insert(std::make_pair(rumorId, &m_networkConfig)).second;
}

std::pair<int, std::vector<Message>>
RumorMember::receivedMessage(const Message& message, int fromPeer)
{
    std::lock_guard<std::mutex> guard(m_mutex); // critical section

    bool isNewPeer = m_peersInCurrentRound.insert(fromPeer).second;
    increaseStatValue(NumMessagesReceived, 1);

    std::vector<Message> pullMessages;
    if (isNewPeer && message.type() == Message::PUSH) {
        for (auto& kv : m_rumors) {
            RumorStateMachine& stateMach = kv.second;
            if (stateMach.age() >= 0) {
                pullMessages.emplace_back(Message(Message::PULL, kv.first, kv.second.age()));
            }
        }

        // No PULL messages to sent i.e. no rumors received yet
        if (pullMessages.empty()) {
            pullMessages.emplace_back(Message(Message::PULL, -1, 0));
            increaseStatValue(NumEmptyPullMessages, 1);
        }
        else {
            increaseStatValue(NumPullMessages, pullMessages.size());
        }
    }

    // An empty response from a peer that was sent a PULL
    const int receivedRumorId = message.rumorId();
    const int theirRound = message.age();
    if (receivedRumorId >= 0) {
        if (m_rumors.count(receivedRumorId) > 0) {
            m_rumors[receivedRumorId].rumorReceived(fromPeer, message.age());
        }
        else {
            m_rumors[receivedRumorId] = RumorStateMachine(&m_networkConfig, fromPeer, theirRound);
        }
    }

    return std::make_pair(fromPeer, pullMessages);
}

std::pair<int, std::vector<Message>> RumorMember::advanceRound()
{
    std::lock_guard<std::mutex> guard(m_mutex); // critical section

    if(m_rumors.empty()) {
        return std::pair<int, std::vector<Message>>();
    }

    // This is a heuristic to determine if this Rumor Spreading instance is too old
    if (m_statistics[Rounds] >= 2 * m_networkConfig.maxRoundsTotal()) {
        return std::pair<int, std::vector<Message>>();
    }
    increaseStatValue(Rounds, 1);

    // Choose a random member
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, m_peers.size() - 1);
    int toMember = m_peers[dis(gen)];

    // Construct the push messages
    std::vector<int> oldRumorIds;
    std::vector<Message> pushMessages;
    for (auto& kv : m_rumors) {
        RumorStateMachine& stateMach = kv.second;
        stateMach.advanceRound(m_peersInCurrentRound);
        pushMessages.emplace_back(Message(Message::PUSH, kv.first, kv.second.age()));
    }
    increaseStatValue(NumPushMessages, pushMessages.size());

    // Move to old rumors
    for (const int rId : oldRumorIds) {
        m_rumors.erase(rId);
        m_oldRumors.insert(rId);
    }

    // No PUSH messages but still want to sent a response to peer.
    if (pushMessages.empty()) {
        pushMessages.emplace_back(Message(Message::PUSH, -1, 0));
        increaseStatValue(NumEmptyPushMessages, 1);
    }

    // Clear round state
    m_peersInCurrentRound.clear();

    return std::make_pair(toMember, pushMessages);
}

// PUBLIC CONST METHODS
int RumorMember::id() const
{
    return m_id;
}

const std::unordered_map<int, RumorStateMachine>& RumorMember::rumorsMap() const
{
    return m_rumors;
}

const std::map<RumorMember::StatisticKey, double>& RumorMember::statistics() const
{
    return m_statistics;
}

bool RumorMember::isOld(int rumorId) const
{
    std::lock_guard<std::mutex> guard(m_mutex); // critical section

    const auto& iter = m_rumors.find(rumorId);
    if (iter != m_rumors.end()) {
        return iter->second.isOld();
    }

    return m_oldRumors.count(rumorId) > 0;
}

std::ostream& RumorMember::printStatistics(std::ostream& outStream) const
{
    outStream << m_id << ": {" << "\n";
    for (const auto& stat : m_statistics) {
        outStream << "  " << s_enumKeyToString.at(stat.first) << ": " << stat.second << "\n";
    }
    outStream << "}";
    return outStream;
}

// OPERATORS
bool RumorMember::operator==(const RumorMember& other) const
{
    return m_id == other.m_id;
}

// FREE OPERATORS
int MemberHash::operator()(const RRS::RumorMember& obj) const
{
    return obj.id();
}

} // project namespace