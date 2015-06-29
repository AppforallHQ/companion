#ifndef DEVICEAPI_H
#define DEVICEAPI_H

#include <QVector>
#include <QString>

struct Device {
    QString name;
    QString udid;
};

struct App {
    QString name;
    QString bundleId;
    QString version;
};

class DeviceAPI
{
public:
    static DeviceAPI& getInstance() {
        static DeviceAPI instance;
        return instance;
    }
    
    QVector<Device> getDevicesList();
    QVector<App> getAppsList(QString udid, bool userOnly=true);
    bool backupApp(QString udid, QString bundleId, QString targetDir);
    
private:
    DeviceAPI() {}
    DeviceAPI(DeviceAPI const&);
    void operator=(DeviceAPI const&);
};

#endif // DEVICEAPI_H
