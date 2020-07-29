#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QByteArray>
#include <QNetworkInterface>
#include <QList>

#include <QMessageBox>

#include "acercadewindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
        Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

    private:
        Ui::MainWindow *ui;

        QList<QNetworkInterface> *interfaces = nullptr;

        AcercaDeWindow *acercaDe = nullptr;

        void getInterfaces();

    private slots:
        void on_pushButtonWOL_clicked();
        void on_actionActualizar_interfaces_triggered();
        void on_actionSalir_triggered();
        void on_actionAcerca_de_triggered();
};
#endif // MAINWINDOW_H
