#include <napi.h>
#include <thread>

namespace windowmacos {
    void start(const Napi::CallbackInfo& info);
    void stop(const Napi::CallbackInfo& info);
    void focusWindow(const Napi::CallbackInfo& info);
    Napi::Object Init(Napi::Env env, Napi::Object exports);
}