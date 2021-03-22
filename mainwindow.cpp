#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Captura de las interfacez de red
    getInterfaces();

    // Se cargan los equipos guardados
    readJsonData();
}

MainWindow::~MainWindow()
{
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
     *
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

    // Se obtiene la interfaz de red
    QNetworkInterface interface = QNetworkInterface::allInterfaces().at(QNetworkInterface::interfaceIndexFromName(iface) - 1);
    QHostAddress address;

    for (int i = 0 ; i < interface.addressEntries().length() ; i++)
    {
        // Se verifica si la IP es válida
        if (interface.addressEntries().at(i).ip().toString().count(".") == 3)
        {
            address = interface.addressEntries().at(i).ip();

            // De la IP se obtiene el broadcast
            QString broadcastIP(address.toString().left(address.toString().lastIndexOf('.')).append(".255"));

            // Se envia el paquete mágico al broadcd ast asi todos los equipor en la red lo escucharan
            qudpsocket.writeDatagram(magicPackage, magicPackage.size(), QHostAddress(broadcastIP), 9);
        }
    }
}

void MainWindow::getInterfaces()
{
    ui->comboBoxIfaces->clear();

    // Se obtienen las interfaces de red
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();

    for (int i = 0 ; i < interfaces.length() ; i++)
    {
        ui->comboBoxIfaces->addItem(interfaces.at(i).name());
    }
}

void MainWindow::saveJsonData()
{
    QFile data(JsonFileName);

    if (data.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QJsonDocument json;
        QJsonObject obj;
        QJsonArray values;
        QJsonObject objs;
        QByteArray dataJson;

        // Se añaden los equipos que esten guardados
        for (int i = 0 ; i < ui->treeWidgetEquipos->topLevelItemCount() ; i++)
        {
            // Se añaden los campos de valor
            obj["desc"] = ui->treeWidgetEquipos->topLevelItem(i)->text(0);
            obj["mac"] = ui->treeWidgetEquipos->topLevelItem(i)->text(1);
            obj["iface"] = ui->treeWidgetEquipos->topLevelItem(i)->text(2);

            // Se agrega el obj a un array
            values.append(obj);
        }

        // Se almacenan todos los equipos en un objeto padre
        objs["equipos"] = values;

        json.setObject(objs);

        dataJson = json.toJson();

        // Se escribe el contenido en el archivo
        data.write(dataJson);
        data.close();

        ui->statusBar->showMessage("Guardado correctamente el archivo de equipos");
    }

    else
    {
        ui->statusBar->showMessage("No se pudo guardar el archivo de equipos");
    }
}

void MainWindow::readJsonData()
{
    QFile data(JsonFileName);

    if (data.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QJsonDocument json;
        QJsonObject obj;
        QJsonArray values;
        QJsonObject objs;
        QByteArray dataJson;

        // Se lee el contenido del archivo
        dataJson = data.readAll();

        json = json.fromJson(dataJson);

        objs = json.object();

        values.append(objs["equipos"].toArray());
        values = values.at(0).toArray();

        // Se leen los equipos que esten guardados
        for (int i = 0 ; i < values.size() ; i++)
        {
            // Se leen los campos de valor
            QStringList equipo;

            obj = values.at(i).toObject();

            equipo.append(obj["desc"].toString());
            equipo.append(obj["mac"].toString());
            equipo.append(obj["iface"].toString());

            ui->treeWidgetEquipos->addTopLevelItem(new QTreeWidgetItem(equipo));
        }

        // Se cierra el archivo
        data.close();

        ui->statusBar->showMessage("Leido correctamente el archivo de equipos");
    }

    else
    {
        ui->statusBar->showMessage("No se pudo leer el archivo de equipos");
    }
}

void MainWindow::on_pushButtonWOL_clicked()
{
    if (ui->treeWidgetEquipos->currentItem() != nullptr)
    {
        // Se extrae la MAC
        QString mac(ui->treeWidgetEquipos->currentItem()->text(1));
        // Se obtiene la interfaz de red
        QString iface(ui->treeWidgetEquipos->currentItem()->text(2));

        // Se envia el pulso de encendido
        sendMagicPackage(mac, iface);
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
    // Si los campos de texto tiene algo lo guardamos
    if (ui->lineEditMAC->text().length() > 0 && ui->lineEditDescripcion->text().length() > 0)
    {
        QStringList equipo;

        equipo.append(ui->lineEditDescripcion->text());
        equipo.append(ui->lineEditMAC->text());
        equipo.append(ui->comboBoxIfaces->currentText());

        ui->treeWidgetEquipos->addTopLevelItem(new QTreeWidgetItem(equipo));

        saveJsonData();
    }
}

void MainWindow::on_pushButtonEliminar_clicked()
{
    if (ui->treeWidgetEquipos->currentItem() != nullptr)
    {
        // Se elimina el archivo
        delete ui->treeWidgetEquipos->currentItem();

        saveJsonData();
    }
}

void MainWindow::on_pushButtonCargar_clicked()
{
    if (ui->treeWidgetEquipos->currentItem() != nullptr)
    {
        // Se extran los datos
        QString desc(ui->treeWidgetEquipos->currentItem()->text(0));
        QString mac(ui->treeWidgetEquipos->currentItem()->text(1));
        QString iface(ui->treeWidgetEquipos->currentItem()->text(2));

        ui->lineEditDescripcion->setText(desc);
        ui->lineEditMAC->setText(mac);

        int index = ui->comboBoxIfaces->findText(iface);

        if (index != -1)
        {
            ui->comboBoxIfaces->setCurrentIndex(index);
        }
    }
}

void MainWindow::on_treeWidgetEquipos_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);

    if (item != nullptr)
    {
        // Se extrae la MAC
        QString mac(item->text(1));
        // Se obtiene la interfaz de red
        QString iface(item->text(2));

        // Se envia el pulso de encendido
        sendMagicPackage(mac, iface);
    }
}
