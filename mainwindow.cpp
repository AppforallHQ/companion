#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "deviceselect.h"
#include "backupworker.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QThread>
#include <QtGui>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->isDeviceSelected = false;
    this->deviceSelect = NULL;
    this->selectDevice();
}

void MainWindow::selectDevice()
{
    if(!this->deviceSelect)
    {
        deviceSelect = new DeviceSelect();
        deviceSelect->setModal(true);
        connect(deviceSelect, SIGNAL(accepted()), this, SLOT(getSelectedDevice()));
        connect(deviceSelect, SIGNAL(rejected()), this, SLOT(close()));
    }
    
    deviceSelect->show();
}

void MainWindow::getSelectedDevice()
{
    this->device = deviceSelect->getSelectedDevice();
    
    if(this->device.udid.length() == 0)
    {
        exit(0);
        return;
    }
    
    this->isDeviceSelected = true;
    
    ui->statusBar->showMessage(QString("%1 (%2)").arg(this->device.name, this->device.udid));
    
    QVector<App> apps = DeviceAPI::getInstance().getAppsList(this->device.udid);
    
    ui->deviceAppsTableView->clear();
    ui->deviceAppsTableView->setColumnCount(2);
    ui->deviceAppsTableView->setRowCount(apps.length());
    
    int i = 0;
    
    foreach(const App &app, apps)
    {
        QTableWidgetItem* checkboxItem = new QTableWidgetItem("");
        checkboxItem->setCheckState(Qt::Unchecked);
        checkboxItem->setData(Qt::UserRole, app.bundleId);
        ui->deviceAppsTableView->setItem(i, 0, checkboxItem);
        
        QTableWidgetItem* appNameItem = new QTableWidgetItem(app.name);
        ui->deviceAppsTableView->setItem(i, 1, appNameItem);
        
//       QTableWidgetItem* appVersionItem = new QTableWidgetItem(app.version);
//       ui->deviceAppsTableView->setItem(i, 2, appVersionItem);
        
        i++;
    }
    
    QStringList headers;
    headers << " " << "Application Name" << "Version";
    ui->deviceAppsTableView->setHorizontalHeaderLabels(headers);
    ui->deviceAppsTableView->resizeColumnToContents(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*void MainWindow::on_selectAll_stateChanged(int state)
{
    if(state == Qt::Checked)
    {
        for(int i = 0; i < ui->deviceAppsTableView->rowCount(); i++)
        {
            ui->deviceAppsTableView->item(i, 0)->setCheckState(Qt::Checked);
        }
    }
    else
    {
        for(int i = 0; i < ui->deviceAppsTableView->rowCount(); i++)
        {
            ui->deviceAppsTableView->item(i, 0)->setCheckState(Qt::Unchecked);
        }
    }
}*/

void MainWindow::on_backupButton_clicked(bool checked)
{
    QVector<QString> selectedBundleIds;
    checked = false;
    
    for(int i = 0; i < ui->deviceAppsTableView->rowCount(); i++)
    {
        if(ui->deviceAppsTableView->item(i, 0)->checkState() == Qt::Checked)
        {
            selectedBundleIds.append(ui->deviceAppsTableView->item(i, 0)->data(Qt::UserRole).toString());
        }
    }
    
    if(selectedBundleIds.length() == 0)
    {
        QMessageBox messageBox;
        messageBox.critical(this, "Error", "Please select at least one app to backup.", QMessageBox::Ok);
        return;
    }
    
    QString destinationPath = QFileDialog::getExistingDirectory(this, "Backup Destination", QString());
    
    if(destinationPath.length() == 0)
        return;
    
    if(!progressDialog)
    {
        progressDialog = new QProgressDialog(this);
        progressDialog->setWindowModality(Qt::WindowModal);
    }
    
    QString progressMessage = QString("Transfering apps from device (%1/%2)..."); 
    this->progressDialog->setLabelText(progressMessage.arg("0", QString::number(selectedBundleIds.length())));
    this->progressDialog->setMinimum(0);
    this->progressDialog->setMaximum(selectedBundleIds.length());
    
    QThread* backupThread = new QThread();
    BackupWorker* backupWorker = new BackupWorker(this->device.udid, selectedBundleIds, destinationPath);
    backupWorker->moveToThread(backupThread);
    
    // connecting signals
    connect(backupWorker, SIGNAL(update(int, int, int)), this, SLOT(updateBackupProgress(int,int,int)));
    connect(backupThread, SIGNAL(started()), backupWorker, SLOT(process()));
    connect(backupThread, SIGNAL(finished()), backupThread, SLOT(deleteLater()));
    connect(backupWorker, SIGNAL(finished()), backupThread, SLOT(quit()));
    connect(backupWorker, SIGNAL(finished()), this, SLOT(finishBackup()));
    connect(backupWorker, SIGNAL(finished()), backupWorker, SLOT(deleteLater()));
    connect(progressDialog, SIGNAL(canceled()), backupWorker, SLOT(cancel()));
    
    backupThread->start();
    progressDialog->exec();
}

void MainWindow::updateBackupProgress(int success, int failure, int total)
{
    QString message;
    
    if(failure == 0)
    {
        message = QString("Transfering apps from device (%1/%2)...").arg(QString::number(success),
                                                                         QString::number(total));
    }
    else
    {
       message = QString("Transfering apps from device (%1/%2), %3 failed...").arg(QString::number(success),
                                                                                   QString::number(total),
                                                                                   QString::number(failure)); 
    }
    
    this->progressDialog->setLabelText(message);
    this->progressDialog->setValue(success + failure);
}

void MainWindow::finishBackup()
{
    this->progressDialog->close();
    QMessageBox::information(this, "Backup done.", "Backup finished successfully.");
}

void MainWindow::on_restoreDirBrowseButton_clicked()
{
    QString restorePath = QFileDialog::getExistingDirectory(this, "Restore", QString());
    ui->restoreDirLineEdit->setText(restorePath);
    
    QDir restoreDir = QDir(restorePath, "*.ipa");
    if(restoreDir.count() == 0)
    {
        QMessageBox::critical(this, "Error", "No files found to restore.");
        return;
    }
    
    ui->restoreFilesTableView->clear();
    ui->restoreFilesTableView->setColumnCount(2);
    ui->restoreFilesTableView->setRowCount(restoreDir.count());
    
    int i = 0;
    
    foreach(const QFileInfo &ipa, restoreDir.entryInfoList())
    {
        QTableWidgetItem* checkboxItem = new QTableWidgetItem("");
        checkboxItem->setCheckState(Qt::Checked);
        checkboxItem->setData(Qt::UserRole, ipa.filePath());
        ui->restoreFilesTableView->setItem(i, 0, checkboxItem);
        
        QTableWidgetItem* appNameItem = new QTableWidgetItem(ipa.fileName());
        ui->restoreFilesTableView->setItem(i, 1, appNameItem);
        
        i++;
    }
    
    QStringList headers;
    headers << " " << "File Name" << "Version";
    ui->restoreFilesTableView->setHorizontalHeaderLabels(headers);
    ui->restoreFilesTableView->resizeColumnToContents(0);
}

void MainWindow::on_restoreButton_clicked()
{
    QVector<QString> selectedFiles;
    
    for(int i = 0; i < ui->deviceAppsTableView->rowCount(); i++)
    {
        if(ui->restoreFilesTableView->item(i, 0)->checkState() == Qt::Checked)
        {
            selectedFiles.append(ui->restoreFilesTableView->item(i, 0)->data(Qt::UserRole).toString());
        }
    }
    
    
}
