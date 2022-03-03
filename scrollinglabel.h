#ifndef SCROLLINGLABEL_H
#define SCROLLINGLABEL_H


#include <QWidget>
#include <QStaticText>
#include <QTimer>


class ScrollingLabel : public QWidget
{
    Q_OBJECT

public:
    explicit ScrollingLabel(QWidget *parent = 0);
    void setText(QString const &text);

public slots:
    QString text() const;

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);

private:
    void updateText();
    QString _text;

    QStaticText staticText;
    int singleTextWidth;
    QSize wholeTextSize;

    int scrollPos;
    QImage buffer;
    QTimer timer;

private slots:
    void onUpdateText();
};

#endif // SCROLLINGLABEL_H
