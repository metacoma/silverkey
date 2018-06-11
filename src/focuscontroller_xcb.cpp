#include <QDebug>
#include <xcb/xcb_ewmh.h>
#include "focuscontroller.h"
#include "focuscontroller_xcb.h"

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
  //qDebug() << "Swtich focus to previous window #" << this->focusReply->focus;
  //xcb_set_input_focus(this->xcbConnection, XCB_INPUT_FOCUS_PARENT, this->focusReply->focus, XCB_CURRENT_TIME);
  xcb_ewmh_set_active_window(this->ewmh, this->defaultScreen, this->win);
}

void FocusControllerXcb::savePrevActive()
{
    //char msg[1024];
    xcb_ewmh_get_utf8_strings_reply_t ewmh_txt_prop;
    xcb_ewmh_get_active_window_reply(this->ewmh, xcb_ewmh_get_active_window(this->ewmh, this->defaultScreen), &this->win, NULL);

    //snprintf(msg, "%.*s", qMin(*ewmh_txt_prop.strings_len, sizeof(msg), ewmh_txt_prop.strings);
    //qDebug << msg;
    //printf("Window title: %.*s, id: %X\n", ewmh_txt_prop.strings_len, ewmh_txt_prop.strings, this->win);
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




