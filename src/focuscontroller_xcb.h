#ifndef FOCUSCONTROLLER_XCB_H
#define FOCUSCONTROLLER_XCB_H

#include <QX11Info>
#include <QObject>
#include <xcb/xcb.h>
#include <xcb/xcb_ewmh.h>

class FocusControllerXcb
{
public:
    FocusControllerXcb();
    ~FocusControllerXcb();
    void switchFocusToOld();
    void savePrevActive();
    //void switchFocus();
private:
    xcb_connection_t *dpy;
    int defaultScreen;
    xcb_window_t win;
    xcb_ewmh_connection_t *ewmh;
};
#endif // FOCUSCONTROLLER_XCB_H
