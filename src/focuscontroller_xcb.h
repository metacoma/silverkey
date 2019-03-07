#ifndef FOCUSCONTROLLER_XCB_H
#define FOCUSCONTROLLER_XCB_H

#include <QObject>
#include <QX11Info>

#include <xcb/xcb.h>
#include <xcb/xcb_ewmh.h>
extern "C" {
#include <xdo.h>
}

#undef Bool
#undef CursorShape
#undef Expose
#undef KeyPress
#undef KeyRelease
#undef FocusIn
#undef FocusOut
#undef FontChange
#undef None
#undef Status
#undef Unsorted

class FocusControllerXcb
{
public:
    FocusControllerXcb();
    ~FocusControllerXcb();
    void switchFocusToOld();
    void savePrevActive();

private:
    void focusWindowDebug(const QString &prefix, xcb_window_t window);

    xcb_connection_t *m_dpy;
    int m_defaultScreen;
    xcb_window_t m_win;
    xcb_ewmh_connection_t *m_ewmh = nullptr;
};
#endif // FOCUSCONTROLLER_XCB_H
