#include "gitcommitdialog.h"
#include "ui_gitcommitdialog.h"

GitCommitDialog::GitCommitDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::GitCommitDialog)
{
  ui->setupUi(this);
}

GitCommitDialog::~GitCommitDialog()
{
  delete ui;
}

void GitCommitDialog::on_commitButton_clicked()
{

}

void GitCommitDialog::on_cancelButton_clicked()
{

}
