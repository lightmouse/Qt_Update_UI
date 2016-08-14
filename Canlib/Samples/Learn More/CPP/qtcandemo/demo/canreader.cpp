#include "canreader.h"
#include <QDebug>

//------------------------------------------------------------------------------
CanReader::CanReader(int ch)
{
  can_channel = ch;
}

//------------------------------------------------------------------------------
void CanReader::run()
{
  exec();
}

//------------------------------------------------------------------------------
int CanReader::exec()
{
  const int timeout = 10;
  is_running = true;
  qDebug() << QString("Thread: channel = %1, running ...").arg(can_channel);

  int open_flags = canOPEN_ACCEPT_VIRTUAL;
  handle = canOpenChannel(can_channel, open_flags);
  if (handle < canOK) {
    qDebug() << QString("Thread: canOpenChannel() failed, %1").arg(handle);
    return 1;
  }

  canStatus status = canBusOn(handle);
  if (status != canOK) {
    qDebug() << QString("Thread: canBusOn() failed, %1").arg(status);
    return 1;
  }

  CanMessage msg;
  while (is_running) {
    do {
      status = canReadWait(handle, &msg.id, &msg.data, &msg.dlc, &msg.flag, &msg.time, timeout);
      if (status == canOK) {
        emit gotRx(msg);
      }
    } while (status == canOK);
  }

  status = canBusOff(handle);
  if (status != canOK) {
    qDebug() << QString("Thread: canBusOff() failed, %1").arg(status);
  }

  return 0;
}

//------------------------------------------------------------------------------
void CanReader::stopRunning()
{
  is_running = false;
}
