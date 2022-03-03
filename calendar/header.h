#ifndef CALENDARHEADER_H
#define CALENDARHEADER_H

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

#define LABELDATE_STYLE "QLabel { color: #000000; qproperty-alignment: AlignCenter; padding-left: 100px; padding-right: 100px; font-size: 20px; }"

class Header : public QWidget
{
    Q_OBJECT
public:
    explicit Header(QWidget *parent = nullptr);

    void setDate(QString &date);

private:
    QHBoxLayout *m_rootLayout;
    QPushButton *m_buttonBack;
    QPushButton *m_buttonNext;
    QLabel *m_labelDate;

signals:
    void onPreviousClicked();
    void onNextClicked();
};

#endif // CALENDARHEADER_H
