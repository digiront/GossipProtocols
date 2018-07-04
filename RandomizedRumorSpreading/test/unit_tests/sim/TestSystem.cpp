#include <chrono>
#include <limits>
#include <RumorMember.h>
#include <Message.h>
#include "gtest/gtest.h"
#include "Sim.h"

using namespace std::placeholders;
using namespace std::chrono;
using namespace RRS;

using Time = Sim::Time;

const unsigned START_TIME = 0;

const seconds sec(1);

long toSeconds(const Time& now)
{
    return duration_cast<seconds>(now.time_since_epoch()).count();
}

struct System {
    std::unordered_map<int, RRS::RumorMember> m_members;
    NetworkConfig m_networkConfig;
    std::vector<int> m_rumors;
    int m_pushMessageCount;
    int m_pullMessageCount;
    int m_epochCount;
    int m_numTicks;
    int m_maxNumTicks;

    std::function<void(Time now, int from, int to, const Message& msg)> send;

    explicit System(size_t numOfPeers)
    : m_members()
    , m_networkConfig(numOfPeers)
    , m_rumors()
    , m_pushMessageCount(0)
    , m_pullMessageCount(0)
    , m_epochCount(0)
    , m_numTicks(0)
    , m_maxNumTicks(0)
    {
        std::unordered_set<int> peerIds;

        for (int i = 0; i < numOfPeers; ++i) {
            peerIds.insert(i);
        }

        // This is an estimation, need to replace random selection with a callback
        m_maxNumTicks = 4 * m_networkConfig.maxRoundsTotal();

        for (auto i : peerIds) {
            m_members.insert(std::make_pair(i, RumorMember(peerIds, m_networkConfig)));
        }
    }

    void addRumor(int memberId, int rumorId)
    {
        if (m_members.count(memberId) <= 0) {
            return;
        }

        m_rumors.push_back(rumorId);

        bool added = m_members.find(memberId)->second.addRumor(rumorId);
        EXPECT_TRUE(added);
    }

    bool allRumorsOld() const
    {
        auto isRumorOld = [&](int rumorId) -> bool {
            for (const auto& kv : m_members) {
                if (!kv.second.rumorExists(rumorId)) {
                    continue;
                }

                if (!kv.second.isOld(rumorId)) {
                    return false;
                }
            }
            return true;
        };

        for (const auto& rumorId: m_rumors) {
            if (!isRumorOld(rumorId)) {
                return false;
            }
        }
        return true;
    }

    void tick(Time now)
    {
        ++m_epochCount;
        if (++m_numTicks >= m_maxNumTicks) {
            return;
        }

        for (auto& kv : m_members) {
            int from = kv.first;

            std::pair<int, std::vector<Message>> roundResult = kv.second.advanceRound();

            int to = roundResult.first;
            std::vector<Message>& pushMessages = roundResult.second;
            for (const auto& pushMsg : pushMessages) {
                EXPECT_EQ(pushMsg.type(), Message::PUSH);
                send(now, from, to, pushMsg);
            }
        }
    }

    void handleMessage(Time now, int fromMember, int toMember, const Message& msg)
    {
        if (msg.type() == Message::PULL) {
            ++m_pullMessageCount;
        } else {
            ASSERT_EQ(msg.type(), Message::PUSH);
            ++m_pushMessageCount;
        }

        RumorMember& member = m_members.find(toMember)->second;

        std::pair<int, std::vector<Message>> recvResult = member.receivedMessage(msg, fromMember);

        for (const auto& pullMsg : recvResult.second) {
            EXPECT_EQ(pullMsg.type(), Message::PULL);
            send(now, member.id(), recvResult.first, pullMsg);
        }
    }
};

class CheckAllDone {
    bool allRumorsOld = false;

    System& system;
  public:
    long completedAtSeconds = std::numeric_limits<long>::max();

    explicit CheckAllDone(System& _system)
    : system(_system)
    {
    }

    void operator()(Time now)
    {
        if (!allRumorsOld && system.allRumorsOld()) {
            completedAtSeconds = toSeconds(now);
            allRumorsOld = true;
        }
    };
};

TEST(SystemTest, Smoke)
{
    for (int i = 0; i < 100; ++i) {
        int numPeers = 8;
        const Time t0 = Time(duration<unsigned>(START_TIME));
        System system = System(8);

        Sim sim;
        system.send = [&](Time now, int from, int to, const Message& msg) {
            auto latency = (rand() % 5) * sec;
            sim.at(now + latency, [=, &system](Time now) {
                system.handleMessage(now, from, to, msg);
            });
        };

        sim.at(t0, [&](Time now) {
            const int memberId = 0;
            const int rumorId = 0;
            system.addRumor(memberId, rumorId);
            EXPECT_FALSE(system.allRumorsOld());
        });

        CheckAllDone checkAllDone(system);

        sim.timer(t0, 5 * sec, [&](Time now) {
            system.tick(now);
            checkAllDone(now);
        });

        sim.runTo(t0 + 1000 * sec);

        EXPECT_TRUE(system.allRumorsOld());
        EXPECT_GT(checkAllDone.completedAtSeconds, 9);
        EXPECT_LT(checkAllDone.completedAtSeconds, 50);

        int peersSquared = numPeers * numPeers;
        EXPECT_GT(system.m_pushMessageCount, 0);
        EXPECT_LT(system.m_pushMessageCount, peersSquared);

        EXPECT_GT(system.m_pullMessageCount, 0);
        EXPECT_LT(system.m_pullMessageCount, peersSquared);

        EXPECT_EQ(system.m_epochCount, 199);

    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    return ret;
}