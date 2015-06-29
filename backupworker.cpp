#include "backupworker.h"
#include "deviceapi.h"

BackupWorker::BackupWorker(QString udid, QVector<QString> bundleIds, QString destinationPath, QObject *parent)
    : QObject(parent)
{
    this->udid = udid;
    this->bundleIds = bundleIds;
    this->destinationPath = destinationPath;
    this->canceled = false;
}

void BackupWorker::cancel()
{
    this->canceled = true;
}

void BackupWorker::process()
{
    int total = this->bundleIds.length();
    int success = 0;
    int failure = 0;
   
    foreach(const QString &bundleId, this->bundleIds)
    {
        if(this->canceled)
        {
            emit update(success, failure, total);
            emit finished();
            return;
        }
        
        if(DeviceAPI::getInstance().backupApp(this->udid, bundleId, this->destinationPath))
        {
            success++;
        }
        else
        {
            failure++;
        }
        
        emit update(success, failure, total);
    }
    
    emit finished();
}
