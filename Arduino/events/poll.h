#pragma once
/**
 * poll.h
 * IO Polling
 */

#define DIST(x, y) (x < y ? x - y : y - x)

namespace Poll {

  namespace detail {
    template<class Poller>
    static inline void _pollMany() {
      Poller::poll();
    }

    template<class First, class Second, class ...Tail>
    static inline void _pollMany() {
      _pollMany<First>();
      _pollMany<Second, Tail...>();
    }
  }

  /**
   * Poll multiple things at once. Good if you wish to only use one slot in a
   * scheduler
   *
   * ex: Schedule 2 analog polls & 1 digital every 50 ms
   *
   *   scheduler.schedule(
   *     pollMany<
   *       PollAnanlogChange<Handler1, 2, 2>,
   *       PollAnanlogChange<Handler2, 3, 2>,
   *       PollDigitalChange<DigiHandler, 6>
   *     >, 50, 0);
   */
  template<class ...Pollers>
  void many() {
    detail::_pollMany<Pollers...>();
  }

  /**
   * Call `Op::run(voltage)` when analog signal on `Pin` changes by `Delta`
   */
  template<class Op, int Pin, int Delta>
  struct AnalogChange {
    static void poll() {
      static int16_t last;

      int v = analogRead(Pin);
      if (DIST(last, v) > Delta) {
        Op::run(v);
      }
    }
  };

  /**
   * Call `Op::run(state)` when `Pin` changes state
   */
  template<class Op, int Pin>
  struct DigitalChange {
    static void poll() {
      static uint16_t last;

      int s = digitalRead(Pin);
      if (s != last) {
        Op::run(s);
        last = s;
      }
    }
  };

  /**
   * Call `Op::run(nextByte)` when another byte is available on the default
   * Serial comm
   */
  template<class Op>
  struct SerialByte {
    static void poll() {
      int avail = Serial.available();
      for(int i = 0; i < avail; ++i) {
        Op::run(Serial.read());
      }
    }
  };

}