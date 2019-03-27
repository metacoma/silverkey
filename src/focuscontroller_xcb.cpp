#include "focuscontroller_xcb.h"

#include "focuscontroller.h"

#include <QDebug>

#include <xcb/xcb_ewmh.h>

void FocusControllerXcb::focusWindowDebug(const QString &prefix, xcb_window_t window)
{
    xcb_ewmh_get_utf8_strings_reply_t ewmh_txt_prop;
    ewmh_txt_prop.strings = nullptr;

    const xcb_get_property_cookie_t cookie = xcb_ewmh_get_wm_name_unchecked(m_ewmh, window);
    if (xcb_ewmh_get_wm_name_reply(m_ewmh, cookie, &ewmh_txt_prop, nullptr)) {
        qDebug("%s %.*s", prefix.toLatin1().data(), ewmh_txt_prop.strings_len, ewmh_txt_prop.strings);
    } else {
        qDebug("%s xcb_ewmh_get_wm_name failed", prefix.toLatin1().data());
    }
}

FocusControllerXcb::FocusControllerXcb()
{
    m_dpy = xcb_connect(nullptr, &m_defaultScreen);
    m_ewmh = reinterpret_cast<xcb_ewmh_connection_t *>(malloc(sizeof(xcb_ewmh_connection_t)));
    xcb_intern_atom_cookie_t *ewmh_cookies = xcb_ewmh_init_atoms(m_dpy, m_ewmh);
    xcb_ewmh_init_atoms_replies(m_ewmh, ewmh_cookies, nullptr);
}

FocusControllerXcb::~FocusControllerXcb()
{
    if (m_ewmh)
        free(m_ewmh);
}

void FocusControllerXcb::switchFocusToOld()
{
    focusWindowDebug("switchFocusToOld", m_win);

    xdo_t *dpy = xdo_new(nullptr);

    if (!dpy) {
        qDebug("dpy init fail");
        return;
    }

    int result = xdo_activate_window(dpy, m_win);

    if (result) {
        qDebug("xdo_activate_window fail, ret: %d", result);
        return;
    }

    xdo_wait_for_window_active(dpy, m_win, 1);

    xcb_set_input_focus(m_dpy, XCB_INPUT_FOCUS_POINTER_ROOT, m_win, XCB_CURRENT_TIME);
}

void FocusControllerXcb::savePrevActive()
{
    xcb_ewmh_get_active_window_reply(m_ewmh, xcb_ewmh_get_active_window(m_ewmh, m_defaultScreen), &m_win, nullptr);
    focusWindowDebug("savePrevActive", m_win);
}

FocusController::FocusController(QObject *parent) : QObject(parent)
{
    controller = new FocusControllerXcb();
}

void FocusController::switchFocus()
{
    FocusControllerXcb *ctrl = reinterpret_cast<FocusControllerXcb *>(controller);
    ctrl->switchFocusToOld();
}

void FocusController::savePrevActive()
{
    FocusControllerXcb *ctrl = reinterpret_cast<FocusControllerXcb *>(controller);
    ctrl->savePrevActive();
}

FocusController::~FocusController()
{
    FocusControllerXcb *ctrl = reinterpret_cast<FocusControllerXcb *>(controller);
    delete ctrl;
}
