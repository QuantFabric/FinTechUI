#ifndef XSORTFILTERPROXYMODEL_H
#define XSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QVariant>
#include <QModelIndex>
#include <QDateTime>
#include <QMap>
#include <QMetaType>

namespace FinTechUI
{

class XSortFilterProxyModel: public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit XSortFilterProxyModel(): QSortFilterProxyModel()
    {

    }

    void setRowFilter(const QMap<int, QStringList>& filter)
    {
        m_RowFilter = filter;
    }

    void setColumnFilter(const QMap<int, QStringList>& filter)
    {
        m_ColumnFilter = filter;
    }

    void setColumnType(int column, int type)
    {
        m_ColumnType[column] = type;
    }

    void resetFilter()
    {
        invalidateFilter();
    }

protected:
    bool lessThan (const QModelIndex& left, const QModelIndex& right) const
    {
        QVariant LeftItem, RightItem;
        LeftItem = left.data();
        RightItem = right.data();
        int Type = m_ColumnType.value(left.column(), QVariant::String);
        switch (Type)
        {
        case QVariant::Invalid:
            return (RightItem.type() == QVariant::Invalid);
        case QVariant::Date:
            if (LeftItem.toDate() <= RightItem.toDate())
                return true;
            else 
                return false;
            break;
        case QVariant::Time:
            if (LeftItem.toTime() <= RightItem.toTime())
                return true;
            else
                return false;
            break;
        case QVariant::DateTime:
            if (LeftItem.toDateTime() <= RightItem.toDateTime())
                return true;
            else
                return false;
            break;
        case QMetaType::Float:
            if (LeftItem.toFloat() <= RightItem.toFloat())
                return true;
            else
                return false;
            break;
        case QVariant::Double:
            if (LeftItem.toDouble() <= RightItem.toDouble())
                return true;
            else
                return false;
            break;
        case QVariant::Int:
            if (LeftItem.toInt() <= RightItem.toInt())
                return true;
            else
                return false;
            break;
        case QVariant::UInt:
            if (LeftItem.toUInt() <= RightItem.toUInt())
                return true;
            else
                return false;
            break;
        case QVariant::LongLong:
            if (LeftItem.toLongLong() <= RightItem.toLongLong())
                return true;
            else
                return false;
            break;
        case QVariant::ULongLong:
            if (LeftItem.toULongLong() <= RightItem.toULongLong())
                return true;
            else
                return false;
            break;
        case QVariant::Char:
            if (LeftItem.toChar() <= RightItem.toChar())
                return true;
            else
                return false;
            break;
        case QVariant::String:
            if (LeftItem.toString() <= RightItem.toString())
                return true;
            else
                return false;
            break;
        default:
            int ret = 0;
            if (isSortLocaleAware())
            {
                ret = LeftItem.toString().localeAwareCompare(RightItem.toString());
                if (ret <= 0)
                    return true;
                else
                    return false;
            }
            else
            {
                ret = LeftItem.toString().compare(RightItem.toString(), sortCaseSensitivity());
                if(ret <= 0)
                    return true;
                else
                    return false;
            }
        }
        return true;
    }

    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
    {
        bool ret = true;
        QList<int> columns = m_RowFilter.keys();
        QList<QModelIndex> values;
        foreach(int col, columns)
        {
            values.append(sourceModel()->index(source_row, col, source_parent));
        }
        foreach(QModelIndex index, values)
        {
            QStringList patterns = m_RowFilter.value(index.column());
            ret = ret && patterns.contains(sourceModel()->data(index).toString());
            if(!ret)
            {
                break;
            }
        }
        return ret;
    }

    virtual bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const
    {
        bool ret = true;
        QList<int> rows = m_ColumnFilter.keys();
        QList<QModelIndex> values;
        foreach(int row, rows)
        {
            values.append(sourceModel()->index(row, source_column, source_parent));
        }
        foreach(QModelIndex index, values)
        {
            QStringList patterns = m_ColumnFilter.value(index.row());
            ret = ret && patterns.contains(sourceModel()->data(index).toString());
            if(!ret)
            {
                break;
            }
        }
        return ret;
    }

protected:
    QMap<int, QStringList> m_RowFilter;
    QMap<int, QStringList> m_ColumnFilter;
    QMap<int, int> m_ColumnType;
};


}

#endif // XSORTFILTERPROXYMODEL_H
