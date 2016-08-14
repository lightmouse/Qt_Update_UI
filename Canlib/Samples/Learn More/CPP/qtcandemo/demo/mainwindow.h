#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "canlib.h"
#include "canreader.h"

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
  void refreshChannels();
  void setChannel(int ch);
  void setBitrate(int br);
  void goBusOn();
  void goBusOff();
  void sendMessage();
  void rxUpdate(CanMessage msg);

  void on_tableWidgetOutput_cellDoubleClicked(int row, int column);

private:
  Ui::MainWindow *ui;
  int selected_channel;
  int selected_bitrate;
  CanHandle can_handle;
  CanReader *rx_thread;
};

#endif // MAINWINDOW_H
