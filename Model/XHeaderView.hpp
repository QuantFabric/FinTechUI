#ifndef XHEADERVIEW_HPP
#define XHEADERVIEW_HPP

#include <QHeaderView>
#include <QAbstractTableModel>
#include <QModelIndex>
#include <QHash>
#include <QPair>
#include <QPainter>
#include <QMouseEvent>
#include <QStandardItem>
#include <qdrawutil.h>

namespace FinTechUI{

#define NAME    "name"
#define ITEMS   "items"

enum EHeaderRole
{
    COLUMN_SPAN_ROLE = Qt::UserRole + 1,
    ROW_SPAN_ROLE,
    COLUMN_SIZE_ROLE,
    ROW_SIZE_ROLE,
};

class XHeaderItem
{
public:
    XHeaderItem(XHeaderItem* parent = 0) : m_ParentItem(parent)
    {
        m_Row = 0;
        m_Column = 0;
    }

    XHeaderItem(int row, int column, XHeaderItem* parent = NULL) : m_ParentItem(parent)
    {
        m_Row = row;
        m_Column = column;
    }

    ~XHeaderItem()
    {

    }

    XHeaderItem* insertChild(int row, int col)
    {
        XHeaderItem* newChild = new XHeaderItem(row, col, this);
        m_ChildItems.insert(QPair<int,int>(row, col), newChild);
        return newChild;
    }

    const XHeaderItem* child(int row, int col) const
    {
        QHash<QPair<int,int>, XHeaderItem*>::const_iterator itr = m_ChildItems.find(QPair<int,int>(row, col));
        if (itr != m_ChildItems.end())
            return itr.value();
        return NULL;
    }

    XHeaderItem* child(int row, int col)
    {
        QHash<QPair<int,int>, XHeaderItem*>::iterator itr = m_ChildItems.find(QPair<int, int>(row, col));
        if (itr != m_ChildItems.end())
            return itr.value();
        return NULL;
    }

    void setData(const QVariant& data, int role)
    {
        m_RoleDatas.insert(role, data);
    }

    QVariant data(int role = Qt::UserRole+1) const
    {
        QHash<int,QVariant>::const_iterator itr = m_RoleDatas.find(role);
        if (itr != m_RoleDatas.end())
            return itr.value();
        return QVariant();
    }

    inline int column() const
    {
        return m_Column;
    }

    inline int row() const
    {
        return m_Row;
    }

    XHeaderItem* parent()
    {
        return m_ParentItem;
    }

    void setText(const QString& text)
    {
        m_RoleDatas.insert(Qt::DisplayRole, text);
    }

    void clear()
    {
        QList<XHeaderItem*> items = m_ChildItems.values();
        foreach(XHeaderItem* item, m_ChildItems)
        {
            if(item)
            {
                delete item;
            }
        }
        m_ChildItems.clear();
    }

private:
    int m_Row;
    int m_Column;
    XHeaderItem* m_ParentItem;
    QHash<QPair<int,int>, XHeaderItem*> m_ChildItems;
    QHash<int, QVariant> m_RoleDatas;
};

class XHeaderModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    XHeaderModel(int rows, int cols, QObject* parent = NULL) : QAbstractTableModel(parent)
    {
        m_RowCount = rows;
        m_ColumnCount = cols;
        m_RootItem = new XHeaderItem();
    }

    virtual ~XHeaderModel()
    {
        m_RootItem->clear();
        delete m_RootItem;
    }

public:
    virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const
    {
        if (!hasIndex(row,column,parent))
            return QModelIndex();

        XHeaderItem* parentItem;
        if (!parent.isValid())
            parentItem = m_RootItem; // parent item is always the root_item on table model
        else
            parentItem = static_cast<XHeaderItem*>(parent.internalPointer()); // no effect

        XHeaderItem* childItem = parentItem->child(row, column);
        if (!childItem)
            childItem = parentItem->insertChild(row,column);
            return createIndex(row,column,childItem);
        return QModelIndex();
    }

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const
    {
        Q_UNUSED(parent);
        return m_RowCount;
    }

    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const
    {
        Q_UNUSED(parent);
        return m_ColumnCount;
    }

