#include <napi.h>

#ifdef _WIN32
#include "windows/window.h"
#endif

#ifdef __APPLE__
#include "macos/windowmacos.h"
#endif


Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
	#ifdef _WIN32
		return windowwindows::Init(env, exports);
	#elif __APPLE__
		return windowmacos::Init(env, exports);
	#endif
}

NODE_API_MODULE(testaddon, InitAll)
