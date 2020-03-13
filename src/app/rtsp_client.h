#pragma once

#include "core/shared_queue.h"
#include <thread>

class Image;
/**
 * @brief The RtspClient class
 * A simple RTSP client that sends frames to a RTSP server with ffmpeg
 * Runs on a separate thread.
 */
namespace app {
using FrameQueue = SharedQueue<std::shared_ptr<Image>>;
class RtspClient {
public:
  RtspClient(int width, int height, FrameQueue &frameQueue,
             std::string_view serverAddr, bool isNvidia);
  ~RtspClient();
  /**
   * @brief start
   * Start streaming frames to RTSP server
   * @return false if failed to start
   */
  bool start();

  /**
   * @brief stop
   * Stop stream frames to RTSP server
   */
  void stop();

private:
  FrameQueue &frameQueue; // refence to fo shared frame queues
  bool shouldStop;
  std::string command;
  FILE *ffmpegStream; // Input stream to feed ffmpeg
  std::thread thread;
};
} // namespace app
