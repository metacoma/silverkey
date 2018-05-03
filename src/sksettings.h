#ifndef SKSETTINGS_H
#define SKSETTINGS_H

#include <QDialog>

namespace Ui {
class SKSettings;
}

class SKSettings : public QDialog
{
    Q_OBJECT

public:
    explicit SKSettings(QWidget *parent = 0);
    ~SKSettings();

public Q_SLOTS:
    void accept() override;

private:
    Ui::SKSettings *ui;
};

#endif // SKSETTINGS_H
