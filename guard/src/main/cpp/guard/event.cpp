#include "event.h"
#include "log_ext.h"
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <string>

#define   LOG_TAG    "cjf_event"
using namespace jamesfchen_event;
// select/poll/epoll 的读写都是同步的属于同步io，异步io的读写是异步的，会自己把数据从内核copy到用户空间

int init_epoll_ctl(int epoll_fd, int fd) {
    struct epoll_event eventItem;
    memset(&eventItem, 0, sizeof(epoll_event)); // zero out unused members of data field union
    eventItem.events = EPOLLIN;
    eventItem.data.fd = fd;
    return epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &eventItem);
}

Epoller::Epoller() {
    LOG_E(LOG_TAG, "Epoller...");
    if (epoll_fd_ >= 0) {
        close(epoll_fd_);
    }
    epoll_fd_ = epoll_create(EPOLL_SIZE_HINT);
    if (epoll_fd_ == -1) {
        LOG_E(LOG_TAG, "Failed to create epoll file descriptor\n");
        return;
    }
    wake_event_fd_ = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (wake_event_fd_ == -1) {
        LOG_E(LOG_TAG, "mWakeEventFd == -1 ");
        return;
    }
    int result = init_epoll_ctl(epoll_fd_, wake_event_fd_);
    if (result == -1) {
        LOG_E(LOG_TAG, "mWakeEventFd  epoll_ctl init fail");
        return;
    }
//    int fd = 0;
//    result = init_epoll_ctl(epoll_fd_, fd);
//    if (result == -1) {
//        LOG_E(LOG_TAG, "fd  epoll_ctl init fail");
//    }

}


Epoller::~Epoller() {
    LOG_E(LOG_TAG, "~Epoller...");
    if (wake_event_fd_ > 0) {
        close(wake_event_fd_);
    }
    if (epoll_fd_ >= 0) {
        close(epoll_fd_);
    }
}

void Epoller::RecWakeEvent() const {
    uint64_t counter;
//    TEMP_FAILURE_RETRY(read(wake_event_fd_, &counter, sizeof(uint64_t)));
    size_t s = read(wake_event_fd_, &counter, sizeof(uint64_t));
    LOG_E(LOG_TAG, "RecWakeEvent counter:%d %d", counter, s);

}

void Epoller::SendWakeEvent() const {
    uint64_t inc = 1;
    ssize_t nWrite = TEMP_FAILURE_RETRY(write(wake_event_fd_, &inc, sizeof(uint64_t)));
    LOG_E(LOG_TAG, "SendWakeEvent counter:%d", nWrite);
    if (nWrite != sizeof(uint64_t)) {
        if (errno != EAGAIN) {
            LOG_E(LOG_TAG, "Could not write wake signal to fd %d: %s", wake_event_fd_,
                  strerror(errno));
        }
    }
}

void Epoller::run() {
    LOG_E(LOG_TAG, "run");
    size_t bytes_read;
    char read_buffer[EPOLL_READ_SIZE + 1];
    struct epoll_event events[EPOLL_MAX_EVENTS];
    int running = 1;
    while (running) {
        LOG_E(LOG_TAG, "\nPolling for input...\n");
        int event_count = epoll_wait(epoll_fd_, events, EPOLL_MAX_EVENTS, -1);//30s
//        int event_count = epoll_wait(epoll_fd_, events, EPOLL_MAX_EVENTS, 30000);//30s
//        int event_count = epoll_wait(epoll_fd_, events, EPOLL_MAX_EVENTS, 300);//30s
        LOG_E(LOG_TAG, "%d ready events\n", event_count);
        RecWakeEvent();
        for (int i = 0; i < event_count; i++) {
            LOG_E(LOG_TAG, "Reading file descriptor '%d' -- ", events[i].data.fd);
            bytes_read = read(events[i].data.fd, read_buffer, EPOLL_READ_SIZE);
            LOG_E(LOG_TAG, "%zd bytes read.\n", bytes_read);
            read_buffer[bytes_read] = '\0';
            LOG_E(LOG_TAG, "Read '%s'\n", read_buffer);

            if (!strncmp(read_buffer, "stop\n", 5))
                running = 0;
        }
    }
}

Epoller *epollerPtr;
Epoller epollerRef;//已经被初始化

void *run_thread(void *args) {
    epollerRef.run();
}

void threadDestructor(void *st) {
//   Looper* const self = static_cast<Looper*>(st);
//    if (self != NULL) {
//        self->decStrong((void*)threadDestructor);
//    }
}

static pthread_key_t gTLSKey = 0;
static pthread_once_t gTLSOnce = PTHREAD_ONCE_INIT;

void initTLSKey() {
    int result = pthread_key_create(&gTLSKey, threadDestructor);
    if (result != 0)LOG_E(LOG_TAG, "Could not allocate TLS key.");
}

bool jamesfchen_event::entry() {
//    epollerPtr = new Epoller();
    pthread_t tid;
    pthread_create(&tid, nullptr, run_thread, nullptr);
    int result = pthread_once(&gTLSOnce, initTLSKey);
    if (result != 0) LOG_E(LOG_TAG, "pthread_once failed");
//    pthread_setspecific(gTLSKey, looper.get());
//    pthread_getspecific(gTLSKey);
    return true;
}

bool jamesfchen_event::TestEvent() {
    epollerPtr->SendWakeEvent();
    return true;
}

