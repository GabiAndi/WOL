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

    for (QNetworkAddressEntry addressEntrie : interface.addressEntries())
    {
        if (addressEntrie.ip().toString().count(".") == 3)
        {
            address = addressEntrie.ip();
        }
    }

    QString broadcastIP(address.toString().left(address.toString().lastIndexOf('.')).append(".255"));

    // Se envia el paquete mágico al broadcd ast asi todos los equipor en la relo escucharan
    qudpsocket.writeDatagram(magicPackage, magicPackage.size(), QHostAddress(broadcastIP), 9);
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
    QJsonDocument json;
    QJsonObject equipo;

    QFile data(JsonFileName);

    if (data.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        // Se pasa la información de la lista de equipos al QByteArray
        equipo["desc"] = ui->lineEditDescripcion->text();
        equipo["mac"] = ui->lineEditMAC->text();
        equipo["iface"] = ui->comboBoxIfaces->currentText();

        json.setObject(equipo);

        QByteArray dataJson = json.toJson();

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
    QJsonDocument json;
    QJsonObject equipo;

    QFile data(JsonFileName);

    if (data.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QByteArray dataJson = data.readAll();

        json = json.fromJson(dataJson);
        equipo = json.object();

        ui->lineEditDescripcion->setText(equipo["desc"].toString());
        ui->lineEditMAC->setText(equipo["mac"].toString());

        int index = ui->comboBoxIfaces->findText(equipo["iface"].toString());

        if (index != -1)
        {
            ui->comboBoxIfaces->setCurrentIndex(index);
        }

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
    /*if (ui->listWidgetEquipos->currentItem() != nullptr)
    {
        // Se extrae la MAC
        QString dato(ui->listWidgetEquipos->currentItem()->text());

        QStringList datos = dato.split(" | ");

        // Se envia el pulso de encendido
        sendMagicPackage(datos.at(1), datos.at(2));
    }*/
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

        equipo.clear();
        equipo.append(ui->lineEditDescripcion->text());
        equipo.append(ui->lineEditMAC->text());
        equipo.append(ui->comboBoxIfaces->currentText());

        ui->treeWidgetEquipos->addTopLevelItem(new QTreeWidgetItem(equipo));

        saveJsonData();
    }
}

void MainWindow::on_pushButtonEliminar_clicked()
{
    /*if (ui->listWidgetEquipos->currentItem() != nullptr)
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
    }*/
}

void MainWindow::on_pushButtonCargar_clicked()
{
    /*if (ui->listWidgetEquipos->currentItem() != nullptr)
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
    }*/
}
