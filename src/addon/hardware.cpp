#include "hardware.hpp"

const UINT Hardware::extendKeys[] = {
    VK_RCONTROL,
    VK_SNAPSHOT,
    VK_RMENU,
    VK_PAUSE,
    VK_HOME,
    VK_UP,
    VK_PRIOR,
    VK_LEFT,
    VK_RIGHT,
    VK_END,
    VK_DOWN,
    VK_NEXT,
    VK_INSERT,
    VK_DELETE,
    VK_LWIN,
    VK_RWIN,
    VK_APPS,
    VK_VOLUME_MUTE,
    VK_VOLUME_DOWN,
    VK_VOLUME_UP,
    VK_MEDIA_NEXT_TRACK,
    VK_MEDIA_PREV_TRACK,
    VK_MEDIA_STOP,
    VK_MEDIA_PLAY_PAUSE,
    VK_BROWSER_BACK,
    VK_BROWSER_FORWARD,
    VK_BROWSER_REFRESH,
    VK_BROWSER_STOP,
    VK_BROWSER_SEARCH,
    VK_BROWSER_FAVORITES,
    VK_BROWSER_HOME,
    VK_LAUNCH_MAIL,
};

const std::map<std::string, std::array<UINT, 2>> Hardware::buttonsDef = {
    {"left", {MOUSEEVENTF_LEFTUP, MOUSEEVENTF_LEFTDOWN}},
    {"right", {MOUSEEVENTF_RIGHTUP, MOUSEEVENTF_RIGHTDOWN}},
    {"middle", {MOUSEEVENTF_MIDDLEUP, MOUSEEVENTF_MIDDLEDOWN}},
    {"x1", {MOUSEEVENTF_XUP, MOUSEEVENTF_XDOWN}},
    {"x2", {MOUSEEVENTF_XUP, MOUSEEVENTF_XDOWN}}};

void Hardware::mousePosGetter(POINT *coords) {
  GetCursorPos(coords);

  ScreenToClient(hWnd, coords);
}

Napi::Value Hardware::getLastCoords(const Napi::CallbackInfo &info) {
  Napi::Object coords = Napi::Object::New(info.Env());

  POINT curr;

  GetCursorPos(&curr);

  ScreenToClient(hWnd, &curr);

  coords["x"] = curr.x;
  coords["y"] = curr.y;

  return coords;
};

void Hardware::mbToggler(std::string button, bool isButtonDown) {
  INPUT ip;

  ip.type = INPUT_MOUSE;

  ip.mi.dx = 0;
  ip.mi.dy = 0;
  ip.mi.mouseData = button == "x1"   ? XBUTTON1
                    : button == "x2" ? XBUTTON2
                                     : 0;
  ip.mi.dwExtraInfo = 0;
  ip.mi.time = 0;
  ip.mi.dwFlags = buttonsDef.at(button)[(int)isButtonDown];

  SendInput(1, &ip, sizeof(INPUT));
}

void Hardware::mover(POINT coords, bool isAbsolute) {
  INPUT ip;

  if (isAbsolute) {
    ClientToScreen(hWnd, &coords);

    if (coords.x < 0) {
      coords.x = 0;
    } else if (coords.x >= screenWidth) {
      coords.x = screenWidth - 1;
    }

    if (coords.y < 0) {
      coords.y = 0;
    } else if (coords.y >= screenHeigh) {
      coords.y = screenHeigh - 1;
    }
  }

  ip.type = INPUT_MOUSE;

  ip.mi.mouseData = 0;
  ip.mi.dwExtraInfo = 0;
  ip.mi.time = 0;
  ip.mi.dwFlags = MOUSEEVENTF_MOVE;

  if (isAbsolute) {
    ip.mi.dx = ((coords.x - GetSystemMetrics(SM_XVIRTUALSCREEN) + 1) << 16) / GetSystemMetrics(SM_CXVIRTUALSCREEN);
    ip.mi.dy = ((coords.y - GetSystemMetrics(SM_YVIRTUALSCREEN) + 1) << 16) / GetSystemMetrics(SM_CYVIRTUALSCREEN);
    ip.mi.dwFlags |= MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK;
  } else {
    ip.mi.dx = coords.x;
    ip.mi.dy = coords.y;
  }

  SendInput(1, &ip, sizeof(INPUT));
}

