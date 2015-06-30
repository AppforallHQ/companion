#include "restoreworker.h"
#include "deviceapi.h"

RestoreWorker::RestoreWorker(QString udid, QVector<QString> filePaths, QObject *parent)
    : QObject(parent)
{
    this->udid = udid;
    this->filePaths = filePaths;
    this->canceled = false;
}

void RestoreWorker::cancel()
{
    this->canceled = true;
}

void RestoreWorker::process()
{
    int total = this->filePaths.length();
    int success = 0;
    int failure = 0;
   
    foreach(const QString &filePath, this->filePaths)
    {
        if(this->canceled)
        {
            emit update(success, failure, total);
            emit finished();
            return;
        }
        
        if(DeviceAPI::getInstance().restoreApp(this->udid, filePath))
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
