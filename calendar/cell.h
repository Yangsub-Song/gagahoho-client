#ifndef CELL_H
#define CELL_H

#include <QFrame>
#include <QVBoxLayout>
#include <QLabel>

#define CELL_STYLE "QFrame { background-color: #FFFFFF; color: #000000; border: 1px outset #CCCCCC; }" \
                   "QFrame#holiday { background-color: #FFF5F5; }" \
                   "QLabel { border: none; font-size: 50px; padding: 5px; background-color:rgba(0,0,0,0); }" \
                   "QLabel#today { background-color: #FFFF88; color: #FF0000; border: 1px solid #FF0000; font-size: 80px;}" \
                   "QLabel#LunarDay { border: none; font-size: 16px; padding: 5px; background-color:rgba(0,0,0,0); }" \
                   "QFrame#selected { background-color: #EEEEFF; }" \
                   "QLabel#header { font-size: 40px; font-weight: bold; }" \
                   "QFrame#header { background-color: #DDDDFF; }" \
                   "QFrame#disabled { background-color: #EFEFEF; }"



class Cell : public QFrame
{
    Q_OBJECT

public:
    explicit Cell(QWidget *parent = nullptr);
    void clear();

    void setDayObjectName(QString name);
    void setDay(QString& day);
    void setLunar(QString& lunar);

private:
    QVBoxLayout *m_layoutRoot;
    QLabel *m_labelDay;
    QLabel *m_labelLunar;

signals:

};

#endif // CELL_H
