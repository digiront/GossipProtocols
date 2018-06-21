#ifndef RANDOMIZEDRUMORSPREADING_MESSAGE_H
#define RANDOMIZEDRUMORSPREADING_MESSAGE_H

#include <vector>
#include <memory>

namespace RRS {

class Message {
  public:
    enum Type {
        UNDEFINED,
        PUSH,
        PULL,
    };

  private:
    Type m_type;
    int m_rumorId;
    int m_round;

  public:
    Message();

    Message(Type type,
            int rumorId,
            int round);

    bool operator==(const Message& other) const;

    bool operator!=(const Message& other) const;
};

} // project namespace

#endif //RANDOMIZEDRUMORSPREADING_MESSAGE_H
