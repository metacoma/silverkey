#ifndef FOCUSCONTROLLER_WIN_H
#define FOCUSCONTROLLER_WIN_H

#include <Windows.h>

class FocusControllerWin
{
public:
    FocusControllerWin();
    ~FocusControllerWin();
    void switchFocusToOld();
    void savePrevActive();
    void detachThread();

private:
    HWND m_lastHwnd = nullptr;
    DWORD m_threadId = 0;
};

#endif // FOCUSCONTROLLER_WIN_H
