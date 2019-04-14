#pragma once

#define DIRECTINPUT_VERSION 0x0800

#include "Utils.h"
#include <dinput.h>
#include <functional>

enum PressState
{
    PRESS_DOWN = 0,
    PRESS_UP
};

enum MouseButton
{
    MOUSE_BUTTON_LEFT = 0,
    MOUSE_BUTTON_RIGHT,
    //MOUSE_BUTTON_MIDDLE
};

class InputManager
{
public:
    typedef std::function<void(unsigned char, PressState)> KeyboardCallBackType;
    typedef std::function<void(MouseButton, PressState, POINT &)> MouseButtonCallBackType;

    InputManager();
    ~InputManager();
    bool Initialize();
    void ProcessInput();
    void ReadKeyBoardInput();
    void ReadMouseInput();
    void ProcessKeyBoardInput();
    void ProcessMouseInput();
    void UnAcquire();
    void CleanUp();
    void CallMouseButtonCallBack(MouseButton button, PressState state);
    void CallKeyBoardCallBack(unsigned char key, PressState state);

    inline void RegisterMouseButtonCallBack(const MouseButtonCallBackType & callBackFunction)
    {
        MouseButtonCallBacks.push_back(callBackFunction);
    }

    inline void RegisterKeyBoardCallBack(const KeyboardCallBackType & callBackFunction)
    {
        keyboardCallBacks.push_back(callBackFunction);
    }

    inline bool IsKeyBoardCallBackCalled(unsigned char key)
    {
        return keyboardCallBackCalled[key];
    }

    inline bool IsKeyPressedDown(unsigned char key)
    {
        return keyboardState[key] & 0x80;
    }

    inline void GetCursorPosition(POINT & mousePosition)
    {
        GetCursorPos(&mousePosition);
        ScreenToClient(hWNDMain, &mousePosition);
    }

    inline bool IsMouseButtonDown(int iButton)
    {
        return (mouseCurrentState.rgbButtons[iButton] & 0x80) != 0;
    }

    inline bool IsMouseButtonCallBackCalled(MouseButton button)
    {
        return mouseButtonCallBackCalled[button];
    }

private:

    bool IsInitialized;
    bool IsMouseAcquired;
    bool IsKeyBoardAcquired;
    HWND hWNDMain;
    IDirectInputDevice8* DIKeyboard;
    IDirectInputDevice8* DIMouse;

    LPDIRECTINPUT8 DirectInput;
    bool mouseButtonCallBackCalled[2] = { 0 };
    std::vector<MouseButtonCallBackType> MouseButtonCallBacks;
    DIMOUSESTATE mouseCurrentState;
    DIMOUSESTATE mouseLastState = { 0 };
    BYTE keyboardState[256] = { 0 };
    bool keyboardCallBackCalled[256] = { 0 };
    std::vector<KeyboardCallBackType> keyboardCallBacks;
};