void Hardware::wheelScroller(int x) {
  INPUT ip;

  ip.type = INPUT_MOUSE;

  ip.mi.dx = 0;
  ip.mi.dy = 0;
  ip.mi.mouseData = x * WHEEL_DELTA;
  ip.mi.dwExtraInfo = 0;
  ip.mi.time = 0;
  ip.mi.dwFlags = MOUSEEVENTF_WHEEL;

  SendInput(1, &ip, sizeof(INPUT));
}

void Hardware::keyToggler(UINT key, bool isKeyDown) {
  INPUT ip;

  DWORD dwFlags = KEYEVENTF_SCANCODE;

  if (!isKeyDown) {
    dwFlags |= KEYEVENTF_KEYUP;
  }

  if (std::find(std::begin(extendKeys), std::end(extendKeys), key) != std::end(extendKeys)) {
    dwFlags |= KEYEVENTF_EXTENDEDKEY;
  }

  ip.type = INPUT_KEYBOARD;

  ip.ki.time = 0;
  ip.ki.wVk = 0;
  ip.ki.dwExtraInfo = 0;
  ip.ki.dwFlags = dwFlags;
  ip.ki.wScan = MapVirtualKeyA(key, MAPVK_VK_TO_VSC);

  SendInput(1, &ip, sizeof(INPUT));
}

void Hardware::charPrinter(std::wstring str) {
  std::vector<INPUT> vec;

  for (size_t i = 0; i < str.size(); i++) {
    INPUT input;

    input.type = INPUT_KEYBOARD;

    input.ki.time = 0;
    input.ki.wVk = 0;
    input.ki.dwExtraInfo = 0;
    input.ki.dwFlags = KEYEVENTF_UNICODE;
    input.ki.wScan = str.at(i);

    vec.push_back(input);

    INPUT nextInput = input;

    nextInput.ki.dwFlags |= KEYEVENTF_KEYUP;

    vec.push_back(nextInput);
  }

  SendInput(vec.size(), vec.data(), sizeof(INPUT));
}

Napi::FunctionReference Hardware::constructor;

Napi::Object Hardware::Init(Napi::Env env, Napi::Object exports) {
  Napi::HandleScope scope(env);

  Napi::Function func = DefineClass(
      env,
      "_Hardware",
      {
          InstanceMethod("getPos", &Hardware::getMousePos),
          InstanceMethod("toggleMb", &Hardware::toggleMb),
          InstanceMethod("move", &Hardware::move),
          InstanceMethod("scrollWheel", &Hardware::scrollWheel),
          InstanceMethod("toggleKey", &Hardware::toggleKey),
          InstanceMethod("printChar", &Hardware::printChar),
          InstanceMethod("isOpen", &Hardware::isOpen),
          InstanceMethod("isForeground", &Hardware::isForeground),
          InstanceMethod("setForeground", &Hardware::setForeground),
          InstanceMethod("capture", &Hardware::capture),
          InstanceMethod("getColor", &Hardware::getColor),
          InstanceMethod("kill", &Hardware::kill),
          InstanceMethod("close", &Hardware::close),
          InstanceMethod("refresh", &Hardware::refresh),
          InstanceMethod("setWorkwindow", &Hardware::setWorkwindow),
          InstanceMethod("getWorkwindow", &Hardware::getWorkwindow),
          InstanceMethod("setView", &Hardware::setWindowView),
          InstanceMethod("getView", &Hardware::getWindowView),
          InstanceAccessor("lastCoords", &Hardware::getLastCoords, NULL),
      });

  constructor = Napi::Persistent(func);

  constructor.SuppressDestruct();

  exports.Set("_Hardware", func);

  return exports;
}