    virtual QVariant data(const QModelIndex &index, int role) const
    {
        if (!index.isValid())
            return QVariant();

        if (index.row() >= m_RowCount || index.row() < 0 || index.column() >= m_ColumnCount || index.column() < 0)
            return QVariant();

        XHeaderItem* item = static_cast<XHeaderItem*>(index.internalPointer());

        return item->data(role);
    }

    virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole)
    {
        if (index.isValid())
        {
             XHeaderItem* item = static_cast<XHeaderItem*>(index.internalPointer());
             if (role == COLUMN_SPAN_ROLE)
             {
                 int col = index.column();
                 int span = value.toInt();
                 if (span > 0) // span size should be more than 1, else nothing to do
                 {
                     if (col + span - 1 >= m_ColumnCount) // span size should be less than whole columns,
                         span = m_ColumnCount - col;
                     item->setData(span, COLUMN_SPAN_ROLE);
                 }
             }
             else if (role == ROW_SPAN_ROLE)
             {
                 int row = index.row();
                 int span = value.toInt();
                 if (span > 0) // span size should be more than 1, else nothing to do
                 {
                     if (row + span - 1 >= m_RowCount)
                         span = m_RowCount - row;
                     item->setData(span, ROW_SPAN_ROLE);
                 }
             }
             else
             {
                 item->setData(value, role);
             }
            return true;
        }
        return false;
    }

    virtual Qt::ItemFlags flags(const QModelIndex &index) const
    {
        if (!index.isValid())
            return Qt::ItemIsEnabled;
        return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
    }
private:
    int m_RowCount;
    int m_ColumnCount;
    XHeaderItem* m_RootItem;
};

class XHeaderView : public QHeaderView
{
    Q_OBJECT
public:
    explicit XHeaderView(const QList<QVariantMap>& headerData, Qt::Orientation orientation = Qt::Horizontal, QWidget* parent = 0) : QHeaderView(orientation, parent)
    {
        int rows = 2;
        QSize baseSectionSize;
        if (orientation == Qt::Horizontal)
        {
            baseSectionSize.setWidth(defaultSectionSize());
            baseSectionSize.setHeight(25);
        }
        else
        {
            baseSectionSize.setWidth(50);
            baseSectionSize.setHeight(defaultSectionSize());
        }
        int columns = 0;
        foreach(QVariantMap data, headerData)
        {
            columns += data[ITEMS].toStringList().size();
        }

        // create header model
        XHeaderModel* headerModel = new XHeaderModel(rows, columns);

        // set default size of item
        for (int row = 0; row < rows; ++row)
        {
            for (int col = 0; col < columns; ++col)
            {
                headerModel->setData(headerModel->index(row, col), baseSectionSize, Qt::SizeHintRole);
            }
        }
        setModel(headerModel);
        // set Header Model Data
        columns = 0;
        for (int i = 0; i < headerData.size(); i++)
        {
            QVariantMap data = headerData.at(i);
            QString name = data[NAME].toString();
            QStringList items = data[ITEMS].toStringList();
            // firt row
            if(items.size() > 1)
            {
                setSpan(0, columns, 0, items.size());
            }
            else
            {
                setSpan(0, columns, 2, 0);
            }
            headerModel->setData(headerModel->index(0, columns), name, Qt::DisplayRole);
            // second row
            for(int j = 0; j < items.size(); j++)
            {
                headerModel->setData(headerModel->index(1, columns + j), items.at(j), Qt::DisplayRole);
            }
            columns += items.size();
        }
        connect(this, SIGNAL(sectionResized(int, int, int)), this, SLOT(onSectionResized(int, int, int)));
    }

