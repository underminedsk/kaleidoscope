#pragma once
/**
 * events.h
 * Tools for running async code on the Arduino platform
 */

#include <arduino.h>
#include "time.h"

typedef void (*callback)();

namespace detail {

  template<class Head>
  static inline void runPoll() {
    Head::poll();
  }

  template<class First, class Second, class ...Tail>
  static inline void runPoll() {
    runPoll<First>();
    runPoll<Second, Tail...>();
  }

  static void wait(time_t howLongMs) {
    if (howLongMs > 5) delay(howLongMs);
    // TODO: Low power if this is a long time?
  }
}

// sizeof(Event) = 12?
struct Event {
  callback f;
  duration_t period;
  time_t nextRun;
};

/**
 * Manage scheduling and running of callbacks
 */
template<int QueueSize>
class Scheduler {

  Event q[QueueSize];
  uint8_t eventCount;

  // Find the event for the callback if it exists, or find an empty slot
  Event * eventForCallback(callback f) {
    for(uint8_t i = 0; i < QueueSize; ++i) if (q[i].f == f) return &q[i];
    for(uint8_t i = 0; i < QueueSize; ++i) if (q[i].f == nullptr) return &q[i];
    return nullptr;
  }

  bool set(callback f, time_t newPeriod, time_t nextRun) {
    if (Event * e = eventForCallback(f)) {
      e->f = f;
      e->period = newPeriod;
      e->nextRun = nextRun;
      ++eventCount;
      return true;
    } else {
      return false;
    }
  }

  bool del(callback f) {
    if (Event * e = eventForCallback(f)) {
      e->f = nullptr;
      --eventCount;
      return true;
    }
    return false;
  }

  inline void runItem(Event * e) {
    e->f();
    if (e->period == Time::Never) {
      e->f = nullptr;
      --eventCount;
    } else {
      e->nextRun += e->period;
    }
  }

  public:

  Scheduler() : eventCount(0) {}

  /**
   * Schedule an event to be run every `period` milliseconds starting after
   * `delayMs` milliseconds. If `period` is Time::Never, this event will not
   * repeat. If `delayMs` is zero, this event will run ASAP. This my also be
   * used to modify existing events.
   * Returns false if the q is full.
   */
  bool run(callback f, time_t period, time_t delayMs) {
    return set(f, period, Time::fromNow(delayMs));
  }

  /**
   * Remove an event from the scheduler by callback. Returns false if the
   * event was not scheduled.
   */
  bool deschedule(callback f) {
    return del(f);
  }

  /**
   * Clear the event q
   */
  void clear() {
    for (uint8_t i = 0; i < QueueSize; ++i) q[i].f = nullptr;
    eventCount = 0;
  }

  /**
   * See if any events are runnable, and run.
   */
  void tick() {
    Time::updatedNow();
    for(uint8_t i = 0; i < QueueSize; ++i)
      if (q[i].f != nullptr && q[i].nextRun <= Time::now())
        runItem(&q[i]);
  }

  /**
   * Fetch the next runnable event if any.
   */
  Event * nextEvent() {
    time_t nextRunTime = Time::Never;
    Event * ret = nullptr;

    for(uint8_t i = 0; i < QueueSize; ++i) {
      if (q[i].f != nullptr && q[i].nextRun < nextRunTime) {
        nextRunTime = q[i].nextRun;
        ret = &q[i];
      }
    }

    return ret;
  }

  /**
   * Fetch the absolute time of the next run of a given callback
   */
  time_t timeOfNextRun(callback f) {
    Event * e = eventForCallback(f);
    if(e && e->f != nullptr) {
      return e->nextRun;
    } else {
      return Time::Never;
    }
  }

  /**
   * Fetch the time of the next event scheduled to run
   */
  time_t timeOfNextEvent() {
    if (Event * e = nextEvent()) {
      return e->nextRun;
    } else {
      return Time::Never;
    }
  }

  uint8_t numEvents() {
    return eventCount;
  }

  /**
   * Continuously check for new events as fast as possible. This is best for
   * events that should run quickly.
   */
  void tickForever() {
    uint8_t i = 0;
    while(1) {
      if (q[i].f != nullptr && q[i].nextRun <= Time::updatedNow())
        runItem(&q[i]);
      i = (i + 1) % QueueSize;
    }
  }

  /**
   * Continuously run by finding the next scheduled event and sleeping until
   * it is scheduled to run. This is best for events that don't run frequently
   */
  void run() {
    time_t time;
    while(1) {
      time = timeOfNextEvent();
      if (time != Time::Never) {

      if(time > Time::updatedNow())
        detail::wait(time - Time::now());

      Time::updatedNow();

      for(uint8_t i = 0; i < QueueSize; ++i)
        if (q[i].f != nullptr && q[i].nextRun <= Time::now())
          runItem(&q[i]);

      } else {
        // allow an interrupt to schedule a new event.
        detail::wait(500);
      }
    }
  }
};
