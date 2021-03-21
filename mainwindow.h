#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QNetworkInterface>

#include <QByteArray>
#include <QList>

#include <QDebug>

#include <QFile>
#include <QDir>

#include <QTreeWidget>
#include <QTreeWidgetItem>

#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>

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

        const QString JsonFileName = QDir::currentPath() + "/equipos.json";

        AcercaDeWindow *acercaDe = nullptr;

        void getInterfaces();

        void saveJsonData();
        void readJsonData();

    private slots:
        void on_pushButtonWOL_clicked();
        void on_actionActualizar_interfaces_triggered();
        void on_actionSalir_triggered();
        void on_actionAcerca_de_triggered();
        void on_pushButtonGuardar_clicked();
        void on_pushButtonEliminar_clicked();
        void on_pushButtonCargar_clicked();
};
#endif // MAINWINDOW_H
