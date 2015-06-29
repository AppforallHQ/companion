#ifndef DEVICESELECT_H
#define DEVICESELECT_H

#include "deviceapi.h"

#include <QDialog>

namespace Ui {
class DeviceSelect;
}

class DeviceSelect : public QDialog
{
    Q_OBJECT

public:
    explicit DeviceSelect(QWidget *parent = 0);
    void updateDeviceList();
    Device getSelectedDevice();
    ~DeviceSelect();

private slots:
    void on_pushButton_clicked();
    
private:
    Ui::DeviceSelect *ui;
};

#endif // DEVICESELECT_H
