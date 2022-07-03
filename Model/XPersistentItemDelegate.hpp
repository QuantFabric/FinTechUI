#ifndef XPERSISTENTITEMDELEGATE_HPP
#define XPERSISTENTITEMDELEGATE_HPP

#include <QObject>
#include <QStyledItemDelegate>
#include <QWidget>
#include <QMap>
#include <QPersistentModelIndex>
#include <QAbstractItemModel>
#include <QPainter>
#include <QItemSelection>
#include <QModelIndex>
#include <QHBoxLayout>

/* 持久代理类
 * 1、View需要将EditTrigger设置为QAbstractItemView::NoEditTriggers,关闭默认的Delegate逻辑
 * 2、继承这个类,实现自定义的 持久代理
 *
 *  继承时,不能也不必重写paint、updateEditorGeometry函数,
 *      前者是因为paint本应该做的绘制工作已没有意义,被编辑器覆盖,绘制了也看不到,
 *              且在本类中用paint实现了根本逻辑,不能被覆盖。
 *      后者是因为本类的paint中会一直重设编辑器的尺寸
 *  其他函数可随需要重写,不做要求。
 *
 * 3、如果需要将数据从编辑器保存到model中,那么需要连接信号槽 到本类的updateModelData,它会调用子类的
 *
 *  void QStyledItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &amp;index) const
 *
 * 因此,此时我们也需要实现这个函数。这也是最重要的额外步骤。
*/

namespace FinTechUI
{
class XPersistentItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit XPersistentItemDelegate(QObject *parent = NULL) : QStyledItemDelegate(parent)
    {

    }

    virtual ~XPersistentItemDelegate()
    {

    }

    virtual void paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
		QPersistentModelIndex perIndex(index);
		QAbstractItemModel* model = const_cast<QAbstractItemModel*>(index.model());
		//如果model有删除、重置,则删除对应的widget
		//如果model有新增,view会调用paint绘制,因此不需做额外工作
		connect(model, SIGNAL(rowsRemoved(QModelIndex, int, int)), this, SLOT(clearWidget()), Qt::UniqueConnection);
		connect(model, SIGNAL(columnsRemoved(QModelIndex, int, int)), this, SLOT(clearWidget()), Qt::UniqueConnection);
		connect(model, SIGNAL(destroyed(QObject*)), this, SLOT(clearWidget()), Qt::UniqueConnection);
		connect(model, SIGNAL(modelReset()), this, SLOT(clearWidget()), Qt::UniqueConnection);
		//如果model有数据变化,更新变化到编辑器
		connect(model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>& )), this, SLOT(updateWidget(const QModelIndex&, const QModelIndex&)));

		if(!m_WidgetsMap.contains(perIndex))
		{
			QWidget* parentWidget = static_cast<QWidget*>(painter->device());
			if(NULL == parentWidget)
				return;
			QWidget* tempWidget = this->createEditor(parentWidget, option, index);
			this->setEditorData(tempWidget, index);
			tempWidget->setGeometry(option.rect);
			tempWidget->setVisible(true);
			m_WidgetsMap.insert(perIndex, tempWidget);
		}
		else
		{
			QWidget* tempWidget = m_WidgetsMap.value(perIndex);
			if(tempWidget != NULL)
			{
				tempWidget->setGeometry(option.rect);
			}
		}
	}

public slots:
    void updateModelData()
    {
		QObject *sender = this->sender();
		if(NULL != sender)
			return;
		QWidget* editor = static_cast<QWidget*>(sender);
		if(NULL != editor)
			return;

		if(!m_WidgetsMap.values().contains(editor))
			return;

		QPersistentModelIndex perIndex = m_WidgetsMap.key(editor);
		if(!perIndex.isValid())
			return;

		QModelIndex index = static_cast<QModelIndex>(perIndex);
		if(!index.isValid())
			return;

		QAbstractItemModel* model = const_cast<QAbstractItemModel*>(index.model());
		this->setModelData(editor,model,index);

		emit model->dataChanged(index,index);
	}
private slots:
    void clearWidget()
    {
		auto it = m_WidgetsMap.begin();
		while (it != m_WidgetsMap.end())
		{
			if(!it.key().isValid())
			{
				it.value()->setParent(NULL);
				it.value()->deleteLater();
				it = m_WidgetsMap.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
    void updateWidget(const QModelIndex& begin, const QModelIndex& end)
    {
        QItemSelection selection(begin, end);
        QModelIndexList list = selection.indexes();

		foreach (QModelIndex index, list)
		{
			QPersistentModelIndex perIndex(index);
			if(m_WidgetsMap.contains(perIndex))
			{
				QWidget *tempWidget = m_WidgetsMap.value(perIndex);
				if(tempWidget)
				{
					this->setEditorData(tempWidget,index);
				}
			}
		}
	}
private:
    mutable QMap<QPersistentModelIndex, QWidget*> m_WidgetsMap;
};

}


#endif // XPERSISTENTITEMDELEGATE_HPP
