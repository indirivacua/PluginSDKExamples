#include "InputManager.h"

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

InputManager::InputManager()
{
    IsInitialized = false;
    IsMouseAcquired = false;
    IsKeyBoardAcquired = false;
    DirectInput = nullptr;
}

InputManager::~InputManager()
{
    CleanUp();
}

bool InputManager::Initialize()
{
    hWNDMain = FindMainWindow();
    if (!hWNDMain)
    {
        return false;
    }

    HINSTANCE hInstanceWindow = (HINSTANCE)GetWindowLong(hWNDMain, GWL_HINSTANCE);
    if (hInstanceWindow)
    {
        HRESULT hResult = DirectInput8Create(hInstanceWindow, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&DirectInput, NULL);
        if (SUCCEEDED(hResult))
        {
            hResult = DirectInput->CreateDevice(GUID_SysKeyboard, &DIKeyboard, NULL);
            if (SUCCEEDED(hResult))
            {
                hResult = DirectInput->CreateDevice(GUID_SysMouse, &DIMouse, NULL);
                if (SUCCEEDED(hResult))
                {
                    hResult = DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
                    if (SUCCEEDED(hResult))
                    {
                        hResult = DIKeyboard->SetCooperativeLevel(hWNDMain, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
                        if (SUCCEEDED(hResult))
                        {
                            hResult = DIMouse->SetDataFormat(&c_dfDIMouse);
                            if (SUCCEEDED(hResult))
                            {
                                hResult = DIMouse->SetCooperativeLevel(hWNDMain, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);
                                if (SUCCEEDED(hResult))
                                {
                                    IsInitialized = true;
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

void InputManager::ProcessInput()
{
    if (!IsInitialized)
    {
        return;
    }

    ReadKeyBoardInput();
    ProcessKeyBoardInput();
    ReadMouseInput();
    ProcessMouseInput();
}

void InputManager::ReadKeyBoardInput()
{
    HRESULT result = DIKeyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);
    if (FAILED(result))
    {
        if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
        {
            IsKeyBoardAcquired = SUCCEEDED(DIKeyboard->Acquire());
        }
    }
}

void InputManager::ReadMouseInput()
{
    HRESULT result = DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrentState);
    if (FAILED(result))
    {
        if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
        {
            IsMouseAcquired = SUCCEEDED(DIMouse->Acquire());
        }
    }
}

void InputManager::ProcessKeyBoardInput()
{
    for (int key = 0; key < sizeof(keyboardState); key++)
    {
        if (IsKeyPressedDown(key))
        {
            if (!IsKeyBoardCallBackCalled(key))
            {
                CallKeyBoardCallBack(key, PRESS_DOWN);
            }

        }
        else
        {
            if (IsKeyBoardCallBackCalled(key))
            {
                CallKeyBoardCallBack(key, PRESS_UP);
            }
        }
    }
}

void InputManager::ProcessMouseInput()
{
    for (int i = 0; i < 2; i++)
    {
        MouseButton button = static_cast<MouseButton>(i);
        if (IsMouseButtonDown(button))
        {
            if (!IsMouseButtonCallBackCalled(button))
            {
                CallMouseButtonCallBack(button, PRESS_DOWN);
            }
        }
        else
        {
            if (IsMouseButtonCallBackCalled(button))
            {
                CallMouseButtonCallBack(button, PRESS_UP);
            }
        }
    }

    if ((mouseCurrentState.lX != mouseLastState.lX) || (mouseCurrentState.lY != mouseLastState.lY))
    {

        // Ok, mouse is moving. We can call an event here (OnMouseMoving)
        // TODO
        mouseLastState = mouseCurrentState;
    }
}

void InputManager::UnAcquire()
{
    if (IsKeyBoardAcquired)
    {
        DIKeyboard->Unacquire();
        IsKeyBoardAcquired = false;
    }
    if (IsMouseAcquired)
    {
        DIMouse->Unacquire();
        IsMouseAcquired = false;
    }
}

void InputManager::CleanUp()
{
    UnAcquire();
    if (DirectInput)
    {
        DirectInput->Release();
    }
}

void InputManager::CallMouseButtonCallBack(MouseButton button, PressState state)
{
    mouseButtonCallBackCalled[button] = state == PRESS_DOWN;
    for (auto& callBackFunction : MouseButtonCallBacks)
    {
        POINT mousePosition;
        GetCursorPosition(mousePosition);
        callBackFunction(button, state, mousePosition);
    }
}

void InputManager::CallKeyBoardCallBack(unsigned char key, PressState state)
{
    keyboardCallBackCalled[key] = state == PRESS_DOWN;
    for (auto& callBackFunction : keyboardCallBacks)
    {
        callBackFunction(key, state);
    }
}