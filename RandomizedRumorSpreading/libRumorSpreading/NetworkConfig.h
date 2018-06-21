#ifndef RANDOMIZEDRUMORSPREADING_NETWORKSTATE_H
#define RANDOMIZEDRUMORSPREADING_NETWORKSTATE_H

#include <string>
#include <unordered_map>

namespace RRS {

class NetworkConfig {
  private:
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

    // Each peer can store an arbitrary number of statistics. Not used for now.
    std::unordered_map<std::string, double> m_statistics;

  public:
    // Create a NetworkConfig instance with the default initialization based on theory.
    NetworkConfig(int numOfPeers);

    // Create a NetworkConfig with user specified configuration.
    NetworkConfig(int networkSize, int maxRoundsInB, int maxRoundsInC, int maxRoundsTotal);

    int networkSize() const;

    int maxRoundsInB() const;

    int maxRoundsInC() const;

    int maxRoundsTotal() const;

    std::unordered_map<std::string, double>& statistics();

};

} // project namespace

#endif //RANDOMIZEDRUMORSPREADING_NETWORKSTATE_H
