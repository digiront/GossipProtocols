#ifndef RANDOMIZEDRUMORSPREADING_NETWORKSTATE_H
#define RANDOMIZEDRUMORSPREADING_NETWORKSTATE_H

#include <string>
#include <unordered_map>

namespace RRS {

class NetworkConfig {
  private:
    // MEMBERS
    // Number of peers
    int m_networkSize;

    // Maximum number of rounds while in state B.
    // Can be configured. Specified in the paper as `O(ln(ln(n)))`.
    int m_maxRoundsInB;

    // Maximum number of rounds while in state C.
    // Can be configured. Specified in the paper as `O(ln(n))`.
    int m_maxRoundsInC;

    // The maximum number of rounds. This is termination condition for a given rumor.
    // Once a peer reaches this number of rounds it will advance to state D and consider the rumor
    // 'cold'. Can be configured. Specified in the paper as `O(ln(n))`.
    int m_maxRoundsTotal;

  public:
    // CONSTRUCTORS
    // Create a NetworkConfig instance with the default initialization based on theory.
    explicit NetworkConfig(int numOfPeers);

    // Create a NetworkConfig with user specified configuration.
    NetworkConfig(int networkSize, int maxRoundsInB, int maxRoundsInC, int maxRoundsTotal);

    // METHODS
    void increaseStatValue(const std::string& key, double value);

    // CONST METHODS
    int networkSize() const;

    int maxRoundsInB() const;

    int maxRoundsInC() const;

    int maxRoundsTotal() const;

    bool operator==(const NetworkConfig& other) const;
};

} // project namespace

#endif //RANDOMIZEDRUMORSPREADING_NETWORKSTATE_H
