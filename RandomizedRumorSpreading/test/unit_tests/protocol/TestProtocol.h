#ifndef RANDOMIZEDRUMORSPREADING_TESTPROTOCOL_H
#define RANDOMIZEDRUMORSPREADING_TESTPROTOCOL_H

#include <map>
#include <unordered_set>
#include <RumorMember.h>
#include <Message.h>

#include "gtest/gtest.h"

class TestProtocol {
  private:
    std::unordered_set<int>                   m_peerIds;
    RRS::NetworkConfig                        m_networkConfig;
    std::unordered_map<int, RRS::RumorMember> m_members;
    std::map<std::string, int>                m_StringToRumorId;
    std::map<int, const std::string *>        m_rumorIdToStringPtr;
    std::chrono::milliseconds                 m_tickInterval;
    int                                       m_numTicks;

    void constructNetwork(size_t numOfPeers);

    int nextId(int memberId) const;

    void handleMessage(int fromMember, int toMember, const RRS::Message& msg);

  public:
    TestProtocol(size_t numPeers);

    virtual ~TestProtocol();

    void insertRumor(int rumorId, const std::string& gossip);

    void addRumor(int memberId, int rumorId);

    void clear();

    void tick();

    int numTicks() const;

    const std::unordered_set<int>& peers() const;

    const RRS::NetworkConfig networkConfig() const;

    const std::unordered_map<int, RRS::RumorMember>& members() const;

    bool isRumorOld(int rumorId) const;

    bool allRumorsOld() const;

    const std::chrono::milliseconds& tickInterval() const;

    std::ostream& printRumorsState(std::ostream& outStream) const;
};

#endif //RANDOMIZEDRUMORSPREADING_TESTPROTOCOL_H
