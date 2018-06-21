#ifndef FOCUSCONTROLLER_H
#define FOCUSCONTROLLER_H

#include <QObject>

class FocusController : public QObject
{
    Q_OBJECT
public:
    explicit FocusController(QObject *parent = nullptr);
    ~FocusController();
    void switchFocus();
    void savePrevActive();
#ifdef Q_OS_OSX
    void sendToFront();
    void setOldAppId(QString bid);
    void eraseOldAppPtr();
#endif // Q_OS_OSX

private:
    void *controller;
};

#endif // FOCUSCONTROLLER_H
