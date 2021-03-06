#include "gitinitdialog.h"
#include "ui_gitinitdialog.h"
#include "gitapi.h"
#include "gitapiresponse.h"
#include <string>
#include <QFileSystemModel>
#include <QtCore>
#include <QtGui>
#include <QDialog>
#include <iostream>
#include "giterrordialog.h"

GitInitDialog::GitInitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GitInitDialog)
{
    ui->setupUi(this);

    QString rootPath = "/home";
    treeModel = new QFileSystemModel(this);
    treeModel->setRootPath(rootPath);
    treeModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    ui->treeView->setModel(treeModel);
    ui->treeView->setRootIndex(treeModel->index(rootPath));
    ui->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->treeView->hideColumn(1);
    ui->treeView->hideColumn(2);
    ui->treeView->hideColumn(3);
}

GitInitDialog::~GitInitDialog()
{
    delete ui;
}

void GitInitDialog::on_initButton_clicked()
{

  string path = ui->path->text().toStdString();
  if(path.empty()){
    QString message = "Must specify a path!";
    GitErrorDialog ErrorBox;
    ErrorBox.setModal(true);
    ErrorBox.updateMessage(message);
    ErrorBox.exec();
    return;
  }
  GitAPIResponse response = GitApi::gitInit(path);
  if(response.getError()){
    QString message = QString::fromStdString(response.getMessage());
    GitErrorDialog ErrorBox;
    ErrorBox.setModal(true);
    ErrorBox.updateMessage(message);
    ErrorBox.exec();
    reject();
  }
  //*(this->path) = QString::fromStdString(path);
  accept();
}

void GitInitDialog::on_cancelButton_clicked()
{
  reject();
}

void GitInitDialog::on_treeView_activated(const QModelIndex &index)
{

}

void GitInitDialog::on_treeView_clicked(const QModelIndex &index)
{
    QString path = QDir::toNativeSeparators(treeModel->fileInfo(index).canonicalFilePath());
    ui->path->setText(path);
}
