#ifndef XBUTTONDELEGATE_HPP
#define XBUTTONDELEGATE_HPP

#include <QObject>
#include <QPushButton>
#include <QPersistentModelIndex>
#include <QStyleOptionViewItem>
#include <QWidget>
#include <QString>
#include "XPersistentItemDelegate.hpp"

namespace FinTechUI
{

class XButtonDelegate : public XPersistentItemDelegate
{
    Q_OBJECT
public:
    explicit XButtonDelegate(QString text, QObject *parent = NULL): XPersistentItemDelegate(parent)
    {
        m_Text = text;
    }

    virtual ~XButtonDelegate()
    {

    }
public:
    // 因为按钮既不需要从model读取数据，也不需要写入，因此仅需要重写一个函数即可
    virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        QPersistentModelIndex perIndex(index);
        QWidget *widget = new QWidget(parent);
        widget->setAutoFillBackground(true);
        QHBoxLayout *layout = new QHBoxLayout;
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setMargin(0);
        layout->addStretch();
        QPushButton* button = new QPushButton(m_Text);
        button->setMinimumHeight(25);
        layout->addWidget(button);
        layout->addStretch();
        widget->setLayout(layout);
        QObject::connect(button, &QPushButton::clicked, [=]{
                QModelIndex Index = static_cast<QModelIndex>(perIndex);
                //const成员里，不能修改对象，因此不能emit信号
                auto temp = const_cast<XButtonDelegate*>(this);
                emit temp->Clicked(Index);
        });

	return widget;
	}

signals:
    void Clicked(const QModelIndex index);
protected:
    QString m_Text;
};

}

#endif // XBUTTONDELEGATE_HPP
