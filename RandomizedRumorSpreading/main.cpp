#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <unordered_set>

#include <RumorMember.h>
#include <NetworkConfig.h>
#include <MemberID.h>

using namespace RRS;

int main (int argc, char *argv[]) {
    int numOfPeers = 10;
    std::unordered_set<int> peers;
    for (int i = 0; i < numOfPeers; ++i) {
        peers.insert(MemberID::next());
    }

    std::unordered_set<RumorMember, MemberHash> members;
    for (auto i : peers) {
        members.insert({peers, i});
    }

    std::cout << "DONE" << std::endl;
}

