#ifndef XTABLEMODEL_H
#define XTABLEMODEL_H

#include <QAbstractTableModel>
#include <QModelIndex>
#include <QVector>
#include "XTableModelItem.hpp"

namespace FinTechUI
{


class XTableModel: public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit XTableModel(QObject* parent = NULL)
    {

    }
    virtual ~XTableModel()
    {
        for(int i = 0; i < m_rows.size(); i++)
        {
            XTableModelRow* modelRow = m_rows.at(i);
            delete modelRow;
            modelRow = NULL;
        }
        m_rows.clear();
    }
    void setHeaderLabels(const QStringList& header)
    {
        m_header = header;
    }

    QStringList headerLabels() const
    {
        return m_header;
    }

    void setRowForegroundColor(int row, const QColor& color = QColor())
    {
        if(row >= 0 && row < m_rows.size())
        {
            XTableModelRow* modelRow = m_rows[row];
            if(modelRow != NULL)
            {
                int columns = modelRow->size();
                if(columns < m_header.size())
                {
                    for(int col = 0; col < columns; col++)
                    {
                        modelRow->at(col)->setForeground(color);
                    }
                }
            }
        }
    }

    static void setRowForegroundColor(XTableModelRow* modelRow, const QColor& color = QColor())
    {
        if(modelRow != NULL)
        {
            int columns = modelRow->size();
            for(int col = 0; col < columns; col++)
            {
                modelRow->at(col)->setForeground(color);
            }
        }
    }

    void setRowBackgroundColor(int row, const QColor& color = QColor())
    {
        if(row >= 0 && row < m_rows.size())
        {
            XTableModelRow* modelRow = m_rows[row];
            if(modelRow != NULL)
            {
                int columns = modelRow->size();
                if(columns < m_header.size())
                {
                    for(int col = 0; col < columns; col++)
                    {
                        modelRow->at(col)->setBackground(color);
                    }
                }
            }
        }
    }

    static void setRowBackgroundColor(XTableModelRow* modelRow, const QColor& color = QColor())
    {
        if(modelRow != NULL)
        {
            int columns = modelRow->size();
            for(int col = 0; col < columns; col++)
            {
                modelRow->at(col)->setBackground(color);
            }
        }
    }

    void appendRow(XTableModelRow* modelRow)
    {
        if(modelRow != NULL && checkRow(modelRow))
        {
            int row = m_rows.size();
            beginInsertRows(QModelIndex(), row, row);
            m_rows.append(modelRow);
            endInsertRows();
        }
    }

    void appendRows(const QList<XTableModelRow*> &modelRows)
    {
        int row = m_rows.size();
        int count = modelRows.size();
        if(count > 0)
        {
            beginInsertRows(QModelIndex(), row, row + count);
            foreach(XTableModelRow* modelRow, modelRows)
            {
                m_rows.append(modelRow);
            }
            endInsertRows();
        }
    }

    void insertRow(int row, XTableModelRow *modelRow)
    {
        if(row > 0 && row <= m_rows.size())
        {
            beginInsertRows(QModelIndex(), row, row);
            m_rows.insert(row, modelRow);
            endInsertRows();
        }
    }

    void insertRows(int row, const QList<XTableModelRow *>& modelRows)
    {
        int count = modelRows.size();
        if(row > 0 && row <= m_rows.size() && count > 0)
        {
            beginInsertRows(QModelIndex(), row, row + count);
            for(int i = 0; i < count; i++)
            {
                m_rows.insert(row + i, modelRows.at(i));
            }
            endInsertRows();
        }
    }

    void updateRow(XTableModelRow *modelRow)
    {
        int row = m_rows.indexOf(modelRow);
        if(row > -1)
        {
            memcpy(m_rows[row], modelRow, sizeof(*m_rows[row]));
            emit dataChanged(index(row, 0), index(row, m_header.size() - 1));
        }
    }

    void updateRows(const QList<XTableModelRow *> &modelRows)
    {
        foreach(XTableModelRow* modelRow, modelRows)
        {
            updateRow(modelRow);
        }
    }

    void deleteRow(XTableModelRow *modelRow, bool release)
    {
        if(modelRow != NULL)
        {
            int row = m_rows.indexOf(modelRow);
            if(row > -1)
            {
                beginRemoveRows(QModelIndex(), row, row);
                m_rows.remove(row);
                endRemoveRows();

            }
            if(release)
            {
                delete modelRow;
                modelRow = NULL;
            }
        }
    }

    void deleteRows(QList<XTableModelRow*> modelRows, bool release)
    {
        foreach(XTableModelRow* modelRow, modelRows)
        {
            deleteRow(modelRow, release);
        }
    }

    XTableModelRow* modelRow(unsigned int row)const
    {
        if(row < m_rows.size())
            return m_rows[row];
        else
            return NULL;
    }

    QString itemText(int row, int col)const
    {
        QString result;
        if(row < m_rows.size())
        {
            XTableModelRow* modelRow = m_rows.at(row);
            if(col < m_header.size())
            {
                XTableModelItem* item = modelRow->at(col);
                result = item->text();
            }
        }
        return result;
    }

    int row(XTableModelRow *modelRow) const
    {
        int row = m_rows.indexOf(modelRow);
        return row;
    }

    /**************************************************
     * Reimplemention function
     * ***********************************************/
    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const
    {
        if(!index.isValid())
        {
            return QVariant();
        }
        int row = index.row();
        int col = index.column();
        XTableModelRow* modelRow = m_rows.at(row);
        XTableModelItem* item = modelRow->at(col);
        switch (role) 
        {
        case Qt::DisplayRole:
            return item->text();
        case Qt::BackgroundRole:
            return item->background();
        case Qt::ForegroundRole:
            return item->foreground();
        case Qt::TextAlignmentRole:
            return item->textAlignment();
        default:
            return QVariant();
        }
    }

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const
    {
        Q_UNUSED(parent)
        return m_rows.size();
    }

    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const
    {
        Q_UNUSED(parent)
        return m_header.size();
    }

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const
    {
        if(section < 0 || orientation == Qt::Horizontal && section > m_header.size())
            return QVariant();
        if(role == Qt::DisplayRole)
        {
            return orientation == Qt::Horizontal ? m_header.at(section) : QString::number(section + 1);
        }
        return QVariant();
    }

    bool setData(const QModelIndex &index, const QVariant &value, int role)
    {
        if (index.isValid() && role == Qt::EditRole) 
        {   
            // 编辑完成,保存数据到model,并返回true
            XTableModelRow* ModelRow = m_rows[index.row()];
            (*ModelRow)[index.column()]->setText(value.toString());
            // 重新实现setData()函数时，必须显式发出信号。
            emit dataChanged(index, index);             
            return true;
        }
        return false;
    }

    Qt::ItemFlags flags(const QModelIndex& index) const
    {
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;       //设置item可编辑
    }

    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const
    {
        return createIndex(row, column);
    }
protected:
    bool checkRow(XTableModelRow* modelRow)
    {
        return true;
        bool result = false;
        if(modelRow != NULL)
        {
            result = modelRow->size() == m_header.size();
        }
        return result;
    }
protected:
    QVector<XTableModelRow*> m_rows;
    QStringList m_header;
};


}


#endif // XTABLEMODEL_H
