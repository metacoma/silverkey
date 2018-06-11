#ifndef FOCUSCONTROLLER_XCB_H
#define FOCUSCONTROLLER_XCB_H

#include <QX11Info>
#include <QObject>
#include <xcb/xcb.h>

class FocusController : public QObject
{
    Q_OBJECT
public:
    explicit FocusController(QObject *parent = nullptr);
    ~FocusController();
    void switchFocusToOld();
    void savePrevActive();
    void switchFocus();
private:
    xcb_connection_t *xcbConnection;
    xcb_get_input_focus_cookie_t focusCookie;
    xcb_generic_error_t *xcbError;
    xcb_get_input_focus_reply_t *focusReply;
};


#endif // FOCUSCONTROLLER_XCB_H
