#ifndef RANDOMIZEDRUMORSPREADING_RUMORMEMBER_H
#define RANDOMIZEDRUMORSPREADING_RUMORMEMBER_H

#include <unordered_set>
#include <mutex>

#include "RumorSpreadingInterface.h"
#include "NetworkConfig.h"
#include "RumorStateMachine.h"

namespace RRS {

// This is a thread-safe implementation of the 'RumorSpreadingInterface'.
class RumorMember : public RumorSpreadingInterface {
  private:
    // MEMBERS
    const int               m_id;
    NetworkConfig           m_networkState;
    std::unordered_set<int> m_peers;
    std::unordered_set<int> m_peersInCurrentRound;
    std::unordered_map<int, RumorStateMachine> m_rumors;
    std::mutex                                 m_mutex;

  public:
    // CONSTRUCTORS
    RumorMember(int id, const std::unordered_set<int>& peers);

    RumorMember(const RumorMember& other);

    RumorMember(RumorMember&& other);

    // METHODS
    bool addRumor(int rumorId) override;

    std::pair<int, std::vector<Message>> receivedMessage(const Message& message, int fromPeer) override;

    std::pair<int, std::vector<Message>> advanceRound() override;

    // CONST METHODS
    int id() const;

    bool operator==(const RumorMember& other) const;
};

// Required by std::unordered_set
struct MemberHash {
    size_t operator() (const RumorMember& obj) const;
};

} // project namespace

#endif //RANDOMIZEDRUMORSPREADING_RUMORMEMBER_H
