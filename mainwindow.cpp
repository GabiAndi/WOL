#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    getInterfaces();
}

MainWindow::~MainWindow()
{
    delete interfaces;

    delete ui;
}

void MainWindow::getInterfaces()
{
    delete interfaces;

    ui->comboBoxIfaces->clear();

    // Se obtienen las interfaces de red
    interfaces = new QList<QNetworkInterface>(QNetworkInterface::allInterfaces());

    for (QNetworkInterface iface : *interfaces)
    {
        ui->comboBoxIfaces->addItem(iface.name());
    }
}

void MainWindow::on_pushButtonWOL_clicked()
{
    /* Para poder despertar un dispositivo mediante el protocolo Wake On Lan (WOL),
     * se debe enviar una cadena de 6 bytes con el valor de 255 y luego 16 veces
     * la dirección MAC del dispositivo a encender.
     *
     * Quedaria algo asi:
     *
     * FF:FF:FF:FF:FF:FF
     * 10:20:30:50:40:60
     * ................. <- 14 veces restantes la MAC
     * 10:20:30:50:40:60
     *
     * Este paquete se manda mediante UDP al broadcast de la red por el puerto 9.
    */

    // Donde se almacena la dirección MAC
    QString MAC = ui->lineEditMAC->text();

    MAC.remove(":");

    QByteArray data1MAC = QByteArray::fromHex(MAC.toLocal8Bit());
    QByteArray data6FF = QByteArray::fromHex("FFFFFFFFFFFF");

    QByteArray data16MAC;

    for (int i = 0; i < 16; i++)
    {
        data16MAC += data1MAC;
    }

    QByteArray magicPackage = data6FF + data16MAC;

    // Se crea un scoket UDP
    QUdpSocket qudpsocket;

    // Se envia el paquete mágico al broadcast asi todos los equipor en la red lo escucharan
    QString broadcastIP(QNetworkInterface::allAddresses()[ui->comboBoxIfaces->currentIndex()].toString().left(
                QNetworkInterface::allAddresses()[ui->comboBoxIfaces->currentIndex()].toString().lastIndexOf('.')).append(".255"));

    qudpsocket.writeDatagram(magicPackage, magicPackage.size(), QHostAddress(broadcastIP), 9);
}

void MainWindow::on_actionActualizar_interfaces_triggered()
{
    getInterfaces();
}

void MainWindow::on_actionSalir_triggered()
{
    close();
}

void MainWindow::on_actionAcerca_de_triggered()
{
    acercaDe = new AcercaDeWindow(this);

    acercaDe->show();
}
