#import <AppKit/AppKit.h>
#import <ApplicationServices/ApplicationServices.h>

#include "window.h"


void ReleaseTSFN();
static auto target_handle = NULL;
std::thread nativeThread;
Napi::ThreadSafeFunction tsfn;

void windowmacos::start(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  // // Stop if already running
  // ReleaseTSFN();

  // const auto handle = info[0].As<Napi::Number>().Int64Value();
  // target_handle = handle;
  // // Create a ThreadSafeFunction
  // tsfn = Napi::ThreadSafeFunction::New(
  //     env,
  //     info[1].As<Napi::Function>(), // JavaScript function called asynchronously
  //     "shareScreen Events",         // Name
  //     0,                            // Unlimited queue
  //     1,                            // Only one thread will use this initially
  //     [](Napi::Env) {         // Finalizer used to clean threads up
  //       if (nativeThread.joinable()) {
  //         nativeThread.join();
  //       }
  //     });


}

void windowmacos::focusWindow(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  Napi::Buffer<void*> handle = info[0].As<Napi::Buffer<void*>>();
  NSView* targetView = static_cast<NSView*>(*reinterpret_cast<void **>(handle.Data()));
  NSWindow* targetWindow = [targetView window];
  
  [NSApp activateIgnoringOtherApps:YES];
  // NSLog(@"title %@", [targetWindow title]);
  // [targetWindow deminiaturize:targetWindow];
  // [targetWindow setIsVisisble:true];
  // [targetWindow setIsVisible:true];
  [targetWindow makeKeyAndOrderFront:nil];
  // AXUIELementSetAttribute(targetWindow, kAXMainAttribute, kCFBooleanTrue);
}

void windowmacos::stop(const Napi::CallbackInfo &info) { ReleaseTSFN(); }

void ReleaseTSFN() {
  if (tsfn) {
    // Release the TSFN
    napi_status status = tsfn.Release();
    if (status != napi_ok) {
    }
    tsfn = NULL;
  }
}

Napi::Object windowmacos::Init(Napi::Env env, Napi::Object exports) {
  exports["start"] = Napi::Function::New(env, start);
  exports["stop"] = Napi::Function::New(env, stop);
  exports["focusWindow"] = Napi::Function::New(env, focusWindow);
  return exports;
}