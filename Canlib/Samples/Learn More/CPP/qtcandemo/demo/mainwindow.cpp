#include <QDebug>
#include <QList>
#include "mainwindow.h"
#include "ui_mainwindow.h"

#define MAX_ROWS 1000

//------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  selected_channel = 0;
  selected_bitrate = 0;
  can_handle = canERR_NOTFOUND;
  rx_thread = NULL;

  qRegisterMetaType< CanMessage >( "CanMessage" ); // so it can be used in queues

  qDebug("Initializing lib");
  canInitializeLibrary();

  ui->setupUi(this);
  ui->actionBusOff->setDisabled(true);
  ui->actionTransmit->setDisabled(true);
  ui->comboBoxBusSpeed->addItem(QString("1000000"), QVariant(canBITRATE_1M));
  ui->comboBoxBusSpeed->addItem(QString("500000"), QVariant(canBITRATE_500K));
  ui->comboBoxBusSpeed->addItem(QString("250000"), QVariant(canBITRATE_250K));
  ui->comboBoxBusSpeed->addItem(QString("125000"), QVariant(canBITRATE_125K));
  ui->comboBoxBusSpeed->addItem(QString("100000"), QVariant(canBITRATE_100K));
  ui->comboBoxBusSpeed->addItem(QString("62500"), QVariant(canBITRATE_62K));
  ui->comboBoxBusSpeed->addItem(QString("50000"), QVariant(canBITRATE_50K));
  ui->comboBoxBusSpeed->addItem(QString("83333"), QVariant(canBITRATE_83K));
  ui->comboBoxBusSpeed->addItem(QString("10000"), QVariant(canBITRATE_10K));

  QStringList labels;
  labels << tr("ID") << tr("Flags") << tr("D0") << tr("D1") << tr("D2") << tr("D3")
         << tr("D4") << tr("D5") << tr("D6") << tr("D7") << tr("Time");
  ui->tableWidgetOutput->setHorizontalHeaderLabels(labels);
}

//------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
  delete ui;
  delete rx_thread;
}


//------------------------------------------------------------------------------
void MainWindow::refreshChannels()
{
  int stat = 0;
  int channels = 0;

  ui->comboBoxChannel->clear();
  stat = canGetNumberOfChannels(&channels);

  for (int i=0; i<channels; i++) {
    char name[64];
    stat = canGetChannelData(i, canCHANNELDATA_CHANNEL_NAME, name, sizeof(name));
    ui->comboBoxChannel->addItem(QString(name));
  }

  ui->comboBoxChannel->setCurrentIndex(selected_channel);
  ui->comboBoxBusSpeed->setCurrentIndex(selected_bitrate);
}

//------------------------------------------------------------------------------
void MainWindow::setChannel(int ch)
{
  selected_channel = ch; 
  qDebug() << QString("selected channel is %1").arg(selected_channel);
}

//------------------------------------------------------------------------------
void MainWindow::setBitrate(int num)
{
  selected_bitrate = num;
  int bitrate = ui->comboBoxBusSpeed->itemData(selected_bitrate).toInt();
  qDebug() << QString("selected bitrate is %1").arg(bitrate);
}

//------------------------------------------------------------------------------
void MainWindow::goBusOn()
{
  canStatus status;
  int flags = canOPEN_ACCEPT_VIRTUAL;
  flags |= ui->checkBoxExclusive->isChecked() ? canWANT_EXCLUSIVE : 0;

  can_handle = canOpenChannel(selected_channel, flags);
  if (can_handle < canOK) {
    qDebug() << QString("canOpenChannel() failed, %1").arg(can_handle);
    return;
  }

  int mode = ui->checkBoxSilentMode->isChecked() ? canDRIVER_SILENT : canDRIVER_NORMAL;
  status = canSetBusOutputControl(can_handle, mode);
  if (status != canOK) {
    qDebug() << QString("canSetBusOutputControl() failed, %1").arg(status);
    return;
  }

  int bitrate = ui->comboBoxBusSpeed->itemData(selected_bitrate).toInt();
  status = canSetBusParams(can_handle, bitrate, 0, 0, 0, 0, 0);
  if (status != canOK) {
    qDebug() << QString("canSetBusParams() failed, %1").arg(status);
    return;
  }

  status = canBusOn(can_handle);
  if (status != canOK) {
    qDebug() << QString("canBusOn() failed, %1").arg(status);
    return;
  }

  qDebug() << QString("Starting RX thread ... ");
  rx_thread = new CanReader(selected_channel);
  rx_thread->start();
  connect(rx_thread, SIGNAL(gotRx(CanMessage)), this, SLOT(rxUpdate(CanMessage)));
  qDebug() << QString("done!");

  ui->actionBusOff->setEnabled(true);
  ui->actionBusOn->setDisabled(true);
  ui->actionTransmit->setEnabled(true);
}

