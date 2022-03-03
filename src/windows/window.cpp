
// #include <iostream>
#include <thread>
#include <Windows.h>

#include "window.h"
using namespace Napi;

std::thread nativeThread;
ThreadSafeFunction tsfn;
const UINT STOP_MESSAGE = WM_USER + 1;

static VOID CALLBACK hook_proc(HWINEVENTHOOK, DWORD, HWND, LONG, LONG, DWORD, DWORD);

static auto callback = [](Napi::Env env, Function jsCallback, std::string value)
{
    // Transform native data into JS data, passing it to the provided
    // `jsCallback` -- the TSFN's JavaScript function.
    jsCallback.Call({String::New(env, value)});
    // We're finished with the data.
    value.clear();
};

static HWND target_hwnd = NULL;
void windowwindows::start(const CallbackInfo &info)
{
    Env env = info.Env();
    const int64_t hwnd = info[0].As<Number>().Int64Value();
    target_hwnd = (HWND)hwnd;
    // Create a ThreadSafeFunction
    tsfn = ThreadSafeFunction::New(
        env,
        info[1].As<Function>(), // JavaScript function called asynchronously
        "Resource Name",        // Name
        0,                      // Unlimited queue
        1,                      // Only one thread will use this initially
        [](Napi::Env) {         // Finalizer used to clean threads up
            DWORD threadId = GetThreadId(nativeThread.native_handle());
            if (threadId == 0)
                return;
            PostThreadMessageA(threadId, STOP_MESSAGE, NULL, NULL);

            if (nativeThread.joinable())
            {
                nativeThread.join();
            }

        });

    nativeThread = std::thread([hwnd]()
                               {
    SetWinEventHook(
    EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZESTART,
    NULL, hook_proc, 0, 0, WINEVENT_OUTOFCONTEXT);
    SetWinEventHook(
    EVENT_SYSTEM_MINIMIZEEND, EVENT_SYSTEM_MINIMIZEEND,
    NULL, hook_proc, 0, 0, WINEVENT_OUTOFCONTEXT);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == STOP_MESSAGE) {
                PostQuitMessage(0);
            } else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            }
    } }

    );
}

static VOID CALLBACK hook_proc(
    HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild,
    DWORD idEventThread, DWORD dwmsEventTime)
{

    if (event == EVENT_SYSTEM_MINIMIZESTART)
    {
        if (target_hwnd != NULL && hwnd == target_hwnd)
        {
            tsfn.BlockingCall("minimize", callback);
        }
    }
    else if (event == EVENT_SYSTEM_MINIMIZEEND)
    {

        if (target_hwnd != NULL && hwnd == target_hwnd)
        {
            tsfn.BlockingCall("maximize", callback);
        }
    }
}

void windowwindows::stop(const CallbackInfo &info)
{
    if (tsfn)
    {
        napi_status status = tsfn.Release();
        tsfn = NULL;
    }
}

Object windowwindows::Init(Env env, Object exports)
{
    exports["start"] = Function::New(env, start);
    exports["stop"] = Function::New(env, stop);
    return exports;
}