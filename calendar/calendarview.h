#ifndef CALENDARVIEW_H
#define CALENDARVIEW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include "util/dateutil.h"
#include "header.h"
#include "cell.h"

#define CELL_STYLE "QFrame { background-color: #FFFFFF; color: #000000; border: 1px outset #CCCCCC; }" \
                   "QFrame#holiday { background-color: #FFF5F5; }" \
                   "QLabel { border: none; font-size: 50px; padding: 5px; background-color:rgba(0,0,0,0); }" \
                   "QLabel#today { background-color: #FFFF88; color: #FF0000; border: 1px solid #FF0000; font-size: 80px;}" \
                   "QLabel#LunarDay { border: none; font-size: 16px; padding: 5px; background-color:rgba(0,0,0,0); }" \
                   "QFrame#selected { background-color: #EEEEFF; }" \
                   "QLabel#header { font-size: 40px; font-weight: bold; }" \
                   "QFrame#header { background-color: #DDDDFF; }" \
                   "QFrame#disabled { background-color: #EFEFEF; }"

#define CALENDAR_CELL_WIDTH 7
#define CALENDAR_CELL_LINES 6

class CalendarView : public QWidget
{
    Q_OBJECT
public:
    explicit CalendarView(QWidget *parent = nullptr);

private:

    QVBoxLayout *m_layoutRoot;

    Header *m_header;
    QGridLayout *m_layoutDays;
    Date currentDate;
    Date viewedDate;

    Cell *cells[CALENDAR_CELL_WIDTH * CALENDAR_CELL_LINES];

    void createNameCells();
    void createDayCells();


private:
    void update(Date date);

signals:

private slots:
    void onBackButton();
    void onNextButton();
};

#endif // CALENDARVIEW_H
