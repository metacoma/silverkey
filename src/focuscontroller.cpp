#include "focuscontroller.h"

FocusController::FocusController(QObject *parent) : QObject(parent)
{

}

FocusController::~FocusController()
{

}

void FocusController::switchFocus()
{

}

void FocusController::savePrevActive()
{

}

#ifdef Q_OS_OSX
void FocusController::sendToFront()
{

}
#endif // Q_OS_OSX
