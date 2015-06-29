#ifndef BACKUPTASK_H
#define BACKUPTASK_H

#include <QObject>
#include <QProgressDialog>

class BackupTask : public QObject
{
    Q_OBJECT
public:
    explicit BackupTask(QObject *parent = 0);
    
signals:
    
public slots:
    void perform();
    void cancel();
    
private:
    QString udid;
    QVector<QString> bundleIds;
    QString destination;
    
    QProgressDialog* progressDialog;
    
};

#endif // BACKUPTASK_H
