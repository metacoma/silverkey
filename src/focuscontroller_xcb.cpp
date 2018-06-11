#include <QDebug>
#include "focuscontroller.h"
#include "focuscontroller_xcb.h"

FocusControllerXcb::FocusControllerXcb()
{
    this->xcbConnection = QX11Info::connection();
}
/*
FocusControllerXcb::~FocusControllerXcb()
{

}
*/

void FocusControllerXcb::switchFocusToOld()
{
  xcb_set_input_focus(this->xcbConnection, XCB_INPUT_FOCUS_PARENT, this->focusReply->focus, XCB_CURRENT_TIME);
}

void FocusControllerXcb::savePrevActive()
{
    this->focusCookie = xcb_get_input_focus(this->xcbConnection);
    this->focusReply = xcb_get_input_focus_reply(this->xcbConnection, this->focusCookie, &this->xcbError);
    qDebug() << "Focus on " << this->focusReply->focus << " window";
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




