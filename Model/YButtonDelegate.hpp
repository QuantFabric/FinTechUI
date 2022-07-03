#ifndef YBUTTONDELEGATE_HPP
#define YBUTTONDELEGATE_HPP

#include <QStyledItemDelegate>
#include <QApplication>
#include <QMouseEvent>
#include <QDialog>
#include <QPainter>
#include <QBrush>
#include <QColor>
#include <QStyleOption>

namespace FinTechUI
{

class YButtonDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit YButtonDelegate(QString Text, QObject *parent = NULL) : QStyledItemDelegate(parent)
    {
        m_Text = Text;
    }
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        QStyleOptionButton* button = m_OptionButtonMap.value(index);
        if (button == NULL)
        {
            button = new QStyleOptionButton();
            button->rect = option.rect.adjusted(1, 1, -1, -1);
            button->text = m_Text;
            button->state |= QStyle::State_Enabled;
            (const_cast<YButtonDelegate*>(this))->m_OptionButtonMap.insert(index, button);
        }
        
        painter->save();
        if (option.state & QStyle::State_Selected)
        {
            painter->fillRect(option.rect, option.palette.highlight());
        }
        painter->restore();
        painter->setBackground(QBrush(QColor("#FFA500")));
        const_cast<QStyleOptionViewItem&>(option).backgroundBrush = QBrush(QColor("#FFA500"));
        QApplication::style()->drawControl(QStyle::CE_PushButton, button, painter);
    }

    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent* e =(QMouseEvent*)event;
            if (option.rect.adjusted(1, 1, -1, -1).contains(e->x(), e->y()) && m_OptionButtonMap.contains(index))
            {
                m_OptionButtonMap.value(index)->state |= QStyle::State_Sunken;
            }
        }
        if (event->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent* e =(QMouseEvent*)event;
            if (option.rect.adjusted(1, 1, -1, -1).contains(e->x(), e->y()) && m_OptionButtonMap.contains(index))
            {
                m_OptionButtonMap.value(index)->state &= (~QStyle::State_Sunken);
                emit Clicked(index);
            }
        }
        return true;
    }
signals:
    void Clicked(const QModelIndex& index);
protected:
    QMap<QModelIndex, QStyleOptionButton*> m_OptionButtonMap;
    QString m_Text;
};

}

#endif // YBUTTONDELEGATE_HPP
