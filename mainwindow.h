#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "deviceapi.h"
#include "deviceselect.h"

#include <QMainWindow>
#include <QProgressDialog>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    void selectDevice();
    ~MainWindow();
    
public slots:
    void getSelectedDevice();
    void updateBackupProgress(int success, int failure, int total);
    void finishBackup();
    
//private slots:
//    void on_selectAll_stateChanged(int arg1);
    
private slots:
    void on_backupButton_clicked(bool checked);
    
    void on_restoreDirBrowseButton_clicked();
    
private:
    Ui::MainWindow *ui;
    DeviceSelect* deviceSelect;
    Device device;
    bool isDeviceSelected;
    
    QProgressDialog* progressDialog;
};

#endif // MAINWINDOW_H