    explicit XHeaderView(Qt::Orientation orientation, int rows, int columns, QWidget* parent = 0) : QHeaderView(orientation, parent)
    {
        QSize baseSectionSize;
        if (orientation == Qt::Horizontal)
        {
            baseSectionSize.setWidth(defaultSectionSize());
            baseSectionSize.setHeight(20);
        }
        else
        {
            baseSectionSize.setWidth(50);
            baseSectionSize.setHeight(defaultSectionSize());
        }

        // create header model
        XHeaderModel* headerModel = new XHeaderModel(rows,columns);

        // set default size of item
        for (int row = 0; row < rows; ++row)
        {
            for (int col = 0; col < columns; ++col)
            {
                headerModel->setData(headerModel->index(row, col), baseSectionSize, Qt::SizeHintRole);
            }
        }
        setModel(headerModel);

        connect(this, SIGNAL(sectionResized(int, int, int)), this, SLOT(onSectionResized(int, int, int)));
    }

    virtual ~XHeaderView()
    {

    }

    void setRowHeight(int row, int rowHeight)
    {
        XHeaderModel* md = qobject_cast<XHeaderModel*>(model());
        const int cols = md->columnCount();
        for (int col = 0; col < cols; ++col)
        {
            QSize sz = md->index(row, col).data(Qt::SizeHintRole).toSize();
            sz.setHeight(rowHeight);
            md->setData(md->index(row, col), sz, Qt::SizeHintRole);
        }
        if (orientation() == Qt::Vertical)
            resizeSection(row, rowHeight);
    }

    void setColumnWidth(int col, int colWidth)
    {
        XHeaderModel* md = qobject_cast<XHeaderModel*>(model());
        const int rows = md->rowCount();
        for (int row = 0; row < rows; ++row)
        {
            QSize sz = md->index(row, col).data(Qt::SizeHintRole).toSize();
            sz.setWidth(colWidth);
            md->setData(md->index(row, col), sz, Qt::SizeHintRole);
        }
        if (orientation() == Qt::Horizontal)
            resizeSection(col, colWidth);
    }

    void setSpan(int row, int column, int rowSpanCount, int columnSpanCount)
    {
        XHeaderModel* md = qobject_cast<XHeaderModel*>(model());
        QModelIndex idx = md->index(row, column);
        if (rowSpanCount > 0)
            md->setData(idx, rowSpanCount, ROW_SPAN_ROLE);
        if(columnSpanCount)
        {
            md->setData(idx, columnSpanCount, COLUMN_SPAN_ROLE);
        }
    }

    void setCellBackgroundColor(const QModelIndex& index, const QColor& color)
    {
        XHeaderModel* md = qobject_cast<XHeaderModel*>(model());
        md->setData(index, color, Qt::BackgroundRole);
    }

    void setCellForegroundColor(const QModelIndex& index, const QColor& color)
    {
        XHeaderModel* md = qobject_cast<XHeaderModel*>(model());
        md->setData(index, color, Qt::ForegroundRole);
    }

protected:
    virtual void mousePressEvent(QMouseEvent* event)
    {
        QHeaderView::mousePressEvent(event);
        QPoint pos = event->pos();
        QModelIndex index = indexAt(pos);
        const int OTN = orientation();
        if (index.isValid())
        {
            int beginSection = -1;
            int endSection = -1;
            int numbers = 0;
            numbers = getSectionRange(index, &beginSection, &endSection);
            if(numbers > 0)
            {
                emit sectionPressed(beginSection,endSection);
                return;
            }
            else
            {
                const XHeaderModel* tblModel = qobject_cast<XHeaderModel*>(this->model());
                const int LEVEL_CNT = (OTN == Qt::Horizontal) ? tblModel->rowCount() : tblModel->columnCount();
                int logicalIdx = (OTN == Qt::Horizontal) ? index.column() : index.row();
                int curLevel   = (OTN == Qt::Horizontal) ? index.row() : index.column();
                for (int i = 0; i < LEVEL_CNT; ++i)
                {
                    QModelIndex cellIndex = (OTN == Qt::Horizontal) ? tblModel->index(i, logicalIdx) : tblModel->index(logicalIdx, i);
                    numbers = getSectionRange(cellIndex,&beginSection,&endSection);
                    if (numbers > 0)
                    {
                        if (beginSection <= logicalIdx && logicalIdx <= endSection)
                        {
                            int beginLevel = (OTN == Qt::Horizontal) ? cellIndex.row() : cellIndex.column();
                            QVariant levelSpanCnt = cellIndex.data((OTN == Qt::Horizontal)?ROW_SPAN_ROLE:COLUMN_SPAN_ROLE);
                            if (!levelSpanCnt.isValid())
                                continue;
                            int endLevel   = beginLevel + levelSpanCnt.toInt() - 1;
                            if (beginLevel <= curLevel && curLevel <= endLevel)
                            {
                                emit sectionPressed(beginSection,endSection);
                                 break;
                            }
                        }
                    }
                }
            }
        }
    }

