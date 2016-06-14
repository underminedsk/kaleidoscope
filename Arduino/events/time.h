// time.h

#include <arduino.h>

typedef uint32_t time_t;
typedef uint32_t duration_t;

namespace Time {

  // Special signaling times
  static const time_t Never    = 0xffffffffL - 0L;
  static const time_t Always   = 0xffffffffL - 1L;
  static const time_t MaxTime  = 0xffffffffL - 2L;
  static const time_t MinTime  = 0L;

  // static const time_t HalfTime = MaxTime / 2;

  namespace detail {
    static time_t approxNow;
  }

  static inline time_t fromNow(time_t howLongMs) {
    return detail::approxNow + howLongMs;
  }

  static inline time_t updatedNow() {
    detail::approxNow = millis();
    return detail::approxNow;
  }

  static inline time_t now() {
    return detail::approxNow;
  }

  static inline bool isBefore(time_t a, time_t b) {
    return
      a <= MaxTime &&
      b <= MaxTime &&
      a < b;
  }

  static inline bool isAfter(time_t a, time_t b) {
    return
      a <= MaxTime &&
      b <= MaxTime &&
      a > b;
  }

  /**** DO I WANT THIS STUFF ??? ***********************************************
   **** Maybe just don't care about rollover?
  // The shortest distance between two times with
  static time_t dist(time_t a, time_t b) {
    if (a == Never || b == Never) return Never;
    if (a == Always || b == Always) return MinTime;

    const time_t d = b > a ? b - a : a - b;
    return d < HalfTime ? d : d - HalfTime;
  }

  // Return true if time rolled over
  static bool add(time_t &time, time_t duration) {
    if (time > MaxTime || duration > MaxTime) return false;

    const time_t orig = time;
    time += duration;
    if (time > MaxTime) time = MaxTime; // Round up
    return time < orig;
  }

  // Return true if time rolled under
  static bool sub(time_t &time, time_t duration) {
    if (time > MaxTime || duration > MaxTime) return false;

    const time_t orig = time;
    time -= duration;
    if (time > MaxTime) time = MinTime; // Round down
    return time > orig;
  }


  // Check if (`a` + `duration`) is before `b`, accounting for rollover
  static bool isBefore(time_t a, time_t duration, time_t b) {
    if (a > MaxTime || b > MaxTime) return false;
    if (add(a, duration)) {
      dist(a, b) > duration;
    } else {
      return a < b;
    }
  }


  // Check if (`a` + `duration`) is after `b`, accounting for rollover
  static bool isAfter(time_t a, time_t duration, time_t b) {
    if (a > MaxTime || b > MaxTime) return false;
    if (add(a, duration)) {
      dist(a, b) < duration;
    } else {
      return a > b;
    }
  }
  *****************************************************************************/
}
