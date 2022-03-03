#include <unistd.h>
#include <linux/reboot.h>
#include <sys/reboot.h>
#include <sys/time.h>
#include <stdio.h>
#include <QDebug>

#include "linux.h"

Linux::Linux(QObject *parent) : QObject(parent)
{

}

void Linux::onSystemOff() {
    sync();
    reboot(LINUX_REBOOT_CMD_POWER_OFF);
}

void Linux::onTimeSync(long sec, long usec) {
    int rc;
    struct timeval t = { 0, 0 };

    t.tv_sec = sec;
    t.tv_usec = usec;

    rc = settimeofday(&t, NULL);
    qDebug() << "settimeofday : " << QString::number(rc);
}
