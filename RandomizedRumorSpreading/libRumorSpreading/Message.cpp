#include <Message.h>
#include "Message.h"

namespace RRS {

Message::Message()
{}

Message::Message(Message::Type type,
                 int rumorId,
                 int round)
: m_type(type)
  , m_rumorId(rumorId)
  , m_round(round)
{}

bool Message::operator==(const Message& other) const
{
    return m_type == other.m_type &&
           m_rumorId == other.m_rumorId &&
           m_round == other.m_round;
}

bool Message::operator!=(const Message& other) const
{
    return !(*this == other);
}

} // project namespace