#ifndef __PUBLISHQUEUE_H__
#define __PUBLISHQUEUE_H__

#include "application.h"
#include <queue>

struct event_t {
    String eventName;
    String data;
    int ttl;
    PublishFlag flags;
};

class PublishQueue {
public:
    PublishQueue() : publishTimer(100, &PublishQueue::PublishTimerCallback, *this, false) {};
    void publish(String eventName, String data, int ttl=60, PublishFlag flags=PRIVATE) {
        event_t event = {.eventName=eventName, .data=data, .ttl=ttl, .flags=flags};
        event_queue.push(event);
        if (!publishTimer.isActive()) {
            publishTimer.changePeriod(100);
            publishTimer.start();
        }
    };
private:
    std::queue<event_t>event_queue;
    Timer publishTimer;
    void PublishTimerCallback() {
        if (!event_queue.empty()) {
            event_t event = event_queue.front();
            bool success = Particle.publish(event.eventName, event.data, event.ttl, event.flags);
            if (success)
              event_queue.pop();
            if (!event_queue.empty()) {
              if (!success)
                publishTimer.changePeriod(1000);
              publishTimer.start();
            }
        }
    };
};

#endif  // End of __PUBLISHQUEUE_H__ definition check
