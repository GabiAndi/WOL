#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QByteArray>
#include <QNetworkInterface>
#include <QList>
#include <QFile>
#include <QTextStream>

#include <QListWidget>
#include <QListWidgetItem>

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

        void sendMagicPackage(QString MAC, QString iface);

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
        void on_pushButtonGuardar_clicked();
        void on_pushButtonEliminar_clicked();
        void on_listWidgetEquipos_itemDoubleClicked(QListWidgetItem *item);
        void on_pushButtonCargar_clicked();
};
#endif // MAINWINDOW_H
