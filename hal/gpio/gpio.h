#ifndef Gpio_H
#define Gpio_H

#include <QObject>
#include <QFileSystemWatcher>
#include <QMap>

class Gpio : public QObject
{
    Q_OBJECT
public:
    explicit Gpio(QObject *parent = nullptr);

    bool pinInit(int pin, int direction);
    bool set(int pin, int v);
    int get(int pin);

    void addInterrupt(int pin, bool isRising);

private:
    QFileSystemWatcher *watcher;
    QMap<QString, bool> *interruptInfo;

private slots:
    void onFileChanged(QString);

signals:
    void gpioInterrupted(int);
};

#endif // Gpio_H
