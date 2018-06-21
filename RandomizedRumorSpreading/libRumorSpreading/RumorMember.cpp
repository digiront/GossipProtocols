#include "RumorMember.h"

namespace RRS {

RumorMember::RumorMember(int id, const std::unordered_set<int>& m_peers)
: m_id(id)
  , m_networkState(m_peers.size())
  , m_peers(m_peers)
  , m_rumorIds()
{
}

bool RumorMember::addRumor(int rumorId)
{
    return m_rumorIds.insert(rumorId).second;
}

void RumorMember::receivedMessage(std::shared_ptr<Message> message,
                                  int fromPeer)
{

}

std::vector<std::shared_ptr<Message>> RumorMember::advanceRound()
{
    m_networkState.statistics()["rounds"] += 1;

    std::vector<Message> pushMessages;


}

void RumorMember::setCallback(const SendMessageCallback& sendMessageCallback)
{
    m_cb = sendMessageCallback;
}

bool RumorMember::operator==(const RumorMember& other) const
{
    return m_id == other.m_id;
}

int RumorMember::id() const
{
    return m_id;
}

size_t MemberHash::operator()(const RRS::RumorMember& obj) const
{
    return obj.id();
}

} // project namespace