#include <QDebug>
#include <xcb/xcb_ewmh.h>
#include "focuscontroller.h"
#include "focuscontroller_xcb.h"

void FocusControllerXcb::FocusWindowDebug(char *prefix, xcb_window_t win) {
    xcb_ewmh_get_utf8_strings_reply_t ewmh_txt_prop;
    ewmh_txt_prop.strings = NULL;

    const xcb_get_property_cookie_t cookie = xcb_ewmh_get_wm_name_unchecked(this->ewmh, win);
    if (xcb_ewmh_get_wm_name_reply(this->ewmh, cookie, &ewmh_txt_prop, NULL)) {
        qDebug("%s %.*s", prefix, ewmh_txt_prop.strings_len, ewmh_txt_prop.strings);
    } else {
        qDebug("%s xcb_ewmh_get_wm_name failed", prefix);
    }
}

FocusControllerXcb::FocusControllerXcb()
{
    this->dpy = xcb_connect(NULL, &this->defaultScreen);
    this->ewmh = (xcb_ewmh_connection_t *)malloc(sizeof(xcb_ewmh_connection_t));
    xcb_intern_atom_cookie_t *ewmh_cookies = xcb_ewmh_init_atoms(this->dpy, this->ewmh);
    xcb_ewmh_init_atoms_replies(this->ewmh, ewmh_cookies, NULL);

}
FocusControllerXcb::~FocusControllerXcb()
{
  if (this->ewmh)
    free(this->ewmh);
}

void FocusControllerXcb::switchFocusToOld()
{
  this->FocusWindowDebug("switchFocusToOld", this->win);
  xcb_ewmh_set_active_window(this->ewmh, this->defaultScreen, this->win);
  xcb_set_input_focus(this->dpy, XCB_INPUT_FOCUS_POINTER_ROOT, this->win, XCB_CURRENT_TIME);
}

void FocusControllerXcb::savePrevActive()
{
    xcb_ewmh_get_active_window_reply(this->ewmh, xcb_ewmh_get_active_window(this->ewmh, this->defaultScreen), &this->win, NULL);
    this->FocusWindowDebug("savePrevActive", this->win);
}

FocusController::FocusController(QObject *parent) :
      QObject(parent)
{
      this->controller = new FocusControllerXcb();
}

void FocusController::switchFocus()
{
      FocusControllerXcb *ctrl = (FocusControllerXcb *) controller;
      ctrl->switchFocusToOld();
}


void FocusController::savePrevActive() {
      FocusControllerXcb *ctrl = (FocusControllerXcb *) controller;
      ctrl->savePrevActive();
}

FocusController::~FocusController()
{
      delete &controller;
}




