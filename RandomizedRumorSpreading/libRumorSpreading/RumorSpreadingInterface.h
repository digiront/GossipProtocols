#ifndef RANDOMIZEDRUMORSPREADING_RUMORSPREADINGINTERFACE_H
#define RANDOMIZEDRUMORSPREADING_RUMORSPREADINGINTERFACE_H

#include <set>
#include <vector>
#include <memory>
#include <functional>
#include "Message.h"

namespace RRS {

class RumorSpreadingInterface {
  public:
    typedef std::function<void(const Message&)> SendMessageCallback;

    virtual ~RumorSpreadingInterface();

    // Add a new rumor that will be spread to the peers. Return true if the rumor was successfully
    // added. The network is known a-priori and this algorithm does not consider new nodes that
    // join the network.
    // Disconnected nodes will miss the rumor however ths will not affect the rest of the network.
    // A maximum number of O(F) uniformed nodes is expected, where F is the number of disconnected
    // nodes.
    virtual bool addRumor(int rumorId) = 0;

    // Handle a new 'message' from peer 'fromPeer'. Int are used to identify a peer and a rumor in
    // order to abstract away the actual peer and rumor types.
    virtual void receivedMessage(std::shared_ptr<Message> message, int fromPeer) = 0;

    // Advance to the next round. Returns a vector of Message shared pointers. These are all PUSH
    // messages that will be sent to a random peer.
    virtual std::vector<std::shared_ptr<Message>> advanceRound() = 0;

    // Set the 'sendMessageCallback'. This will be called each time a message is ready. It is
    // recommended that the callback returns as soon as possible and all processing is done
    // asynchronously.
    virtual void setCallback(const SendMessageCallback& sendMessageCallback) = 0;
};

} // project namespace

#endif //RANDOMIZEDRUMORSPREADING_RUMORSPREADINGINTERFACE_H
