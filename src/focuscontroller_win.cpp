#include <QDebug>
#include <Windows.h>
#include "focuscontroller.h"
#include "focuscontroller_win.h"

FocusControllerWin::FocusControllerWin()
{

}
FocusControllerWin::~FocusControllerWin()
{
}

void FocusControllerWin::switchFocusToOld()
{


}

void FocusControllerWin::savePrevActive()
{

}

FocusController::FocusController(QObject *parent) :
      QObject(parent)
{
      this->controller = new FocusControllerWin();
}

void FocusController::switchFocus()
{
      FocusControllerWin *ctrl = (FocusControllerWin *) controller;
      ctrl->switchFocusToOld();
}


void FocusController::savePrevActive() {
      FocusControllerWin *ctrl = (FocusControllerWin *) controller;
      ctrl->savePrevActive();
}

FocusController::~FocusController()
{
      delete &controller;
}

