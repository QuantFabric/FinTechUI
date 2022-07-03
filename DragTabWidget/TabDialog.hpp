#ifndef TABDIALOG_HPP
#define TABDIALOG_HPP

#include <QObject>
#include <QDialog>
#include <QEvent>
#include <QMouseEvent>
#include <QVBoxLayout>
#include "TabPageWidget.hpp"

namespace FinTechUI{
class TabDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TabDialog(QWidget* parent = NULL): QDialog(parent)
    {
        m_tabPage = NULL;
    }

    void setContentWidget(TabPageWidget* page)
    {
        TabPageWidget* tabPage = dynamic_cast<TabPageWidget*>(page);
        if(tabPage != NULL)
        {
            m_tabPage = tabPage;
            QVBoxLayout* layout = new QVBoxLayout();
            layout->setMargin(0);
            layout->addWidget(m_tabPage);
            setLayout(layout);
            setWindowTitle(tabPage->tabLabel());
            Qt::WindowFlags flags = Qt::Dialog;
            flags |= Qt::WindowMinMaxButtonsHint;
            flags |= Qt::WindowCloseButtonHint;
            setWindowFlags(flags);
            m_tabPage->show();
        }
    }

    QWidget* getContentWidget()const
    {
        return m_tabPage;
    }
protected:
    bool event(QEvent *event)
    {
        switch(event->type())
        {
        // close Dialog
        case QEvent::Close:
            // restore TabPage to TabWidget
            emit DragInTabPage(this);
            break;
        // double click title bar
        case QEvent::NonClientAreaMouseButtonDblClick:
            showMaximized();
            break;
        }
        return QDialog::event(event);
    }
    void keyPressEvent(QKeyEvent *event)
    {
        switch(event->type())
        {
        case Qt::Key_Escape:

            break;
        default:
            QDialog::keyPressEvent(event);
            break;
        }
    }
protected slots:
    void reject()
    {
        emit DragInTabPage(this);
        QDialog::reject();
    }
signals:
    void DragRelease(const QPoint& globalPos, QDialog*);
    void DragInTabPage(QDialog*);
private:
    TabPageWidget* m_tabPage;
};
}

#endif // TABDIALOG_HPP
