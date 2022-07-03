#ifndef XCOMBOBOXDELEGATE_HPP
#define XCOMBOBOXDELEGATE_HPP

#include <QStyledItemDelegate>
#include <QComboBox>

namespace FinTechUI
{
class XComboBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit XComboBoxDelegate(QObject* parent = NULL) : QStyledItemDelegate(parent)
    {

    }
    virtual ~XComboBoxDelegate()
    {

    }
public:
    virtual QWidget *createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        Q_UNUSED(option)
        QStringList items = index.model()->data(index, Qt::UserRole).toStringList();
        QComboBox* editor = new QComboBox(parent);
        editor->addItems(items);
        return editor;
    }

    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const
    {
        QString text = index.model()->data(index, Qt::DisplayRole).toString();
        QStringList items = index.model()->data(index, Qt::UserRole).toStringList();
        QComboBox* comboBox =  static_cast <QComboBox*>(editor);
        if(comboBox != NULL)
        {
            comboBox->setCurrentText(text);
        }
    }

    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
    {
        QComboBox *comboBox = static_cast<QComboBox*>(editor);
        if(comboBox != NULL)
        {
            model->setData(index, comboBox->currentText(), Qt::DisplayRole);
        }
    }

    virtual void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        editor->setGeometry(option.rect);
    }
};

}

#endif // XCOMBOBOXDELEGATE_HPP