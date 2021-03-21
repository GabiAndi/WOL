#include "acercadewindow.h"
#include "ui_acercadewindow.h"

AcercaDeWindow::AcercaDeWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AcercaDeWindow)
{
    ui->setupUi(this);
}

AcercaDeWindow::~AcercaDeWindow()
{
    delete ui;
}

void AcercaDeWindow::closeEvent(QCloseEvent *)
{
    deleteLater();
}
