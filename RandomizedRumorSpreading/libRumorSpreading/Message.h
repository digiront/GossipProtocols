#ifndef RANDOMIZEDRUMORSPREADING_MESSAGE_H
#define RANDOMIZEDRUMORSPREADING_MESSAGE_H

#include <vector>
#include <memory>

namespace RRS {

class Message {
  public:
    // ENUMS
    enum Type {
        UNDEFINED,
        PUSH,
        PULL,
    };

  private:
    // MEMBERS
    Type m_type;
    int m_rumorId;
    int m_round;

  public:
    // CONSTRUCTORS
    Message();

    Message(Type type,
            int rumorId,
            int round);

    // OPERATORS
    bool operator==(const Message& other) const;

    bool operator!=(const Message& other) const;

    // CONST METHODS
    Type type() const;

    int rumorId() const;

    int round() const;
};

} // project namespace

#endif //RANDOMIZEDRUMORSPREADING_MESSAGE_H
