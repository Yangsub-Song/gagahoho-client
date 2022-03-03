#ifndef LINUX_H
#define LINUX_H

#include <QObject>

class Linux : public QObject
{
    Q_OBJECT
public:
    explicit Linux(QObject *parent = nullptr);

signals:

public slots:
    void onSystemOff();
    void onTimeSync(long sec, long usec);
};

#endif // LINUX_H
