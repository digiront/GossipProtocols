#ifndef RANDOMIZEDRUMORSPREADING_MEMBERID_H
#define RANDOMIZEDRUMORSPREADING_MEMBERID_H

#include <atomic>

namespace RRS {

class MemberID {
  private:
    static std::atomic<int> d_id;

  public:
    static int next()
    {
        return ++d_id;
    }
};

} // project namespace

#endif //RANDOMIZEDRUMORSPREADING_MEMBERID_H
