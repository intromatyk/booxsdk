
#include <QPainter>
#include <QKeyEvent>
#include <QLabel>
#include <QMouseEvent>
#include "onyx/screen/screen_update_watcher.h"
#include "onyx/ui/ui_utils.h"
#include "onyx/ui/content_view.h"

namespace ui
{

static QPoint s_mouse;
static const int MARGIN = 4;

ContentView::ContentView(QWidget *parent)
        : QWidget(parent)
        , data_(0)
        , pressed_(false)
        , checked_(false)
        , pen_width_(3)
{
    setFocusPolicy(Qt::StrongFocus);
    setAutoFillBackground(false);
}

ContentView::~ContentView()
{
}

void ContentView::setChecked(bool checked)
{
    checked_ = checked;
}

bool ContentView::isChecked()
{
    return checked_;
}

bool ContentView::updateData(OData* data, bool force)
{
    if (data_ == data && !force)
    {
        return false;
    }
    data_ = data;
    updateView();
    update();
    return true;
}

OData * ContentView::data()
{
    return data_;
}

bool ContentView::isPressed()
{
    return pressed_;
}

void ContentView::setPressed(bool p)
{
    pressed_ = p;
}

void ContentView::activate(int user_data)
{
    if (data())
    {
        emit activated(this, user_data);
    }
}

void ContentView::repaintAndRefreshScreen()
{
    update();
    onyx::screen::watcher().enqueue(this, onyx::screen::ScreenProxy::DW);
}

void ContentView::mousePressEvent(QMouseEvent *event)
{
    s_mouse = event->globalPos();
    if (data())
    {
        setPressed(true);
        repaintAndRefreshScreen();
    }
    QWidget::mousePressEvent(event);
}

void ContentView::mouseReleaseEvent(QMouseEvent *event)
{
    if (isPressed())
    {
        activate();
    }
    else
    {
        emit mouse(s_mouse, event->globalPos());
    }
    setPressed(false);
    if (data())
    {
        repaintAndRefreshScreen();
    }
    QWidget::mouseReleaseEvent(event);
}

void ContentView::mouseMoveEvent(QMouseEvent * e)
{
    if (isPressed() && !rect().contains(e->pos()) && data())
    {
        setPressed(false);
        repaintAndRefreshScreen();
    }
}

void ContentView::keyReleaseEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Return)
    {
        activate();
        e->accept();
        return;
    }
    e->ignore();
    emit keyRelease(this, e);
}

void ContentView::changeEvent(QEvent *event)
{
}

void ContentView::resizeEvent(QResizeEvent * event)
{
    QWidget::resizeEvent(event);
}

bool ContentView::event(QEvent * e)
{
    switch (e->type())
    {
    case QEvent::HoverMove:
    case QEvent::HoverEnter:
    case QEvent::HoverLeave:
        e->accept();
        return true;
    default:
        break;
    }
    return QWidget::event(e);
}

void ContentView::focusInEvent(QFocusEvent * e)
{
    QWidget::focusInEvent(e);
    onyx::screen::watcher().enqueue(this, onyx::screen::ScreenProxy::DW);
}

void ContentView::focusOutEvent(QFocusEvent * e)
{
    QWidget::focusOutEvent(e);
    onyx::screen::watcher().enqueue(this, onyx::screen::ScreenProxy::DW);
}

void ContentView::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::white);

    if (data())
    {
        if (isPressed())
        {
            //painter.fillRect(rect(), Qt::darkGray);
        }
        if (hasFocus())
        {
            QPen pen;
            pen.setWidth(penWidth());
            painter.setPen(pen);
            painter.drawRoundedRect(rect().adjusted(0, 0, -penWidth() , -penWidth()), 5, 5);
        }
    }
}




CheckBoxView::CheckBoxView(QWidget *parent)
: ContentView(parent)
{
}

CheckBoxView::~CheckBoxView()
{
}

void CheckBoxView::updateView()
{
    update();
}

void CheckBoxView::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::white);

    if (data())
    {
        if (isPressed() || isChecked())
        {
            painter.fillRect(rect().adjusted(penWidth(), penWidth(), -penWidth() - 1, -penWidth() - 1), Qt::gray);
        }
        if (hasFocus())
        {
            QPen pen;
            pen.setWidth(penWidth());
            painter.setPen(pen);
            painter.drawRoundedRect(rect().adjusted(0, 0, -penWidth() , -penWidth()), 5, 5);
        }

        drawCover(painter, rect());
        if (isPressed() || isChecked())
        {
            painter.setPen(Qt::white);
        }
        drawTitle(painter, rect());
    }
}

void CheckBoxView::drawCover(QPainter & painter, QRect rect)
{
    if (data() && data()->contains("cover"))
    {
        QPixmap pixmap(qVariantValue<QPixmap>(data()->value("cover")));
        painter.drawPixmap(MARGIN, (rect.height() - pixmap.height()) / 2, pixmap);
    }
}

void CheckBoxView::drawTitle(QPainter & painter, QRect rect)
{
    if (data() && data()->contains("title"))
    {
        rect.adjust(30, 0, 0, 0);
        QFont font;
        font.setPointSize(ui::defaultFontPointSize());
        painter.setFont(font);
        painter.drawText(rect, Qt::AlignCenter, data()->value("title").toString());
    }
}

}
