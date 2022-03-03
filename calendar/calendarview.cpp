#include "calendarview.h"

#include <QTextCodec>
#include <QLabel>


CalendarView::CalendarView(QWidget *parent) : QWidget(parent)
{
    m_layoutRoot = new QVBoxLayout(this);

    m_header = new Header();
    m_layoutDays = new QGridLayout();

    createNameCells();
    createDayCells();

    m_layoutRoot->addWidget(m_header);
    m_layoutRoot->addLayout(m_layoutDays);

    currentDate = DateUtil::get_current_date();
    viewedDate = DateUtil::get_current_date();

    setStyleSheet("border-style: solid; border-width: 0px; border-color: #000000");
    update(DateUtil::get_current_date());

    resize(1280, 800);

    connect(m_header, SIGNAL(onNextClicked()), this, SLOT(onNextButton()));
    connect(m_header, SIGNAL(onPreviousClicked()), this, SLOT(onBackButton()));
}

void CalendarView::createNameCells() {
    for (int i = 0; i < CALENDAR_CELL_WIDTH; i++) {
        QFrame *cell = new QFrame(this);
        QHBoxLayout *cellLayout = new QHBoxLayout();
        QLabel *name = new QLabel(
                    DateUtil::numeric2literal_day_of_week(i+1).c_str());

        cellLayout->addWidget(name);
        cell->setLayout(cellLayout);
        cell->setStyleSheet(CELL_STYLE);
        m_layoutDays->addWidget(cell, 0, i);
    }

    m_layoutDays->setHorizontalSpacing(0);
    m_layoutDays->setVerticalSpacing(0);
    m_layoutDays->setMargin(0);

}

void CalendarView::createDayCells() {

    for (int i = 1; i < CALENDAR_CELL_WIDTH; i++) {
        for (int j = 0; j < CALENDAR_CELL_WIDTH; j++) {
            Cell *c = new Cell(this);
            m_layoutDays->addWidget(c, i, j);
            this->cells[7*(i-1)+j] = c;
        }
    }
}

void CalendarView::update(Date date) {

    QString headerDate = QString::number(date.getYear()) +
            QString("년") + QString("    ") +
            QString(DateUtil::get_literal_month(date.getMonth()).c_str());

    Date currentDate = DateUtil::get_current_date();

    int daysOfMonth = DateUtil::get_days_in_month(date.getMonth(), date.getYear());
    int startOfWeekDay = DateUtil::get_first_day_of_month(date).getWeekDay();

    for(int i = 0, days = 1; i < CALENDAR_CELL_WIDTH * CALENDAR_CELL_LINES; i++) {
        cells[i]->clear();

        if ( ((i > 6 || startOfWeekDay == 6) || (i % 7 >= startOfWeekDay)) &&
           (days <= daysOfMonth)) {

            QString day = QString("%1").arg(days, 2, 10, QChar('0'));

            QDate d = DateUtil::solarToLunar(date.getYear(), date.getMonth(), days);
            QString LunarDsp = QString("%1.").arg(d.month(), 2, 10, QChar(' ')) + QString("%1").arg(d.day(), 2, 10, QChar('0'));
            QString LunarDayDsp = LunarDsp.right(2);
            LunarDsp += " " + DateUtil::GetTodaySpecial(date.getYear(), date.getMonth(), days);

            cells[i]->setDay(day);
            cells[i]->setLunar(LunarDsp);

            if ( ((i % 7)==0) || ((i % 7)==6) ){
                cells[i]->setObjectName("holiday");
            } else {
                cells[i]->setObjectName("enabled");
            }

            if (date.getYear() == currentDate.getYear() &&
                    date.getMonth() == currentDate.getMonth() &&
                    date.getMonthDay() == days) {
                cells[i]->setDayObjectName("today");
            }

            days += 1;
        } else {
            cells[i]->setObjectName("disabled");
        }
    }

    m_header->setDate(headerDate);
}

void CalendarView::onBackButton() {
    viewedDate = DateUtil::decrease_month(viewedDate);
    update(viewedDate);
}

void CalendarView::onNextButton() {
    viewedDate = DateUtil::increase_month(viewedDate);
    update(viewedDate);
}
