#ifndef ATTACK_ON_TITAN_EVENT_H
#define ATTACK_ON_TITAN_EVENT_H
namespace jamesfchen_event {
    bool entry();
    bool TestEvent();

}
class Epoller {
public:
    static const int EPOLL_MAX_EVENTS = 16;
    static const int EPOLL_READ_SIZE = 10;
    static const int EPOLL_SIZE_HINT = 8;
    int wake_event_fd_=-1;
    int epoll_fd_=-1;

    Epoller();

    virtual ~Epoller();

    void run();
    void RecWakeEvent() const;
    void SendWakeEvent() const;
};

#endif //ATTACK_ON_TITAN_EVENT_H
