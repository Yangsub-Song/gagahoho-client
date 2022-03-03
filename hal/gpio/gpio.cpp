#include <QFile>
#include <QRegExp>
#include <QFileInfo>

#include "gpio.h"

Gpio::Gpio(QObject *parent) : QObject(parent)
{
    watcher = new QFileSystemWatcher(this);
    interruptInfo = new QMap<QString, bool>();
    connect(watcher, SIGNAL(fileChanged(QString)), this, SLOT(onFileChanged(QString)));
}

bool Gpio::pinInit(int pin, int direction) {
    QString filename = "/sys/class/gpio/export";
    QFile exportFile(filename);
    if (exportFile.open(QIODevice::WriteOnly)) {
        QString gpioNumber = QString::number(pin);
        exportFile.write(gpioNumber.toUtf8());
        exportFile.close();

        auto dirname = QStringLiteral("/sys/class/gpio/gpio%1/direction").arg(pin);
        QFile dirFile(dirname);

        if (dirFile.open(QIODevice::WriteOnly)) {
            if (direction == 0) {
                dirFile.write("in");
            } else {
                dirFile.write("out");
            }
            dirFile.close();
            return true;
        }
    }

    return false;
}

bool Gpio::set(int pin, int v) {
    auto pathname = QStringLiteral("/sys/class/gpio/gpio%1/value").arg(pin);
    QFile dirFile(pathname);
    if (dirFile.open(QIODevice::WriteOnly)) {
        if(v == 0) {
            dirFile.write("0");
        } else {
            dirFile.write("1");
        }
        dirFile.close();
        return true;
    }
    return false;
}

int Gpio::get(int pin) {
    int value = 0;
    auto pathname = QStringLiteral("/sys/class/gpio/gpio%1/value").arg(pin);
    QFile dirFile(pathname);

    if (dirFile.open(QIODevice::ReadOnly)) {
        value = dirFile.readAll().toInt();
        dirFile.close();
        return value;
    }

    return -1;
}

void Gpio::addInterrupt(int pin, bool isRising) {
    auto edgePath = QStringLiteral("/sys/class/gpio/gpio%1/edge").arg(pin);
    QFile edgeFile(edgePath);

    if (edgeFile.open(QIODevice::WriteOnly)) {
        if (isRising) {
            edgeFile.write("rising");
        } else {
            edgeFile.write("falling");
        }
        edgeFile.close();
    }

    auto valuePath = QStringLiteral("/sys/class/gpio/gpio%1/value").arg(pin);

    interruptInfo->insert(valuePath, isRising);
    watcher->addPath(valuePath);
}


void Gpio::onFileChanged(QString path) {
    bool rising;
    QRegExp regex("(\\d+)");

    if (interruptInfo->contains(path)) {
        rising = interruptInfo->value(path);

        if (regex.indexIn(path, 0)) {
            int pinNumber = regex.cap(1).toInt();
            int v = get(pinNumber);

            if (rising && v == 1)  {
                emit gpioInterrupted(pinNumber);
            } else if (!rising && v == 0) {
                emit gpioInterrupted(pinNumber);
            }
        }
    }

}

