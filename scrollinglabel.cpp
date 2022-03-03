#include "scrollinglabel.h"
#include <QPainter>

ScrollingLabel::ScrollingLabel(QWidget *parent) :
    QWidget(parent), scrollPos(0)
{
    staticText.setTextFormat(Qt::PlainText);
    connect(&timer, SIGNAL(timeout()), this, SLOT(onUpdateText()));
    timer.setInterval(50);
}

QString ScrollingLabel::text() const
{
    return _text;
}

void ScrollingLabel::setText(QString const &text)
{
    _text = text;
    updateText();
    update();
}

void ScrollingLabel::updateText()
{
    timer.stop();
    singleTextWidth = fontMetrics().width(_text);
    scrollPos = -64;
    staticText.setText(_text + "   ");
    timer.start();
    staticText.prepare(QTransform(), font());
    wholeTextSize = QSize(fontMetrics().width(staticText.text()), fontMetrics().height());
}

void ScrollingLabel::paintEvent(QPaintEvent*)
{
    QPainter p(this);

    buffer.fill(qRgba(0, 80, 119, 255));
    QPainter pb(&buffer);
    pb.setPen(p.pen());
    pb.setFont(p.font());

    int x = qMin(-scrollPos, 0);
    while(x < width())
    {
        pb.drawStaticText(QPointF(x, (height() - wholeTextSize.height()) / 2) /*+ QPoint(2, 2)*/, staticText);
        x += wholeTextSize.width();
    }
    p.drawImage(0, 0, buffer);
}

void ScrollingLabel::resizeEvent(QResizeEvent*)
{
    buffer = QImage(size(), QImage::Format_ARGB32_Premultiplied);
}

void ScrollingLabel::onUpdateText()
{
    scrollPos = (scrollPos + 2)
            % wholeTextSize.width();
    update();
}
