#include "deviceselect.h"
#include "ui_deviceselect.h"

#include "deviceapi.h"

DeviceSelect::DeviceSelect(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeviceSelect)
{
    ui->setupUi(this);
    this->updateDeviceList();
}

void DeviceSelect::updateDeviceList()
{
    ui->devicesComboBox->clear();
    
    QVector<Device> devices = DeviceAPI::getInstance().getDevicesList();
    
    foreach(const Device &device, devices) 
    {
        ui->devicesComboBox->addItem(device.name, device.udid);
    }
    
    if(devices.length() == 0)
    {
        //ui->devicesComboBox->addItem("[No device was found]", "");
    }
}

DeviceSelect::~DeviceSelect()
{
    delete ui;
}

void DeviceSelect::on_pushButton_clicked()
{
    this->updateDeviceList();
}

Device DeviceSelect::getSelectedDevice()
{
    Device device;
    device.name = ui->devicesComboBox->currentText();
    device.udid = ui->devicesComboBox->currentData().toString();
    
    return device;
}
