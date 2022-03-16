#import <AppKit/AppKit.h>
#import <ApplicationServices/ApplicationServices.h>
#include <napi.h>

using namespace Napi;

// Data structure representing our thread-safe function context.
struct TsfnContext {
  TsfnContext(Napi::Env env){};
  std::thread nativeThread;
};
ThreadSafeFunction tsfn;

void ReleaseTSFN();
// The thread-safe function finalizer callback. This callback executes
// at destruction of thread-safe function, taking as arguments the finalizer
// data and threadsafe-function context.
void FinalizerCallback(Napi::Env env, void *finalizeData, TsfnContext *context);
static auto target_handle = NULL;

void start(const CallbackInfo &info) {
  Env env = info.Env();
  // Stop if already running
  ReleaseTSFN();

  // Create a new thread-safe function context.
  TsfnContext *context = new TsfnContext(env);

  const auto handle = info[0].As<Number>().Int64Value();
  target_handle = handle;
  // Create a ThreadSafeFunction
  tsfn = Napi::ThreadSafeFunction::New(
      env,
      info[1].As<Napi::Function>(), // JavaScript function called asynchronously
      "shareScreen Events",         // Name
      0,                            // Unlimited queue
      1,                            // Only one thread will use this initially
      contextData,                  // Context that can be accessed by Finalizer
      FinalizerCallback,            // Finalizer used to clean threads up
      (void *)nullptr               // Finalizer data
  );

  // Create a new thread-safe function.
  /**
   * Initializes listeners for the frontmost window, and then starts the event
   * loop.
   */
  context->nativeThread = std::thread([handle]() {
    // Do some work.
    std::cout << "Hello from thread " << handle << std::endl;
  });
}

void focusWindow(const CallbackInfo &info) {
  Env env = info.Env();
  const auto handle = info[0].As<Number>().Int64Value();
  NSView *targetView = *(NSView * __weak *)handle;
  NSWindow *targetWindow = [targetView window];
  
  [NSApp activateIgnoringOtherApps:YES];
  [targetWindow makeKeyAndOrderFront:nil];
}

void Stop(const Napi::CallbackInfo &info) { ReleaseTSFN(); }

void ReleaseTSFN() {
  if (tsfn) {
    // Release the TSFN
    napi_status status = tsfn.Release();
    if (status != napi_ok) {
      std::cerr << "Failed to release the TSFN!" << std::endl;
    }
    tsfn = NULL;
  }
}
void FinalizerCallback(Napi::Env env, void *finalizeData,
                       TsfnContext *context) {
  if (context->nativeThread.joinable()) {
    context->nativeThread.join();
  } else {
    std::cerr << "Failed to join nativeThread!" << std::endl;
  }

  delete context;
}

Object windowwindows::Init(Env env, Object exports) {
  exports["start"] = Function::New(env, start);
  exports["stop"] = Function::New(env, stop);
  exports["focusWindow"] = Function::New(env, focusWindow);
  return exports;
}
