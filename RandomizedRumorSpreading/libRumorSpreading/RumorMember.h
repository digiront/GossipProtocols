#ifndef RANDOMIZEDRUMORSPREADING_RUMORMEMBER_H
#define RANDOMIZEDRUMORSPREADING_RUMORMEMBER_H

#include <unordered_set>

#include "RumorSpreadingInterface.h"
#include "NetworkConfig.h"

namespace RRS {

class RumorMember : public RumorSpreadingInterface {
  private:
    int                     m_id;
    NetworkConfig            m_networkState;
    std::unordered_set<int> m_peers;
    std::unordered_set<int> m_rumorIds;
    SendMessageCallback     m_cb;

  public:
    RumorMember(int id, const std::unordered_set<int>& peers);

    bool addRumor(int rumorId) override;

    void receivedMessage(std::shared_ptr<Message> message, int fromPeer) override;

    std::vector<std::shared_ptr<Message>> advanceRound() override;

    void setCallback(const SendMessageCallback& sendMessageCallback) override;

    bool operator==(const RumorMember& other) const;

    int id() const;
};

struct MemberHash {
    size_t operator() (const RumorMember& obj) const;
};

} // project namespace

#endif //RANDOMIZEDRUMORSPREADING_RUMORMEMBER_H
