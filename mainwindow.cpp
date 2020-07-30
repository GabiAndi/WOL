#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Captura de las interfacez de red
    getInterfaces();

    // Se abre el archivo de los equipos guardados
    QFile file("equipos.txt");

    // El archivo se pudo abrir
    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream texto(&file);

        QStringList items;

        items = QString(texto.readAll()).split(";");

        file.close();

        for (QString item : items)
        {
            if (item != "\n" && item != "")
            {
                ui->listWidgetEquipos->addItem(item);
            }
        }
    }
}

MainWindow::~MainWindow()
{
    delete interfaces;

    delete ui;
}

void MainWindow::sendMagicPackage(QString MAC, QString iface)
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

    // Se obtione el indice de la interfaz
    int ifaceIndex = 0;

    for (int i = 0 ; i < ui->comboBoxIfaces->count() ; i++)
    {
        if (ui->comboBoxIfaces->itemText(i) == iface)
        {
            ifaceIndex = i;
        }
    }

    // Se envia el paquete mágico al broadcast asi todos los equipor en la red lo escucharan
    QString broadcastIP(QNetworkInterface::allAddresses()[ifaceIndex].toString().left(
                QNetworkInterface::allAddresses()[ifaceIndex].toString().lastIndexOf('.')).append(".255"));

    qudpsocket.writeDatagram(magicPackage, magicPackage.size(), QHostAddress(broadcastIP), 9);
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
    if (ui->listWidgetEquipos->currentItem() != nullptr)
    {
        // Se extrae la MAC
        QString dato(ui->listWidgetEquipos->currentItem()->text());

        QStringList datos = dato.split(" | ");

        // Se envia el pulso de encendido
        sendMagicPackage(datos.at(1), datos.at(2));
    }
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

void MainWindow::on_pushButtonGuardar_clicked()
{
    if (ui->lineEditMAC->text().size() > 0 && ui->lineEditDescripcion->text().size() > 0)
    {
        ui->listWidgetEquipos->addItem(QString(ui->lineEditDescripcion->text() + " | " + ui->lineEditMAC->text() + " | " +
                                               ui->comboBoxIfaces->currentText()));

        // Se abre el archivo de los equipos guardados
        QFile file("equipos.txt");

        // El archivo se pudo abrir
        if (file.open(QIODevice::Append))
        {
            QTextStream texto(&file);

            texto << QString(ui->listWidgetEquipos->item(ui->listWidgetEquipos->count() - 1)->text() + ";");

            file.close();
        }
    }
}

void MainWindow::on_pushButtonEliminar_clicked()
{
    if (ui->listWidgetEquipos->currentItem() != nullptr)
    {
        delete ui->listWidgetEquipos->currentItem();

        // Se abre el archivo de los equipos guardados
        QFile file("equipos.txt");

        // El archivo se pudo abrir
        if (file.open(QIODevice::WriteOnly))
        {
            QTextStream texto(&file);

            texto.flush();

            for (int i = 0 ; i < ui->listWidgetEquipos->count() ; i++)
            {
                texto << QString(ui->listWidgetEquipos->item(i)->text() + ";");
            }

            file.close();
        }
    }
}

void MainWindow::on_listWidgetEquipos_itemDoubleClicked(QListWidgetItem *item)
{
    // Se extrae la MAC
    QString dato(item->text());

    QStringList datos = dato.split(" | ");

    // Se envia el pulso de encendido
    sendMagicPackage(datos.at(1), datos.at(2));
}

void MainWindow::on_pushButtonCargar_clicked()
{
    if (ui->listWidgetEquipos->currentItem() != nullptr)
    {
        // Se extrae la MAC
        QString dato(ui->listWidgetEquipos->currentItem()->text());

        QStringList datos = dato.split(" | ");

        ui->lineEditDescripcion->setText(datos.at(0));
        ui->lineEditMAC->setText(datos.at(1));

        for (int i = 0 ; i < ui->comboBoxIfaces->count() ; i++)
        {
            if (ui->comboBoxIfaces->itemText(i) == datos.at(2))
            {
                ui->comboBoxIfaces->setCurrentIndex(i);
            }
        }
    }
}
