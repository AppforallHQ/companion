#include "deviceapi.h"

#include <QProcess>
#include <QVariant>
#include <QBuffer>
#include <QRegularExpression>

#include <plistparser.h>

#define DEVICE_LIST_COMMAND "/usr/local/bin/idevice_id -l"
#define DEVICE_NAME_COMMAND "/usr/local/bin/idevice_id %1"
#define GET_APP_LIST_COMMAND "/usr/local/bin/ideviceinstaller -u %1 -l -o xml"
#define BACKUP_APP_COMMAND "/usr/local/bin/ideviceinstaller -u %1 -a %2 -o copy=\"%3\" -o remove"

QVector<Device> DeviceAPI::getDevicesList()
{
    QVector<Device> result(0);
    
    QProcess deviceListProcess;
    deviceListProcess.start(DEVICE_LIST_COMMAND);
    deviceListProcess.waitForFinished(-1);
    
    QString output = deviceListProcess.readAllStandardOutput();
    //QString errorOutput = deviceListProcess.readAllStandardError();
    
    QStringList udids = output.split(QRegularExpression("[\\n\\s]+"), QString::SkipEmptyParts);
    
    foreach(const QString &udid, udids)
    {
        QProcess deviceNameProcess;
        deviceNameProcess.start(QString(DEVICE_NAME_COMMAND).arg(udid));
        deviceNameProcess.waitForFinished(-1);
        
        QString deviceName = deviceNameProcess.readAllStandardOutput();
        
        Device dev;
        dev.name = deviceName.trimmed();
        dev.udid = udid;
        
        result.append(dev);
    }
    
    return result;
}

QVector<App> DeviceAPI::getAppsList(QString udid, bool userOnly)
{
    QVector<App> result;
    
    QProcess appsListProcess;
    appsListProcess.start(QString(GET_APP_LIST_COMMAND).arg(udid));
    appsListProcess.waitForFinished(-1);
    
    if(appsListProcess.exitCode() != 0)
    {
        return result;
    }
    
    QByteArray data = appsListProcess.readAllStandardOutput();
    QBuffer buffer(&data);
    QVariantList apps = PListParser::parsePList(&buffer).toList();
    
    foreach(const QVariant &item, apps)
    {
        QVariantMap appData = item.toMap();
        
        if(userOnly)
        {
            if(!appData.contains("CFBundleURLTypes"))
                continue;
            
            bool hasAfaUrl = false;
            foreach(const QVariant &urlTypes, appData["CFBundleURLTypes"].toList())
            {
                QVariantMap urlData = urlTypes.toMap();
                foreach(const QVariant &urlScheme, urlData["CFBundleURLSchemes"].toList())
                {
                    if(urlScheme.toString().startsWith("userlid-"))
                    {
                        hasAfaUrl = true;
                        break;
                    }
                }
                
                if(hasAfaUrl)
                    break;
            }
            
            if(!hasAfaUrl)
                continue;
        }
        
        App app;
        
        if(appData.count("CFBundleDisplayName") > 0)
        {
            app.name = appData["CFBundleDisplayName"].toString();
        }
        else
        {
            app.name = appData["CFBundleName"].toString();
        }
        
        app.bundleId = appData["CFBundleIdentifier"].toString();
        app.version = appData["CFBundleVersion"].toString();
        
        result.append(app);
    }
    
    return result;
}

bool DeviceAPI::backupApp(QString udid, QString bundleId, QString targetDir)
{
    QProcess backupProcess;
    backupProcess.start(QString(BACKUP_APP_COMMAND).arg(udid, bundleId, targetDir));
    backupProcess.waitForFinished(-1);
    
    if(backupProcess.exitCode() == 0)    
        return true;
    else
        return false;
}

