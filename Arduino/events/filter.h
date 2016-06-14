#pragma once
// filter.h

#include "time.h"

namespace Filter {

  /**
   * Make sure that this event is only triggered every `LimitMs` milliseconds
   * Useful for de-bouncing buttons or something.
   */
  template<class Op, time_t LimitMs, class T>
  struct FrequencyLimiter {
    static void run(const T& arg) {
      static time_t lastRun = 0;

      if (lastRun + LimitMs < Time::now()) {
        lastRun = Time::now();
        Op::run(arg);
      }
    }
  };

  /**
   * Pass the difference between consecutive calls
   */
  template<class Op, class T>
  struct Diff {
    static void run(const T &next) {
      static T previous = 0;
      Op::run(next - previous);
      previous = next;
    }
  };

  template<class Op, class From, class To>
  struct Cast {
    static inline void run(const From &f) {  Op::run((To) f); }
  };

  template<class Op>
  struct Rate {
    static void run(const float &f) {
      static float previousVal = 0.0f;
      static time_t previousTime = 0;

      const time_t currentTime = Time::now();
      Op::run((f - previousVal) / (float) (currentTime - previousTime));
      previousTime = currentTime;
    }
  };
}
