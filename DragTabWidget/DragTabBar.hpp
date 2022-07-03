#ifndef DRAGTABBAR_HPP
#define DRAGTABBAR_HPP

#include <QObject>
#include <QTabBar>
#include <QApplication>
#include <QMouseEvent>
#include <QPainter>

namespace FinTechUI{

class DragTabBar : public QTabBar
{
    Q_OBJECT
public:
    explicit DragTabBar(QWidget* parent = NULL): QTabBar(parent)
    {
        setAttribute(Qt::WA_StyledBackground);
        setMovable(true);
    }
protected:
    void mousePressEvent(QMouseEvent* event)override
    {
        QTabBar::mousePressEvent(event);
        if(event->button() == Qt::LeftButton && currentIndex() >= 0)
        {
            m_bDrag = true;
        }
    }

    void mouseMoveEvent(QMouseEvent* event)override
    {
        QTabBar::mouseMoveEvent(event);
        if(m_bDrag && event->buttons())
        {
            if(!m_bDragOut && !contentsRect().contains(event->pos()))
            {
                m_bDragOut = true;
                emit DragOut(this->currentIndex());
            }
        }
    }

    void mouseReleaseEvent(QMouseEvent* event)override
    {
        m_bDrag = false;
        m_bDragOut = false;
        QTabBar::mouseReleaseEvent(event);
    }
signals:
    void DragOut(int index);
protected:
    bool m_bDrag;
    bool m_bDragOut;
};
}

#endif // DRAGTABBAR_HPP