    virtual QModelIndex indexAt(const QPoint& pos)
    {
        const XHeaderModel* tblModel = qobject_cast<XHeaderModel*>(this->model());
        const int OTN = orientation();
        const int ROWS = tblModel->rowCount();
        const int COLS = tblModel->columnCount();
        int logicalIdx = logicalIndexAt(pos);

        if (OTN == Qt::Horizontal)
        {
            int dY=0;
            for (int row = 0; row < ROWS; ++row)
            {
                QModelIndex cellIndex = tblModel->index(row, logicalIdx);
                dY += cellIndex.data(Qt::SizeHintRole).toSize().height();
                if (pos.y() <= dY)
                    return cellIndex;
            }
        }
        else
        {
            int dX=0;
            for (int col = 0; col < COLS; ++col)
            {
                QModelIndex cellIndex = tblModel->index(logicalIdx,col);
                dX += cellIndex.data(Qt::SizeHintRole).toSize().width();
                if (pos.x() <= dX)
                    return cellIndex;
            }
        }

        return QModelIndex();
    }

    virtual void paintSection(QPainter* painter, const QRect& rect, int logicalIdx) const
    {
        const XHeaderModel* tblModel = qobject_cast<XHeaderModel*>(this->model());
        const int OTN = orientation();
        const int LEVEL_CNT = (OTN == Qt::Horizontal) ? tblModel->rowCount() : tblModel->columnCount();
        for (int i = 0; i < LEVEL_CNT; ++i)
        {
            QModelIndex cellIndex = (OTN == Qt::Horizontal) ? tblModel->index(i, logicalIdx) : tblModel->index(logicalIdx, i);
            QSize cellSize = cellIndex.data(Qt::SizeHintRole).toSize();
            QRect sectionRect(rect);

            // set position of the cell
            if (OTN == Qt::Horizontal)
                sectionRect.setTop(rowSpanSize(logicalIdx, 0, i)); // distance from 0 to i-1 rows
            else
                sectionRect.setLeft(columnSpanSize(logicalIdx, 0, i));

            sectionRect.setSize(cellSize);

            // check up span column or row
            QModelIndex colSpanIdx = columnSpanIndex(cellIndex);
            QModelIndex rowSpanIdx = rowSpanIndex(cellIndex);
            if (colSpanIdx.isValid())
            {
                int colSpanFrom = colSpanIdx.column();
                int colSpanCnt  = colSpanIdx.data(COLUMN_SPAN_ROLE).toInt();
                int colSpanTo   = colSpanFrom + colSpanCnt - 1;
                int colSpan     = columnSpanSize(cellIndex.row(), colSpanFrom, colSpanCnt);
                if (OTN == Qt::Horizontal)
                    sectionRect.setLeft(sectionViewportPosition(colSpanFrom));
                else
                {
                    sectionRect.setLeft(columnSpanSize(logicalIdx, 0, colSpanFrom));
                    i = colSpanTo;
                }

                sectionRect.setWidth(colSpan);

                // check up  if the column span index has row span
                QVariant subRowSpanData = colSpanIdx.data(ROW_SPAN_ROLE);
                if (subRowSpanData.isValid())
                {
                    int subRowSpanFrom = colSpanIdx.row();
                    int subRowSpanCnt  = subRowSpanData.toInt();
                    int subRowSpanTo   = subRowSpanFrom+subRowSpanCnt - 1;
                    int subRowSpan     = rowSpanSize(colSpanFrom, subRowSpanFrom, subRowSpanCnt);
                    if (OTN == Qt::Vertical)
                        sectionRect.setTop(sectionViewportPosition(subRowSpanFrom));
                    else
                    {
                        sectionRect.setTop(rowSpanSize(colSpanFrom,0,subRowSpanFrom));
                        i = subRowSpanTo;
                    }
                    sectionRect.setHeight(subRowSpan);
                }
                cellIndex=colSpanIdx;
            }
            if (rowSpanIdx.isValid())
            {
                int rowSpanFrom = rowSpanIdx.row();
                int rowSpanCnt  = rowSpanIdx.data(ROW_SPAN_ROLE).toInt();
                int rowSpanTo   = rowSpanFrom + rowSpanCnt - 1;
                int rowSpan     = rowSpanSize(cellIndex.column(), rowSpanFrom, rowSpanCnt);
                if (OTN == Qt::Vertical)
                    sectionRect.setTop(sectionViewportPosition(rowSpanFrom));
                else
                {
                    sectionRect.setTop(rowSpanSize(logicalIdx, 0, rowSpanFrom));
                    i = rowSpanTo;
                }
                sectionRect.setHeight(rowSpan);

                // check up if the row span index has column span
                QVariant subColSpanData = rowSpanIdx.data(COLUMN_SPAN_ROLE);
                if (subColSpanData.isValid())
                {
                    int subColSpanFrom = rowSpanIdx.column();
                    int subColSpanCnt  = subColSpanData.toInt();
                    int subColSpanTo   = subColSpanFrom + subColSpanCnt - 1;
                    int subColSpan     = columnSpanSize(rowSpanFrom, subColSpanFrom, subColSpanCnt);
                    if (OTN == Qt::Horizontal)
                        sectionRect.setLeft(sectionViewportPosition(subColSpanFrom));
                    else
                    {
                        sectionRect.setLeft(columnSpanSize(rowSpanFrom, 0, subColSpanFrom));
                        i = subColSpanTo;
                    }
                    sectionRect.setWidth(subColSpan);
                }
                cellIndex=rowSpanIdx;
            }

            // draw section with style
            QStyleOptionHeader sectionStyle;
            initStyleOption(&sectionStyle);
            sectionStyle.textAlignment = Qt::AlignCenter;
            sectionStyle.iconAlignment = Qt::AlignVCenter;
            sectionStyle.section = logicalIdx;
            sectionStyle.text = cellIndex.data(Qt::DisplayRole).toString();
            sectionStyle.rect = sectionRect;

            // file background or foreground color of the cell
            QVariant bg = cellIndex.data(Qt::BackgroundRole);
            QVariant fg = cellIndex.data(Qt::ForegroundRole);
            if (bg.canConvert(QVariant::Brush))
            {
                sectionStyle.palette.setBrush(QPalette::Button, qvariant_cast<QBrush>(bg));
                sectionStyle.palette.setBrush(QPalette::Window, qvariant_cast<QBrush>(bg));
            }
            if (fg.canConvert(QVariant::Brush))
            {
                sectionStyle.palette.setBrush(QPalette::ButtonText, qvariant_cast<QBrush>(fg));
            }

            painter->save();
            qDrawShadePanel(painter, sectionStyle.rect, sectionStyle.palette, false, 1, &sectionStyle.palette.brush(QPalette::Button));
            style()->drawControl(QStyle::CE_HeaderLabel, &sectionStyle, painter);
            painter->restore();
        }
    }

