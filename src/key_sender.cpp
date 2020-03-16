
#include <windows.h>
#include "hardware.hpp"
#include "virtual.hpp"

void sleep(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    if (info.Length() != 1 || !info[0].IsNumber())
        Napi::Error::New(env, "Expected 1 argument: Number")
            .ThrowAsJavaScriptException();
    std::this_thread::sleep_for(std::chrono::milliseconds(info[0].As<Napi::Number>().Int32Value()));
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    Hardware::Init(env, exports);
    Virtual::Init(env, exports);
    exports["_sleep"] = Napi::Function::New(env, sleep);
    exports["_getWindow"] = Napi::Function::New(env, getWindow);
    exports["_getWindowChild"] = Napi::Function::New(env, getWindowChild);
    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)