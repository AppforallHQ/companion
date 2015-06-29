#ifndef BACKUPWORKER_H
#define BACKUPWORKER_H

#include <QObject>
#include <QVector>
#include <QString>

class BackupWorker : public QObject
{
    Q_OBJECT
public:
    explicit BackupWorker(QString udid, QVector<QString> bundleIds, QString destinationPath, QObject *parent = 0);
    
public slots:
    void process();
    void cancel();
    
signals:
    void update(int success, int failure, int total);
    void finished();
    
private:
    QString udid;
    QVector<QString> bundleIds;
    QString destinationPath;
    bool canceled;
};

#endif // BACKUPWORKER_H