    virtual QSize sectionSizeFromContents(int logicalIndex) const
    {
        const XHeaderModel* tblModel = qobject_cast<const XHeaderModel*>(this->model());
        const int OTN = orientation();
        const int LEVEL_CNT = (OTN == Qt::Horizontal) ? tblModel->rowCount():tblModel->columnCount();

        QSize siz = QHeaderView::sectionSizeFromContents(logicalIndex);
        for (int i = 0; i < LEVEL_CNT; ++i)
        {
            QModelIndex cellIndex = (OTN == Qt::Horizontal) ? tblModel->index(i, logicalIndex) : tblModel->index(logicalIndex, i);
            QModelIndex colSpanIdx = columnSpanIndex(cellIndex);
            QModelIndex rowSpanIdx = rowSpanIndex(cellIndex);
            siz = cellIndex.data(Qt::SizeHintRole).toSize();

            if (colSpanIdx.isValid())
            {
                int colSpanFrom = colSpanIdx.column();
                int colSpanCnt     = colSpanIdx.data(COLUMN_SPAN_ROLE).toInt();
                int colSpanTo   = colSpanFrom + colSpanCnt - 1;
                siz.setWidth(columnSpanSize(colSpanIdx.row(), colSpanFrom, colSpanCnt));
                if (OTN == Qt::Vertical)
                    i = colSpanTo;
            }
            if (rowSpanIdx.isValid())
            {
                int rowSpanFrom = rowSpanIdx.row();
                int rowSpanCnt  = rowSpanIdx.data(ROW_SPAN_ROLE).toInt();
                int rowSpanTo   = rowSpanFrom + rowSpanCnt - 1;
                siz.setHeight(rowSpanSize(rowSpanIdx.column(), rowSpanFrom, rowSpanCnt));
                if (OTN == Qt::Horizontal)
                    i = rowSpanTo;
            }
        }
        return siz;
    }

protected:
    QModelIndex columnSpanIndex(const QModelIndex& currentIndex) const
    {
        const XHeaderModel* tblModel = qobject_cast<XHeaderModel*>(model());
        const int curRow = currentIndex.row();
        const int curCol = currentIndex.column();
        int i = curCol;
        while (i >= 0)
        {
           QModelIndex spanIndex = tblModel->index(curRow,i);
            QVariant span   = spanIndex.data(COLUMN_SPAN_ROLE);
            if (span.isValid() && spanIndex.column() + span.toInt() - 1 >= curCol)
                return spanIndex;
            i--;
        }
        return QModelIndex();
    }

