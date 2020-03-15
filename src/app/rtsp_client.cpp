#include "rtsp_client.h"
#include "core/shared_queue.h"
#include "utils/image.h"
#include "utils/slogger.h"

namespace app {
RtspClient::RtspClient(int width, int height, FrameQueue &frameQueue,
                       std::string_view serverAddr, bool isNvidia)
    : frameQueue(frameQueue), shouldStop(true), ffmpegStream(NULL) {
  std::string format = "";
  std::string res = std::to_string(width) + "x" + std::to_string(height);
  if (serverAddr.find("rtsp://") != std::string::npos) {
    format = "-rtpflags skip_rtcp -allowed_media_types video -rtsp_transport "
             "udp -f rtsp ";
  }
  if (!isNvidia) {
    command = "ffmpeg -loglevel error -fflags 'nobuffer;flush_packets' -r 60 "
              "-f rawvideo "
              "-pix_fmt rgba -s " +
              res +
              " -i - "
              "-tune zerolatency -threads 1 -preset fast -y -pix_fmt yuv420p "
              "-vf vflip -vsync 1 -r 60 -c:v libx264 ";
  } else {
    command = "ffmpeg -loglevel error -fflags 'nobuffer;flush_packets' "
              "-hwaccel cuda -r 60 -f "
              "rawvideo -pix_fmt "
              "rgba -s " +
              res +
              " -i - "
              "-tune zerolatency -threads 1 -preset fast -y -pix_fmt bgr0 "
              "-vf vflip -vsync 1 -r 60 -c:v h264_nvenc ";
  }
  command += format;
  command += serverAddr;
}

RtspClient::~RtspClient() { stop(); }

bool RtspClient::start() {
  if (!shouldStop && !ffmpegStream) {
    SLOG("RTSP Client already running.");
    return false;
  }
  shouldStop = false;
  thread = std::thread([&shouldStop = shouldStop, &frameQueue = frameQueue,
                        &ffmpegStream = ffmpegStream, command = command]() {
    // start ffmpeg process
    ffmpegStream = popen(command.c_str(), "w");
    if (!ffmpegStream) {
      CSLOG("Failed to init ffmpeg.");
      shouldStop = true;
    } else {
      while (!shouldStop) {
        // feed fames to ffmpeg
        std::shared_ptr<Image> image;
        frameQueue.popGetFront(image, shouldStop);
        if (shouldStop)
          break;
        auto buffer = image->getBuffer();
        fwrite(buffer->data(), sizeof(uchar), buffer->getSize(), ffmpegStream);
      }
      // stop ffmpeg
      pclose(ffmpegStream);
    }
  });
  return true;
} // namespace app

void RtspClient::stop() {
  shouldStop = true;
  if (thread.joinable())
    thread.join();
}
} // namespace app
