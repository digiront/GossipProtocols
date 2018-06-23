#include <cmath>
#include "NetworkConfig.h"

namespace RRS {

// CONSTRUCTORS
NetworkConfig::NetworkConfig(int numOfPeers)
: m_networkSize(numOfPeers)
, m_maxRoundsInB()
, m_maxRoundsInC()
, m_maxRoundsTotal()
, m_statistics()
{
    // Refer to "Randomized Rumor Spreading" paper
    int magicNumber = std::ceil(std::log(std::log(m_networkSize)));
    m_maxRoundsInB = std::max(1, magicNumber);
    m_maxRoundsInC = m_maxRoundsInB;
    m_maxRoundsTotal = std::ceil(std::log(m_networkSize));
}

NetworkConfig::NetworkConfig(int networkSize,
                             int maxRoundsInB,
                             int maxRoundsInC,
                             int maxRoundsTotal)
: m_networkSize(networkSize)
, m_maxRoundsInB(maxRoundsInB)
, m_maxRoundsInC(maxRoundsInC)
, m_maxRoundsTotal(maxRoundsTotal)
{}

// PUBLIC CONST METHODS
int NetworkConfig::networkSize() const
{
    return m_networkSize;
}

int NetworkConfig::maxRoundsInB() const
{
    return m_maxRoundsInB;
}

int NetworkConfig::maxRoundsInC() const
{
    return m_maxRoundsInC;
}

int NetworkConfig::maxRoundsTotal() const
{
    return m_maxRoundsTotal;
}

// PUBLIC METHODS
std::unordered_map<std::string, double>& NetworkConfig::statistics()
{
    return m_statistics;
}


bool NetworkConfig::operator==(const NetworkConfig& other) const
{
    return  m_networkSize == other.m_networkSize &&
            m_maxRoundsInB == other.m_maxRoundsInB &&
            m_maxRoundsInC == other.m_maxRoundsInC &&
            m_maxRoundsTotal == other.m_maxRoundsTotal &&
            m_statistics == other.m_statistics;
}

} // project namespace