// #pragma once

// #ifndef FRAME_SIZE
//   #define FRAME_SIZE 64
// #endif

// /**
//  * comm.h
//  * Communication helpers.
//  * Notes:
//  *  - Oversized frames are not handled. They will just mess things up.
//  */
// namespace Comm {

//   struct Frame {
//     uint8_t bytes[FRAME_SIZE]; // Don't make frames larger than this. Just don't.
//     uint8_t size;
//   };

//   /**
//    * Frame bytes where the first byte is interpreted as the length of the frame
//    */
//   template<class FrameHandler>
//   struct SizedFrameDecoder {
//     static void run(uint8_t byte) {
//       static uint8_t cur = 0;
//       static Frame frame { 0, 0 };

//       if (frame.size == 0) {
//         frame.size = byte;
//       } else {
//         frame.bytes[cur] = byte;
//         ++cur;

//         if (cur == frame.size) {
//           FrameHandler::run(frame);
//           frame.size = 0;
//           cur = 0;
//         }
//       }
//     }
//   };

//   *
//    * Frame bytes that are always a fixed size

//   template<int FrameSize, class FrameHandler>
//   struct FixedFrameDecoder {

//     void run(uint8_t byte) {
//       static uint8_t cur = 0;
//       static Frame frame {0, FrameSize};

//       if (cur == FrameSize) {
//         FrameHandler::run(frame);
//         cur = 0;
//       } else {
//         frame.bytes[cur] = byte;
//         ++cur;
//       }
//     }
//   };

//   template<uint8_t Delim, class FrameHandler>
//   struct DelimFrameDecoder {

//     static void run(uint8_t byte) {
//       static uint8_t cur = 0;
//       static Frame frame {0, 0};

//       if (byte == Delim) {
//         frame.size = cur;
//         FrameHandler::run(frame);
//         cur = 0;
//       } else {
//         frame.bytes[cur] = byte;
//         ++cur;
//       }
//     }
//   };

//   /**
//    * Dump frames over a setup serial connection
//    */
//   struct SerialDumpFrameHandler {
//     void run(const Frame &f) {
//       // TODO Serial.
//     }
//   };
// }