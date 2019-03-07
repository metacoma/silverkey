#include "focuscontroller_win.h"

#include "focuscontroller.h"

#include <QDebug>

#include <Windows.h>

FocusControllerWin::FocusControllerWin()
{}

FocusControllerWin::~FocusControllerWin()
{}

void FocusControllerWin::switchFocusToOld()
{
    if (m_threadId == GetCurrentThreadId())
        return;
    AttachThreadInput(GetCurrentThreadId(), m_threadId, true);
    if (m_lastHwnd) {
        if (SetForegroundWindow(m_lastHwnd))
            qDebug() << "Foreground returned to" << m_lastHwnd;
        else {
            qDebug() << "Foreground  return failed to" << m_lastHwnd;
            qDebug() << "Error:" << GetLastError();
        }

        if (SetFocus(m_lastHwnd))
            qDebug() << "Focus returned to" << m_lastHwnd;
        else {
            qDebug() << "Focus return failed to" << m_lastHwnd;
            qDebug() << "Error:" << GetLastError();
        }
    } else {
        qDebug() << "Focus can't be returned to NULL";
    }
}

void FocusControllerWin::savePrevActive()
{
    LPGUITHREADINFO pgui = new GUITHREADINFO;
    pgui->cbSize = sizeof(GUITHREADINFO);
    if (GetGUIThreadInfo(NULL, pgui)) {
        m_lastHwnd = pgui->hwndFocus;
        m_threadId = GetWindowThreadProcessId(GetForegroundWindow(), nullptr);
        qDebug() << "Got GUI Thread" << pgui->flags << "," << pgui->hwndCaret << "," << pgui->hwndActive << ","
                 << pgui->hwndFocus;
    } else {
        m_lastHwnd = nullptr;
        m_threadId = 0;
        qDebug() << "Error:" << GetLastError();
    }
}

void FocusControllerWin::detachThread()
{
    AttachThreadInput(GetCurrentThreadId(), m_threadId, false);
    m_threadId = 0;
}

FocusController::FocusController(QObject *parent) : QObject(parent)
{
    this->controller = new FocusControllerWin();
}

void FocusController::switchFocus()
{
    FocusControllerWin *ctrl = reinterpret_cast<FocusControllerWin *>(controller);
    ctrl->switchFocusToOld();
}

void FocusController::savePrevActive()
{
    FocusControllerWin *ctrl = reinterpret_cast<FocusControllerWin *>(controller);
    ctrl->savePrevActive();
}

void FocusController::detachThread()
{
    FocusControllerWin *ctrl = reinterpret_cast<FocusControllerWin *>(controller);
    ctrl->detachThread();
}

FocusController::~FocusController()
{
    FocusControllerXcb *ctrl = reinterpret_cast<FocusControllerXcb *>(controller);
    delete &ctrl;
}