//------------------------------------------------------------------------------
void MainWindow::goBusOff()
{
  canStatus status;
  status = canBusOff(can_handle);
  if (status != canOK) {
    qDebug() << QString("canBusOff() failed, %1").arg(status);
    return;
  }

  qDebug() << QString("Stopping RX thread ... ");
  rx_thread->stopRunning();
  rx_thread->wait();
  delete rx_thread;
  rx_thread = NULL;
  disconnect(rx_thread, SIGNAL(gotRx(CanMessage)), this, SLOT(rxUpdate(CanMessage)));
  qDebug() << QString("done!");

  ui->actionBusOff->setEnabled(false);
  ui->actionBusOn->setDisabled(false);
  ui->actionTransmit->setDisabled(true);
}

//------------------------------------------------------------------------------
void MainWindow::rxUpdate(CanMessage msg)
{

  if (ui->tableWidgetOutput->rowCount() == MAX_ROWS) {
    ui->tableWidgetOutput->removeRow(MAX_ROWS-1);
  }

  int id_base = ui->checkBoxHexId->isChecked() ? 16 : 10;
  int flags_base = ui->checkBoxHexFlags->isChecked() ? 16 : 10;
  int data_base = ui->checkBoxHexData->isChecked() ? 16 : 10;

  QTableWidgetItem *id_item = new QTableWidgetItem(QString("%1").arg(msg.id, 0, id_base), 0);
  QTableWidgetItem *flags_item = new QTableWidgetItem(QString("%1").arg(msg.flag, 0, flags_base), 0);
  QTableWidgetItem *time_item = new QTableWidgetItem(QString("%1").arg(msg.time), 0);
  QTableWidgetItem *data_item[8];

  for (unsigned int i = 0; i < 8; i++) {
    QString data_str = QString("");
    if (i < msg.dlc) {
      data_str = QString("%1").arg(msg.data[i], 0, data_base);
    }
    data_item[i] = new QTableWidgetItem(data_str, 0);
  }

  ui->tableWidgetOutput->insertRow(0);
  ui->tableWidgetOutput->setItem(0, 0, id_item);
  ui->tableWidgetOutput->setItem(0, 1, flags_item);
  ui->tableWidgetOutput->setItem(0, 2, data_item[0]);
  ui->tableWidgetOutput->setItem(0, 3, data_item[1]);
  ui->tableWidgetOutput->setItem(0, 4, data_item[2]);
  ui->tableWidgetOutput->setItem(0, 5, data_item[3]);
  ui->tableWidgetOutput->setItem(0, 6, data_item[4]);
  ui->tableWidgetOutput->setItem(0, 7, data_item[5]);
  ui->tableWidgetOutput->setItem(0, 8, data_item[6]);
  ui->tableWidgetOutput->setItem(0, 9, data_item[7]);
  ui->tableWidgetOutput->setItem(0, 10, time_item);
  ui->tableWidgetOutput->setCurrentCell(0,0);
}

//------------------------------------------------------------------------------
void MainWindow::sendMessage()
{
  int flags = 0;
  unsigned char data[8] = {ui->spinBoxData0->value(), ui->spinBoxData1->value(),
                           ui->spinBoxData2->value(), ui->spinBoxData3->value(),
                           ui->spinBoxData4->value(), ui->spinBoxData5->value(),
                           ui->spinBoxData6->value(), ui->spinBoxData7->value()};
  int dlc = ui->spinBoxDLC->value();
  int id = ui->spinBoxID->value();

  if (ui->checkBoxExtID->isChecked()) {
    flags = canMSG_EXT;
  } else {
    flags = canMSG_STD;
    id = id & 0x7FF;
  }

  canStatus status = canWrite(can_handle, id, data, dlc, flags);
  if (status != canOK) {
    qDebug() << QString("canWrite() failed, %1").arg(status);
  }
}


//------------------------------------------------------------------------------
void MainWindow::on_tableWidgetOutput_cellDoubleClicked(int row, int column)
{
  (void)row;
  (void)column;

  while (ui->tableWidgetOutput->rowCount() > 0) {
    ui->tableWidgetOutput->removeRow(0);
  }
}
