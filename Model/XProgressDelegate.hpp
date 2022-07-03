#ifndef XPROGRESSDELEGATE_HPP
#define XPROGRESSDELEGATE_HPP

#include <QStyledItemDelegate>
#include <QProgressBar>
#include <QApplication>

namespace FinTechUI
{

class XProgressDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit XProgressDelegate(QObject* parent = NULL) : QStyledItemDelegate(parent)
    {

    }

    virtual ~XProgressDelegate()
    {

    }
protected:
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        QStyleOptionViewItem viewOption(option);
        initStyleOption(&viewOption, index);
        if (option.state.testFlag(QStyle::State_HasFocus))
            viewOption.state = viewOption.state ^ QStyle::State_HasFocus;

        QStyledItemDelegate::paint(painter, viewOption, index);
        int nProgress = index.model()->data(index, Qt::DisplayRole).toInt();
        int nLeft = 0;
        int nTop = 0;
        int nWidth = option.rect.width() - 2 * nLeft;
        int nHeight = option.rect.height() - 2 * nTop;
        // 设置进度条风格
        QStyleOptionProgressBar progressBarOption;
        progressBarOption.initFrom(option.widget);
        // 进度条显示区域
        progressBarOption.rect = QRect(option.rect.left() + nLeft, option.rect.top() + nTop,  nWidth, nHeight);
        // 设置最小值
        progressBarOption.minimum = 0;
        // 设置最大值
        progressBarOption.maximum = 100;
        // 设置对齐方式
        progressBarOption.textAlignment = Qt::AlignCenter;
        // 设置进度
        progressBarOption.progress = nProgress;
        // 设置文本（百分比）
        progressBarOption.text = QString("%1%").arg(nProgress);
        // 设置文本可见
        progressBarOption.textVisible = true;
        QProgressBar progressBar;
        //绘制进度条
        QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarOption, painter, &progressBar);
    }

    virtual QWidget *createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }

    virtual void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        editor->setGeometry(option.rect);
    }

    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }

    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
};

}


#endif // XPROGRESSDELEGATE_HPP