#include "virtual.hpp"

#include "helper.hpp"

const std::map<std::string, UINT> Virtual::wParams = {{"left", MK_LBUTTON},
                                                      {"right", MK_RBUTTON},
                                                      {"middle", MK_MBUTTON},
                                                      {"x1", MK_XBUTTON1},
                                                      {"x2", MK_XBUTTON2}};

void Virtual::mousePosGetter(POINT *coords) {
  GetCursorPos(coords);
  ScreenToClient(hWnd, coords);
}

Napi::Value Virtual::getLastCoords(const Napi::CallbackInfo &info) {
  Napi::Object coords = Napi::Object::New(info.Env());

  coords["x"] = lastCoords.x;
  coords["y"] = lastCoords.y;

  return coords;
};

void Virtual::mbToggler(std::string button, bool isButtonDown) {
  PostMessageA(hWnd, Helper::mouseEvents.at(button)[(int)isButtonDown], isButtonDown ? wParams.at(button) : 0, MAKELPARAM(lastCoords.x, lastCoords.y));
}

void Virtual::mover(POINT coords, bool isAbsolute) {
  PostMessageA(hWnd, WM_MOUSEMOVE, 1, isAbsolute ? MAKELPARAM(coords.x, coords.y) : MAKELPARAM(lastCoords.x + coords.x, lastCoords.y + coords.y));

  if (isAbsolute) {
    lastCoords.x = coords.x;
    lastCoords.y = coords.y;
  } else {
    lastCoords.x += coords.x;
    lastCoords.y += coords.y;
  }
}

void Virtual::wheelScroller(int x) {
  SendMessageA(hWnd, WM_MOUSEWHEEL, MAKEWPARAM(0, x * WHEEL_DELTA), MAKELPARAM(lastCoords.x, lastCoords.y));
}

void Virtual::keyToggler(UINT key, bool isKeyDown) {
  PostMessageA(hWnd, isKeyDown ? WM_KEYDOWN : WM_KEYUP, key, 0 | (key << 16) | (0 << 24) | (0 << 29) | ((UINT)!isKeyDown << 30) | ((UINT)!isKeyDown << 31));
}

void Virtual::charPrinter(std::wstring str) {
  for (size_t i = 0; i < str.size(); i++) {
    SendMessageW(hWnd, WM_CHAR, str.at(i), 0);
  }
}

Napi::FunctionReference Virtual::constructor;

Napi::Object Virtual::Init(Napi::Env env, Napi::Object exports) {
  Napi::HandleScope scope(env);

  Napi::Function func = DefineClass(
      env,
      "_Virtual",
      {
          InstanceMethod("getPos", &Virtual::getMousePos),
          InstanceMethod("toggleMb", &Virtual::toggleMb),
          InstanceMethod("move", &Virtual::move),
          InstanceMethod("scrollWheel", &Virtual::scrollWheel),
          InstanceMethod("toggleKey", &Virtual::toggleKey),
          InstanceMethod("printChar", &Virtual::printChar),
          InstanceMethod("setForeground", &Virtual::setForeground),
          InstanceMethod("isForeground", &Virtual::isForeground),
          InstanceMethod("isOpen", &Virtual::isOpen),
          InstanceMethod("capture", &Virtual::capture),
          InstanceMethod("getColor", &Virtual::getColor),
          InstanceMethod("kill", &Virtual::kill),
          InstanceMethod("close", &Virtual::close),
          InstanceMethod("refresh", &Virtual::refresh),
          InstanceMethod("setWorkwindow", &Virtual::setWorkwindow),
          InstanceMethod("getWorkwindow", &Virtual::getWorkwindow),
          InstanceMethod("setView", &Virtual::setWindowView),
          InstanceMethod("getView", &Virtual::getWindowView),
          InstanceAccessor("lastCoords", &Virtual::getLastCoords, NULL),
      });

  constructor = Napi::Persistent(func);

  constructor.SuppressDestruct();

  exports.Set("_Virtual", func);

  return exports;
}
