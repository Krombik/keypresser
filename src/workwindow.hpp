#ifndef WORKWINDOW_H
#define WORKWINDOW_H

#include <napi.h>
#include <winuser.h>
#include <windows.h>
#include <iostream>

Napi::Value getWindow(const Napi::CallbackInfo &info);
Napi::Value getWindowChild(const Napi::CallbackInfo &info);

class Workwindow
{
public:
    Napi::Value isForeground(const Napi::CallbackInfo &info);
    Napi::Value isOpen(const Napi::CallbackInfo &info);
    void Workwindow::setForeground(const Napi::CallbackInfo &info);
    void sleep(const Napi::CallbackInfo &info);

protected:
    HWND hWnd = NULL;
    void setWorkwindow(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getWorkwindow(const Napi::CallbackInfo &info);
};

#endif