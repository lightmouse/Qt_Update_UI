#ifndef CANREADER_H
#define CANREADER_H

#include <QThread>
#include <QList>

#include "canlib.h"

typedef struct {
  unsigned char data[8];
  unsigned int dlc;
  unsigned int flag;
  unsigned long time;
  long id;
} CanMessage;

class CanReader : public QThread
{
  Q_OBJECT

private:
  CanHandle handle;
  CanReader() {};
  int exec();
  bool is_running;
  int can_channel;

public:
  CanReader(int ch);
  void run();
  void stopRunning();

signals:
  void gotRx(CanMessage msg);
};

#endif // CANREADER_H