    QModelIndex rowSpanIndex(const QModelIndex& currentIdx) const
    {
        const XHeaderModel* tblModel = qobject_cast<XHeaderModel*>(model());
        const int curRow = currentIdx.row();
        const int curCol = currentIdx.column();
        int i = curRow;
        while (i >= 0)
        {
            QModelIndex spanIndex = tblModel->index(i,curCol);
            QVariant span   = spanIndex.data(ROW_SPAN_ROLE);
            if (span.isValid() && spanIndex.row() + span.toInt() - 1 >= curRow)
                return spanIndex;
            i--;
        }
        return QModelIndex();
    }

    int columnSpanSize(int row, int from, int spanCount) const
    {
        const XHeaderModel* tblModel = qobject_cast<XHeaderModel*>(model());
        int span = 0;
        for (int i = from; i < from + spanCount; ++i)
        {
            QSize cellSize = tblModel->index(row,i).data(Qt::SizeHintRole).toSize();
            span += cellSize.width();
        }
        return span;
    }

    int rowSpanSize(int column, int from, int spanCount) const
    {
        const XHeaderModel* tblModel = qobject_cast<XHeaderModel*>(model());
        int span = 0;
        for (int i = from; i < from+spanCount; ++i)
        {
            QSize cellSize = tblModel->index(i, column).data(Qt::SizeHintRole).toSize();
            span += cellSize.height();
        }
        return span;
    }

