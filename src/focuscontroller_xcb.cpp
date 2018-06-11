#include <QDebug>
#include "focuscontroller_xcb.h"

FocusController::FocusController(QObject *parent) : QObject(parent)
{
    this->xcbConnection = QX11Info::connection();
}


FocusController::~FocusController()
{

}



void FocusController::switchFocus()
{

}

void FocusController::savePrevActive()
{
    this->focusCookie = xcb_get_input_focus(this->xcbConnection);
    this->focusReply = xcb_get_input_focus_reply(this->xcbConnection, this->focusCookie, &this->xcbError);
    qDebug() << "Focus on " << this->focusReply->focus << " window";
}
