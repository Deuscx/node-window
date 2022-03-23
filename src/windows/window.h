
#include <napi.h>

enum WindowEventType { Minimize = 1, Maxmize, Resize, Destroy };

struct ow_window_bounds {
  int32_t x;
  int32_t y;
  uint32_t width;
  uint32_t height;
};
struct ow_event {
  enum WindowEventType type;
  union {
    struct ow_window_bounds position;
  };
};
namespace windowwindows {
void start(const Napi::CallbackInfo &info);
void stop(const Napi::CallbackInfo &info);
void focusWindow(const Napi::CallbackInfo &info);
Napi::Object Init(Napi::Env env, Napi::Object exports);
}; // namespace windowwindows
