#include "cell.h"

#include <QStyle>

Cell::Cell(QWidget *parent) : QFrame(parent)
{
    m_layoutRoot = new QVBoxLayout(this);
    m_layoutRoot->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_layoutRoot->setMargin(0);
    m_layoutRoot->setSpacing(0);

    m_labelDay = new QLabel(this);
    m_labelLunar = new QLabel(this);

    m_labelDay->setMaximumWidth(160);
    m_labelDay->setFixedWidth(160);

    m_labelLunar->setObjectName("LunarDay");

    m_layoutRoot->addWidget(m_labelDay);
    m_layoutRoot->addWidget(m_labelLunar);

    setMinimumWidth(150);
    setMinimumHeight(60);
    setLayout(m_layoutRoot);
    setStyleSheet(CELL_STYLE);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void Cell::setDayObjectName(QString name) {
    m_labelDay->setObjectName(name);
    style()->polish(this);
    style()->polish(m_labelDay);
}

void Cell::setDay(QString& day) {
    m_labelDay->setText(day);
}

void Cell::setLunar(QString& lunar) {
    m_labelLunar->setText(lunar);
}

void Cell::clear() {
    m_labelDay->setText("");
    m_labelLunar->setText("");
    m_labelDay->setObjectName("");
    setObjectName("");

    style()->polish(this);
    style()->polish(m_labelDay);
}
