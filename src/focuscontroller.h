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
    void sendToFront();
    void eraseOldAppPtr();
    void setOldAppId(QString bid);

private:
    void *controller;
};

#endif // FOCUSCONTROLLER_H