    int getSectionRange(QModelIndex&index, int* beginSection, int* endSection) const
    {
        // check up section range from the index
        QModelIndex colSpanIdx = columnSpanIndex(index);
        QModelIndex rowSpanIdx = rowSpanIndex(index);

        if (colSpanIdx.isValid())
        {
            int colSpanFrom = colSpanIdx.column();
            int colSpanCnt  = colSpanIdx.data(COLUMN_SPAN_ROLE).toInt();
            int colSpanTo   = colSpanFrom + colSpanCnt - 1;
            if (orientation() == Qt::Horizontal)
            {
                *beginSection = colSpanFrom;
                *endSection   = colSpanTo;
                index = colSpanIdx;
                return colSpanCnt;
            }
            else
            {
                // check up  if the column span index has row span
                QVariant subRowSpanData = colSpanIdx.data(ROW_SPAN_ROLE);
                if (subRowSpanData.isValid())
                {
                    int subRowSpanFrom = colSpanIdx.row();
                    int subRowSpanCnt  = subRowSpanData.toInt();
                    int subRowSpanTo   = subRowSpanFrom+subRowSpanCnt - 1;
                    *beginSection = subRowSpanFrom;
                    *endSection   = subRowSpanTo;
                    index = colSpanIdx;
                    return subRowSpanCnt;
                }
            }
        }

        if (rowSpanIdx.isValid())
        {
            int rowSpanFrom = rowSpanIdx.row();
            int rowSpanCnt  = rowSpanIdx.data(ROW_SPAN_ROLE).toInt();
            int rowSpanTo   = rowSpanFrom + rowSpanCnt - 1;
            if (orientation() == Qt::Vertical)
            {
                *beginSection = rowSpanFrom;
                *endSection   = rowSpanTo;
                index = rowSpanIdx;
                return rowSpanCnt;
            }
            else
            {
                // check up if the row span index has column span
                QVariant subColSpanData = rowSpanIdx.data(COLUMN_SPAN_ROLE);
                if (subColSpanData.isValid())
                {
                    int subColSpanFrom = rowSpanIdx.column();
                    int subColSpanCnt  = subColSpanData.toInt();
                    int subColSpanTo   = subColSpanFrom + subColSpanCnt - 1;
                    *beginSection = subColSpanFrom;
                    *endSection   = subColSpanTo;
                    index = rowSpanIdx;
                    return subColSpanCnt;
                }
            }
        }

        return 0;
    }

protected slots:
    void onSectionResized(int logicalIndex, int oldSize, int newSize)
    {
        XHeaderModel* tblModel = qobject_cast<XHeaderModel*>(this->model());
        const int OTN = orientation();
        const int LEVEL_CNT = (OTN == Qt::Horizontal) ? tblModel->rowCount() : tblModel->columnCount();
        int pos = sectionViewportPosition(logicalIndex);
        int xx  = (OTN == Qt::Horizontal) ? pos : 0;
        int yy  = (OTN == Qt::Horizontal) ? 0 : pos;
        QRect sectionRect(xx, yy, 0, 0);
        for (int i = 0; i< LEVEL_CNT; ++i)
        {
            QModelIndex cellIndex = (OTN == Qt::Horizontal) ? tblModel->index(i, logicalIndex)  :tblModel->index(logicalIndex, i);
            QSize cellSize = cellIndex.data(Qt::SizeHintRole).toSize();
            // set position of cell
            if (OTN == Qt::Horizontal)
            {
                sectionRect.setTop(rowSpanSize(logicalIndex, 0, i));
                cellSize.setWidth(newSize);
            }
            else
            {
                sectionRect.setLeft(columnSpanSize(logicalIndex, 0, i));
                cellSize.setHeight(newSize);
            }
            tblModel->setData(cellIndex, cellSize, Qt::SizeHintRole);

            QModelIndex colSpanIdx = columnSpanIndex(cellIndex);
            QModelIndex rowSpanIdx = rowSpanIndex(cellIndex);

            if (colSpanIdx.isValid())
            {
                int colSpanFrom = colSpanIdx.column();
                if (OTN == Qt::Horizontal)
                    sectionRect.setLeft(sectionViewportPosition(colSpanFrom));
                else
                {
                    sectionRect.setLeft(columnSpanSize(logicalIndex, 0, colSpanFrom));
                }

            }
            if (rowSpanIdx.isValid())
            {
                int rowSpanFrom = rowSpanIdx.row();
                if (OTN == Qt::Vertical)
                    sectionRect.setTop(sectionViewportPosition(rowSpanFrom));
                else
                    sectionRect.setTop(rowSpanSize(logicalIndex, 0, rowSpanFrom));
            }
            QRect rToUpdate(sectionRect);
            rToUpdate.setWidth(viewport()->width() - sectionRect.left());
            rToUpdate.setHeight(viewport()->height() - sectionRect.top());
            viewport()->update(rToUpdate.normalized());
        }
    }

signals:
    void sectionPressed(int from, int to);
};
}

#endif // XHEADERVIEW_HPP
