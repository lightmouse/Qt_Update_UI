#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include "canlib_class.h"

#include <QtCore/QFile>
#include <QtCore/QIODevice>
#include <QtCore/QTextStream>

#define MSG_CHANNEL  0
#define MSG_BAUDRATE 1
#define MSG_BUSON    2


Canlib *lib = new Canlib();

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    ui->comboBox_baudrate->setEnabled(false);
    ui->pushButton_ChooseFile->setEnabled(false);
//    ui->pushButton_StartCAN->setEnabled(false);
    ui->pushButton_CloseCAN->setEnabled(false);
    ui->pushButton_StartUpdate->setEnabled(false);

}

MainWindow::~MainWindow()
{
    delete ui;
}


int GetChannel(QString CanChannel)
{
    if (CanChannel == "0")
        return 0;
    if (CanChannel == "1")
        return 1;
    if (CanChannel == "2")
        return 2;
    if (CanChannel == "3")
        return 3;
    if (CanChannel == "4")
        return 4;
    if (CanChannel == "5")
        return 5;

    return -1;
}

int GetBaudRate(QString CanBaudRate)
{
    if (CanBaudRate == "10000")
        return canBITRATE_10K;
    if (CanBaudRate == "50000")
        return canBITRATE_50K;
    if (CanBaudRate == "100000")
        return canBITRATE_100K;
    if (CanBaudRate == "125000")
        return canBITRATE_125K;
    if (CanBaudRate == "250000")
        return canBITRATE_250K;
    if (CanBaudRate == "500000")
        return canBITRATE_500K;
    if (CanBaudRate == "1000000")
        return canBITRATE_1M;

    return 0;
}

int CheckStatus(canStatus status, QWidget *widget, int msg_num)
{
    char* can_msg[3] = {"CAN通道错误", "CAN波特率错误", "CAN总线打开错误"};

    if (status != canOK){
        QMessageBox::warning(widget, "error", can_msg[msg_num],
                             QMessageBox::Yes|QMessageBox::No,
                             QMessageBox::Yes);
        return -1;
    }
    return 0;
}

void MainWindow::on_pushButton_StartCAN_clicked()
{
    int channel = GetChannel(ui->comboBox_channel->currentText());
//    canStatus stat = lib->canOpenChannel(channel, 1);
//    if (CheckStatus(stat, this, MSG_CHANNEL) != 0)
//        return;

//    int BaudRate = GetBaudRate(ui->comboBox_baudrate->currentText());
//    stat = lib->canSetBusParams(BaudRate, 0, 0, 0, 0, 0);
//    if (CheckStatus(stat, this, MSG_BAUDRATE) != 0)
//        return;

//    stat = lib->canBusOn();
//    if (CheckStatus(stat, this, MSG_BUSON) != 0)
//        return;


    ui->pushButton_StartCAN->setEnabled(false);
    ui->pushButton_ChooseFile->setEnabled(true);
    ui->pushButton_CloseCAN->setEnabled(true);
    ui->pushButton_StartUpdate->setEnabled(true);
}

void MainWindow::on_pushButton_CloseCAN_clicked()
{
    ui->pushButton_CloseCAN->setEnabled(false);
    ui->pushButton_ChooseFile->setEnabled(false);
    ui->pushButton_StartCAN->setEnabled(true);
    ui->pushButton_StartUpdate->setEnabled(false);

    delete(lib);
}

void MainWindow::on_pushButton_ChooseFile_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this);

    if (!filename.isEmpty()){
        QFile file(filename);
        if (!file.open(QFile::ReadWrite | QFile::Text)){
            QMessageBox::warning(this, tr("打开文件"), tr("打开文件失败"),QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
            return;//%1和%2表示后面两个arg的值
        }
        QTextStream in(&file);//新建流对象，指向选定文件
        QString curFile = QFileInfo(filename).canonicalFilePath();
        ui->lineEdit_FilePath->setText(curFile);
        ui->lineEdit_FilePath->setVisible(true);
    }
}
