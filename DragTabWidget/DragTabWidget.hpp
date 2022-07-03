#ifndef DRAGTABWIDGET_HPP
#define DRAGTABWIDGET_HPP

#include <QObject>
#include <QTabWidget>
#include <QApplication>
#include <QMouseEvent>
#include <QPixmap>
#include <QMimeData>
#include <QPainter>
#include <QCursor>
#include <QDrag>
#include <QDebug>
#include "DragTabBar.hpp"
#include "TabDialog.hpp"
#include "TabPageWidget.hpp"

namespace FinTechUI{

class DragTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit DragTabWidget(QWidget *parent = NULL): QTabWidget(parent)
    {
        setAttribute(Qt::WA_StyledBackground);
        initTabBar();
        connect(this, SIGNAL(PopWindow(TabPageWidget*)), this, SLOT(OnShowTabDialog(TabPageWidget*)));
        connect(m_tabBar, SIGNAL(DragOut(int)), this, SLOT(OnDragOut(int)));
    }

    int addTabPage(TabPageWidget *page, const QString& title)
    {
        int index = -1;
        TabPageWidget* tabPage = static_cast<TabPageWidget*>(page);
        if(tabPage != NULL)
        {
            const int index = addTab(tabPage, title);
            tabPage->setTabLabel(title);
            setCurrentIndex(index);
        }
        if(!m_tabPages.contains(page))
        {
            m_tabPages.append(tabPage);
        }

        return index;
    }

    void removeTabPage(TabPageWidget *page)
    {
        if(page != NULL)
        {
            removeTab(indexOf(page));
        }
    }

    void takeTabPage(TabPageWidget *page)
    {
        if(page != NULL)
        {
            removeTab(indexOf(page));
        }
    }

    void removeTabPage(int index)
    {
        if(indexValid(index))
        {
            removeTab(index);
        }
    }

    void removeCurrentPage()
    {
        removeTab(currentIndex());
    }

    bool indexValid(int index) const
    {
        if(index < 0 || index >= this->count())
            return false;
        return true;
    }

    bool pageValid(TabPageWidget *page) const
    {
        if(page == NULL)
            return false;
        return indexValid(indexOf(page));
    }
protected:
    void initTabBar()
    {
        m_tabBar = new DragTabBar(this);
        setTabBar(m_tabBar);
    }

    void popPage(TabPageWidget *page)
    {
        TabPageWidget* tabPage = dynamic_cast<TabPageWidget*>(page);
        if(tabPage != NULL)
        {
            takeTabPage(tabPage);
            TabDialog* pop = new TabDialog();
            pop->setAttribute(Qt::WA_DeleteOnClose);
            pop->setContentWidget(tabPage);
            pop->setWindowTitle(tabPage->tabLabel());
            pop->resize(page->size());
            pop->move(QCursor::pos() - QPoint(10, 10));
            connect(pop, SIGNAL(DragInTabPage(QDialog*)), this, SLOT(OnDragInTabPage(QDialog*)));
            pop->show();
            pop->activateWindow();
            pop->setFocus();
        }

    }

signals:
    void PopWindow(TabPageWidget* page);
protected slots:
    void OnDragOut(int index)
    {
        if(!indexValid(index))
            return;
        if(count() <= 1)
            return;
        QWidget* tabPage = this->widget(index);
        if(tabPage == NULL || !m_tabPages.contains(tabPage))
            return;
        QMimeData *mime = new QMimeData;
        QDrag *drag = new QDrag(m_tabBar);
        QVariant data;
        data.setValue(tabPage);
        drag->setProperty("page", data);
        drag->setMimeData(mime);
        drag->setHotSpot(QPoint(10, 0));
        connect(drag, SIGNAL(destroyed(QObject*)), this, SLOT(OnDragRelease(QObject*)));
        drag->exec(Qt::MoveAction);
    }

    void OnShowTabDialog(TabPageWidget* page)
    {
        QPoint bar_point = m_tabBar->mapFromGlobal(QCursor::pos());
        if(!m_tabBar->contentsRect().contains(bar_point))
        {
            popPage(page);
        }
    }

    void OnDragRelease(QObject* obj)
    {
        if(obj != NULL)
        {
            QVariant data = obj->property("page");
            TabPageWidget* page = reinterpret_cast<TabPageWidget*>(data.value<QWidget*>());
            if(page != NULL)
            {
                emit PopWindow(page);
            }
        }
    }

    void OnDragInTabPage(QDialog* tabDialog)
    {
        TabDialog* tabPage = dynamic_cast<TabDialog*>(tabDialog);
        if(tabPage != NULL)
        {
            TabPageWidget* page = static_cast<TabPageWidget*>(tabPage->getContentWidget());
            addTabPage(page, page->tabLabel());
            tabDialog->disconnect();
            tabDialog->close();
        }
    }
protected:
    QList<QWidget*> m_tabPages;
    QTabBar* m_tabBar;
};
}
#endif // DRAGTABWIDGET_HPP
