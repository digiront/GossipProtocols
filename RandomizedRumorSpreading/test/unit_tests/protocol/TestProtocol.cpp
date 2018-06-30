#include "TestProtocol.h"

// STD
#include <condition_variable>
#include <mutex>

// RRS
#include <MemberID.h>
#include <thread>
#include <cmath>

using namespace RRS;

void TestProtocol::constructNetwork(int numOfPeers)
{
    for (int i = 0; i < numOfPeers; ++i) {
        m_peerIds.insert(i);
    }

    for (auto i : m_peerIds) {
        m_members.insert(std::make_pair(i, RumorMember(m_peerIds, i)));
    }
}

TestProtocol::TestProtocol(int numPeers)
: m_peerIds()
, m_members()
, m_StringToRumorId()
, m_rumorIdToStringPtr()
, m_tickInterval(100)
{
    constructNetwork(numPeers);
}

TestProtocol::~TestProtocol()
{
};

void TestProtocol::insertRumor(int rumorId, const std::string& gossip)
{
    m_StringToRumorId[gossip] = rumorId;
    auto iter = m_StringToRumorId.find(gossip);
    m_rumorIdToStringPtr[rumorId] = &(iter->first);
}

void TestProtocol::addRumor(int memberId, int rumorId)
{
    if (m_members.count(memberId) <= 0) {
        return;
    }

    m_members.find(memberId)->second.addRumor(rumorId);
}

void TestProtocol::clear()
{
    m_rumorIdToStringPtr.clear();
    m_StringToRumorId.clear();
}

void TestProtocol::handleMessage(int fromMember, int toMember, const Message& msg)
{
    RumorMember& member = m_members.find(toMember)->second;
    std::pair<int, std::vector<Message>> recvResult =  member.receivedMessage(msg, fromMember);
    for (const auto& pullMsg : recvResult.second) {
        EXPECT_EQ(pullMsg.type(), Message::PULL);

        // handle each message in another thread
        std::thread(std::bind(&TestProtocol::handleMessage,
                    this,
                    member.id(),
                    recvResult.first,
                    pullMsg))
                    .detach();
    }
}

void TestProtocol::tick()
{
    m_numTicks++;
    for (auto& kv : m_members) {
        std::pair<int, std::vector<Message>> roundResult = kv.second.advanceRound();

        int from = kv.first;
        int to = roundResult.first;
        EXPECT_GT(m_members.count(to), 0);

        std::vector<Message>& pushMessages = roundResult.second;
        for (const auto& pushMsg : pushMessages) {
            EXPECT_EQ(pushMsg.type(), Message::PUSH);
            handleMessage(from, to, pushMsg);
        }
    }
}

int TestProtocol::numTicks() const
{
    return m_numTicks;
}

const std::unordered_set<int>& TestProtocol::peers() const
{
    return m_peerIds;
}

const std::unordered_map<int, RRS::RumorMember>& TestProtocol::members() const
{
    return m_members;
}

bool TestProtocol::isRumorOld(int rumorId) const
{
    for (const auto& kv : m_members) {
        if (!kv.second.rumorExists(rumorId)) {
            continue;
        }

        if (!kv.second.isOld(rumorId)) {
            return false;
        }
    }
    return true;
}

bool TestProtocol::allRumorsOld() const
{
    for (const auto& kv: m_StringToRumorId) {
        if (!isRumorOld(kv.second)) {
            return false;
        }
    }
    return true;
}

const std::chrono::milliseconds& TestProtocol::tickInterval() const
{
    return m_tickInterval;
}

std::ostream& TestProtocol::printRumorState(std::ostream& outStream) const
{
    typedef std::unordered_map<int, RumorStateMachine> RumorsMap;
    for (const auto& kv : m_members) {
        const int memberId = kv.first;
        const RumorsMap& rumorsMap = kv.second.rumorsMap();
        for (const auto& kv2 : rumorsMap) {
            const int rumorId = kv2.first;
            outStream << "\n{ MemberId: " << memberId
                      << ", RumorId: " << rumorId
                      << ", State: " << kv2.second
                      << "}";
        }
    }
    return outStream;
}

// *** TEST CASES ***
TEST(TestProtocol, Spread_One_Rumor)
{
    std::mutex mutex;
    std::condition_variable cond_var;

    TestProtocol testObj(8);
    // Add rumor and map it to an int
    testObj.insertRumor(0, "Are Eminem and Nicki Minaj an item, or are they messing with us?");

    // First member will be the first one to start spreading the rumor
    testObj.addRumor(0, 0);

    // Schedule periodic ticks
    std::thread([&]()
    {
        while (!testObj.allRumorsOld()) {
            testObj.tick();
            std::this_thread::sleep_for(testObj.tickInterval());
        }

        // mark as isOld and signal to exit
        std::lock_guard<std::mutex> lock(mutex);
        cond_var.notify_one();
    }).detach();

    // Schedule timeout
    std::thread([&]()
    {
        int lnn = std::ceil(std::log(testObj.peers().size()));
        std::this_thread::sleep_for(
                                 std::chrono::milliseconds(2 * lnn * testObj.tickInterval()));
        std::lock_guard<std::mutex> lock(mutex);
        cond_var.notify_one();
    }).detach();

    {
        std::unique_lock<std::mutex> lock(mutex);
        cond_var.wait(lock);
        EXPECT_TRUE(testObj.allRumorsOld());

        std::cout << "Rumor/Peer state:\n[";
        testObj.printRumorState(std::cout) << "\n]\n" << std::endl;

        std::cout << "Member statistics: [" << std::endl;
        for (const auto& kv : testObj.members()) {
            kv.second.printStatistics(std::cout) << "\n";
        }
        std::cout << "]" << std::endl;
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    return ret;
}