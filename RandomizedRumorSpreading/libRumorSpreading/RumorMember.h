#ifndef RANDOMIZEDRUMORSPREADING_RUMORMEMBER_H
#define RANDOMIZEDRUMORSPREADING_RUMORMEMBER_H

#include <map>
#include <unordered_set>
#include <mutex>

#include "RumorSpreadingInterface.h"
#include "MemberID.h"
#include "NetworkConfig.h"
#include "RumorStateMachine.h"

namespace RRS {

// This is a thread-safe implementation of the 'RumorSpreadingInterface'.
class RumorMember : public RumorSpreadingInterface {
  public:
    // ENUMS
    enum StatisticKey {
        NumPeers,
        NumMessagesReceived,
        Rounds,
        NumPushMessages,
        NumEmptyPushMessages,
        NumPullMessages,
        NumEmptyPullMessages,
    };

    static std::map<StatisticKey, std::string> s_enumKeyToString;

  private:
    // MEMBERS
    const int                                  m_id;
    NetworkConfig                              m_networkConfig;
    std::vector<int>                           m_peers;
    std::unordered_set<int>                    m_peersInCurrentRound;
    std::unordered_map<int, RumorStateMachine> m_rumors;
    std::mutex                                 m_mutex;
    std::map<StatisticKey, double>             m_statistics;

    // METHODS
    void increaseStatValue(StatisticKey key, double value);

  public:
    // CONSTRUCTORS
    // Automatically figures out the network parameters based on 'peers.size()'.
    RumorMember(const std::unordered_set<int>& peers, int id = MemberID::next());

    // Used for manually passed network parameters.
    RumorMember(const std::unordered_set<int>& peers,
                const NetworkConfig& networkConfig,
                int id = MemberID::next());

    RumorMember(const RumorMember& other);

    RumorMember(RumorMember&& other);

    // METHODS
    bool addRumor(int rumorId) override;

    std::pair<int, std::vector<Message>> receivedMessage(const Message& message, int fromPeer) override;

    std::pair<int, std::vector<Message>> advanceRound() override;

    // CONST METHODS
    int id() const;

    const std::unordered_map<int, RumorStateMachine>& rumorsMap() const;

    bool isOld(int rumorId) const;

    const std::map<StatisticKey, double>& statistics() const;

    std::ostream& printStatistics(std::ostream& outStream) const;

    bool operator==(const RumorMember& other) const;
};

// Required by std::unordered_set
struct MemberHash {
    size_t operator() (const RumorMember& obj) const;
};

} // project namespace

#endif //RANDOMIZEDRUMORSPREADING_RUMORMEMBER_H
