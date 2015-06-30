#ifndef RESTOREWORKER_H
#define RESTOREWORKER_H

#include <QObject>
#include <QVector>
#include <QString>

class RestoreWorker : public QObject
{
    Q_OBJECT
public:
    explicit RestoreWorker(QString udid, QVector<QString> filePaths, QObject *parent = 0);
    
public slots:
    void process();
    void cancel();
    
signals:
    void update(int success, int failure, int total);
    void finished();
    
private:
    QString udid;
    QVector<QString> filePaths;
    bool canceled;
};

#endif // RESTOREWORKER_H
