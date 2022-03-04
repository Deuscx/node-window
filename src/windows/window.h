#include <napi.h>

namespace windowwindows {
	void start(const Napi::CallbackInfo& info);
	void stop(const Napi::CallbackInfo& info);
	void focusWindow(const Napi::CallbackInfo& info);
	Napi::Object Init(Napi::Env env, Napi::Object exports);
}