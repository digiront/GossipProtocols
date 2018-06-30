#include <chrono>
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
    std::vector<int>                          m_rumors;
    int pushMessageCount = 0;
    int pullMessageCount = 0;
    int epochCount = 0;

    std::function<void(Time now, int from, int to, const Message& msg)> send;

    explicit System(int numOfPeers)
    {
        std::unordered_set<int> peerIds;

        for (int i = 0; i < numOfPeers; ++i) {
            peerIds.insert(i);
        }

        for (auto i : peerIds) {
            m_members.insert(std::make_pair(i, RumorMember(peerIds, i)));
        }
    }

    void addRumor(int memberId, int rumorId)
    {
        if (m_members.count(memberId) <= 0) {
            return;
        }

        m_rumors.push_back(rumorId);

        m_members.find(memberId)->second.addRumor(rumorId);
    }

    bool allRumorsOld() const
    {
        auto isRumorOld = [&](int rumorId) -> bool
        {
            for (const auto& kv : m_members) {
                if (!kv.second.done(rumorId)) {
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
        for (auto& kv : m_members) {
            std::pair<int, std::vector<Message>> roundResult = kv.second.advanceRound();
            ++epochCount;

            int from = kv.first;
            int to = roundResult.first;
            EXPECT_GT(m_members.count(to), 0);

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
            ++pullMessageCount;
        }
        else {
            ASSERT_EQ(msg.type(), Message::PUSH);
            ++pushMessageCount;
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
    long completedAtSeconds = 0;

    explicit CheckAllDone(System& _system)
    : system(_system)
    {
    }

    void operator() (Time now)
    {
        if (!allRumorsOld && system.allRumorsOld()) {
            completedAtSeconds = toSeconds(now);
            allRumorsOld = true;
        }
    };
};

TEST(SystemTest, Smoke)
{
    for (int i = 0; i < 10; ++i)
    {
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
        EXPECT_GT(system.pushMessageCount, 1550);
        EXPECT_LT(system.pushMessageCount, 1580);
        EXPECT_GT(system.pullMessageCount, 1430);
        EXPECT_LT(system.pullMessageCount, 1510);
        EXPECT_EQ(system.epochCount, 1592);
        EXPECT_GT(checkAllDone.completedAtSeconds, 20);
        EXPECT_LT(checkAllDone.completedAtSeconds, 80);

        std::cout << "Member statistics:" << std::endl;
        for (const auto& kv : system.m_members) {
            kv.second.printStatistics(std::cout);
        }
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    return ret;
}