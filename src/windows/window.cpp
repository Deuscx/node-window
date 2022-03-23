
// #include <iostream>
#include <Windows.h>
#include <thread>

#include "window.h"
using namespace Napi;

std::thread nativeThread;
ThreadSafeFunction tsfn;
const UINT STOP_MESSAGE = WM_USER + 1;

void releaseThreadFunction();
static VOID CALLBACK hook_proc(HWINEVENTHOOK, DWORD, HWND, LONG, LONG, DWORD,
                               DWORD);
Napi::Object event_data(Napi::Env env, struct ow_event *event);
static auto callback = [](Napi::Env env, Function jsCallback,
                          struct ow_event *value) {
  // Transform native data into JS data, passing it to the provided
  // `jsCallback` -- the TSFN's JavaScript function.
  jsCallback.Call({event_data(env, value)});
  // We're finished with the data.
  // value.clear();
  // delete value;
};

/**
 *  获取窗口大小
 * */
static bool get_content_bounds(HWND hwnd, struct ow_window_bounds *bounds) {
  RECT rect;
  if (GetClientRect(hwnd, &rect) == FALSE) {
    return false;
  }
  POINT ptClientUL = {rect.left, rect.top};
  if (ClientToScreen(hwnd, &ptClientUL) == FALSE) {
    return false;
  }

  bounds->x = ptClientUL.x;
  bounds->y = ptClientUL.y;
  bounds->width = rect.right;
  bounds->height = rect.bottom;
  return true;
}
static HWND target_hwnd = NULL;
void windowwindows::start(const CallbackInfo &info) {
  Env env = info.Env();
  releaseThreadFunction();
  HWND hwnd = static_cast<HWND>(
      *reinterpret_cast<void **>(info[0].As<Napi::Buffer<void *>>().Data()));
  target_hwnd = hwnd;
  // Create a ThreadSafeFunction
  tsfn = ThreadSafeFunction::New(
      env,
      info[1].As<Function>(), // JavaScript function called asynchronously
      "shareScreen Events",   // Name
      0,                      // Unlimited queue
      1,                      // Only one thread will use this initially
      [](Napi::Env) {         // Finalizer used to clean threads up
        DWORD threadId = GetThreadId(nativeThread.native_handle());
        if (threadId == 0)
          return;
        PostThreadMessageA(threadId, STOP_MESSAGE, NULL, NULL);

        if (nativeThread.joinable()) {
          nativeThread.join();
        }
      });

  nativeThread = std::thread([hwnd]() {
    SetWinEventHook(EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZESTART,
                    NULL, hook_proc, 0, 0, WINEVENT_OUTOFCONTEXT);
    SetWinEventHook(EVENT_SYSTEM_MINIMIZEEND, EVENT_SYSTEM_MINIMIZEEND, NULL,
                    hook_proc, 0, 0, WINEVENT_OUTOFCONTEXT);
    SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE,
                    NULL, hook_proc, 0, 0, WINEVENT_OUTOFCONTEXT);

    DWORD pid;
    DWORD threadId = GetWindowThreadProcessId(target_hwnd, &pid);
    SetWinEventHook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_DESTROY, NULL, hook_proc,
                    0, threadId, WINEVENT_OUTOFCONTEXT);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
      if (msg.message == STOP_MESSAGE) {
        PostQuitMessage(0);
      } else {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    }
  }

  );
}

static VOID CALLBACK hook_proc(HWINEVENTHOOK hWinEventHook, DWORD event,
                               HWND hwnd, LONG idObject, LONG idChild,
                               DWORD idEventThread, DWORD dwmsEventTime) {

  if (event == EVENT_SYSTEM_MINIMIZESTART) {
    if (target_hwnd != NULL && hwnd == target_hwnd) {
      struct ow_event e = {Minimize};
      tsfn.BlockingCall(&e, callback);
    }
  } else if (event == EVENT_SYSTEM_MINIMIZEEND) {
    if (target_hwnd != NULL && hwnd == target_hwnd) {
      struct ow_event e = {Maxmize};
      tsfn.BlockingCall(&e, callback);
    }
  } else if (event == EVENT_OBJECT_LOCATIONCHANGE) {
    if (target_hwnd != NULL && hwnd == target_hwnd) {
      struct ow_event e = {Resize};
      ow_window_bounds bounds;
      get_content_bounds(hwnd, &bounds);
      e.position = bounds;
      tsfn.BlockingCall(&e, callback);
    }
  } else if (event == EVENT_OBJECT_DESTROY) {
    if (target_hwnd != NULL && hwnd == target_hwnd) {
      struct ow_event e = {Destroy};
      tsfn.BlockingCall(&e, callback);
      releaseThreadFunction();
    }
  }
}

void windowwindows::stop(const CallbackInfo &info) { releaseThreadFunction(); }

void windowwindows::focusWindow(const CallbackInfo &info) {
  Env env = info.Env();
  // const int64_t hwnd = info[0].As<Number>().Int64Value();
  HWND hwnd = static_cast<HWND>(
      *reinterpret_cast<void **>(info[0].As<Napi::Buffer<void *>>().Data()));
  if (IsIconic(hwnd)) {
    ShowWindow(hwnd, SW_RESTORE);
  } else {
    SetForegroundWindow(hwnd);
  }
}

void releaseThreadFunction() {
  if (tsfn) {
    napi_status status = tsfn.Release();
    tsfn = NULL;
  }
}

/**
 * 转为JS对象
 * */
Napi::Object event_data(Napi::Env env, struct ow_event *event) {
  Napi::Object obj = Napi::Object::New(env);
  obj["type"] = Napi::Number::New(env, event->type);

  if (event->type == Resize) {
    Napi::Object bounding = Napi::Object::New(env);
    bounding["x"] = event->position.x;
    bounding["y"] = event->position.y;
    bounding["width"] = event->position.width;
    bounding["height"] = event->position.height;
    obj["bounding"] = bounding;
  }
  return obj;
}
Object windowwindows::Init(Env env, Object exports) {
  exports["start"] = Function::New(env, start);
  exports["stop"] = Function::New(env, stop);
  exports["focusWindow"] = Function::New(env, focusWindow);
  return exports;
}