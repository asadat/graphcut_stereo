#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    char cmd[256];
    sprintf(cmd,"source ~/graphcut_stereo/test.sh %s %s %s %s", ui->lineEdit->text().toStdString().c_str(),
            ui->lineEdit_2->text().toStdString().c_str()
            ,(ui->checkBox_4->checkState()==Qt::Checked)?"-swap":"-expansion",
            (ui->checkBox->checkState()==Qt::Checked)?"-potts":"-linear");
    system(cmd);
}
