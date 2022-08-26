#ifndef FROZENTABLEVIEW_H
#define FROZENTABLEVIEW_H

#include <QWidget>
#include <QTableView>
#include <QSplitter>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QScrollBar>
#include <QDebug>

namespace FinTechUI
{

class FrozenTableView : public QTableView
{
    Q_OBJECT
public:
    explicit FrozenTableView(unsigned int frozenColumns, QWidget* parent = NULL) : QTableView(parent)
    {
        m_frozenColumns = frozenColumns;
        InitFrozenTableView();
        InitTableView();
    }

    virtual ~FrozenTableView()
    {
        delete m_frozenTableView;
        m_frozenTableView = NULL;
    }

    virtual void setModel(QAbstractItemModel *model)
    {
        if(model == NULL)
            return;
        QTableView::setModel(model);
        if(m_frozenTableView != NULL)
        {
            m_frozenTableView->setModel(model);
            m_frozenTableView->setSelectionModel(selectionModel());
            // 隐藏冻结表格的后续列、显示表格前m_frozenColumns冻结列，确保表格行对齐
            for(int col = 0; col < m_frozenColumns; col++)
            {
                m_frozenTableView->setColumnWidth(col, columnWidth(col));
            }
            for(int col = m_frozenColumns; col < model->columnCount(); col++)
            {
                m_frozenTableView->setColumnHidden(col, true);
            }
        }
    }

    virtual void setColumnHidden(int col, bool hidden)
    {
        if(m_frozenTableView != NULL && col < m_frozenColumns)
        {
            m_frozenTableView->setColumnHidden(col, hidden);
        }
        QTableView::setColumnHidden(col, hidden);
    }

    virtual void setColumnWidth(int col, int width)
    {
        if(m_frozenTableView != NULL && col < m_frozenColumns)
        {
            m_frozenTableView->setColumnWidth(col, width);
        }
        QTableView::setColumnWidth(col, width);
    }

    virtual void setShowGrid(bool show)
    {
        if(m_frozenTableView != NULL)
        {
            m_frozenTableView->setShowGrid(show);
        }
        QTableView::setShowGrid(show);
    }

    virtual void setHorizontalHeader(QHeaderView *header)
    {
        if(header == NULL)
            return;
        if(m_frozenTableView != NULL)
        {
            m_frozenTableView->setHorizontalHeader(header);
        }
        QTableView::setHorizontalHeader(header);
    }

    virtual QModelIndex indexAt(const QPoint &pos) const
    {
        if(underMouse())
        {
            return QTableView::indexAt(pos);
        }
        if(m_frozenTableView != NULL)
        {
            if(m_frozenTableView->underMouse())
            {
                return m_frozenTableView->indexAt(pos);
            }
        }
        return QModelIndex();
    }

    virtual void setRowHeight(int row, int height)
    {
        if(m_frozenTableView != NULL)
        {
            m_frozenTableView->setRowHeight(row, height);
        }
        QTableView::setRowHeight(row, height);
    }

    virtual void setRowHidden(int row, bool hide)
    {
        if(m_frozenTableView != NULL)
        {
            m_frozenTableView->setRowHidden(row, hide);
        }
        QTableView::setRowHidden(row, hide);
    }

    virtual void setSortingEnabled(bool enable)
    {
        if(m_frozenTableView != NULL)
        {
            m_frozenTableView->setSortingEnabled(enable);
        }
        QTableView::setSortingEnabled(enable);
    }

    virtual void setEditTriggers(QAbstractItemView::EditTriggers triggers)
    {
        if(m_frozenTableView != NULL)
        {
            m_frozenTableView->setEditTriggers(triggers);
        }
        QTableView::setEditTriggers(triggers);
    }

    virtual void setSelectionBehavior(QAbstractItemView::SelectionBehavior behavior)
    {
        if(m_frozenTableView != NULL)
        {
            m_frozenTableView->setSelectionBehavior(behavior);
        }
        QTableView::setSelectionBehavior(behavior);
    }

    virtual void setSelectionMode(QAbstractItemView::SelectionMode mode)
    {
        if(m_frozenTableView != NULL)
        {
            m_frozenTableView->setSelectionMode(mode);
        }
        QTableView::setSelectionMode(mode);
    }

    virtual void sortByColumn(int column, Qt::SortOrder order)
    {
        if(m_frozenTableView != NULL && column < m_frozenColumns)
        {
            m_frozenTableView->sortByColumn(column, order);
        }
        QTableView::sortByColumn(column, order);
    }

    virtual void hideColumn(int column)
    {
        if(m_frozenTableView != NULL && column < m_frozenColumns)
        {
            m_frozenTableView->hideColumn(column);
        }
        QTableView::hideColumn(column);
    }

    virtual void hideRow(int row)
    {
        if(m_frozenTableView != NULL)
        {
            m_frozenTableView->hideRow(row);
        }
        QTableView::hideRow(row);
    }

    virtual void resizeColumnToContents(int column)
    {
        if(m_frozenTableView != NULL && column < m_frozenColumns)
        {
            m_frozenTableView->resizeColumnToContents(column);
        }
        QTableView::resizeColumnToContents(column);
    }

    virtual void resizeColumnsToContents()
    {
        if(m_frozenTableView != NULL)
        {
            m_frozenTableView->resizeColumnsToContents();
        }
        QTableView::resizeColumnsToContents();
    }

    virtual void resizeRowToContents(int row)
    {
        if(m_frozenTableView != NULL)
        {
            m_frozenTableView->resizeRowToContents(row);
        }
        QTableView::resizeRowToContents(row);
    }

    virtual void resizeRowsToContents()
    {
        if(m_frozenTableView != NULL)
        {
            m_frozenTableView->resizeRowsToContents();
        }
        QTableView::resizeRowsToContents();
    }

    virtual void selectColumn(int column)
    {
        if(m_frozenTableView != NULL && column < m_frozenColumns)
        {
            m_frozenTableView->selectColumn(column);
        }
        QTableView::selectColumn(column);
    }

    virtual void selectRow(int row)
    {
        if(m_frozenTableView != NULL)
        {
            m_frozenTableView->selectRow(row);
        }
        QTableView::selectRow(row);
    }

    virtual void showColumn(int column)
    {
        if(m_frozenTableView != NULL && column < m_frozenColumns)
        {
            m_frozenTableView->showColumn(column);
        }
        QTableView::showColumn(column);
    }

    virtual void showRow(int row)
    {
        if(m_frozenTableView != NULL)
        {
            m_frozenTableView->showRow(row);
        }
        QTableView::showRow(row);
    }
protected:
    void InitTableView()
    {
        QTableView::setEditTriggers(QAbstractItemView::NoEditTriggers);
        QTableView::setSelectionBehavior(QAbstractItemView::SelectRows);
        QTableView::setSelectionMode(QAbstractItemView::SingleSelection);
        QTableView::setAlternatingRowColors(true);
        QTableView::setSortingEnabled(true);
        horizontalHeader()->setStretchLastSection(true);
        verticalHeader()->hide();
        setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        connect(this, &QAbstractItemView::doubleClicked, this, &FrozenTableView::doubleClicked);
        connect(this, &QAbstractItemView::clicked, this, &FrozenTableView::clicked);
    }

    void InitFrozenTableView()
    {
        if(m_frozenColumns > 0)
        {
            m_frozenTableView = new QTableView(this);
            m_frozenTableView->setFocusPolicy(Qt::NoFocus);
            // 设置冻结表格为无边框模式，确保没有边框重叠
            m_frozenTableView->setFrameShape(QFrame::NoFrame);
            m_frozenTableView->verticalHeader()->hide();
            m_frozenTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
            m_frozenTableView->horizontalHeader()->setStretchLastSection(true);
            m_frozenTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            m_frozenTableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            m_frozenTableView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
            m_frozenTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
            m_frozenTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
            m_frozenTableView->setSelectionMode(QAbstractItemView::SingleSelection);
            m_frozenTableView->setAlternatingRowColors(true);
            m_frozenTableView->setSortingEnabled(true);
            m_frozenTableView->horizontalHeader()->setSortIndicatorShown(false);

            viewport()->stackUnder(m_frozenTableView);
            UpdateFrozenTableGeometry();

            //connect the headers and scrollbars of both tableviews together
            connect(horizontalHeader(),&QHeaderView::sectionResized, this,
                    &FrozenTableView::OnUpdateSectionWidth);
            connect(verticalHeader(),&QHeaderView::sectionResized, this,
                    &FrozenTableView::OnUpdateSectionHeight);

            connect(m_frozenTableView->horizontalHeader(),&QHeaderView::sectionClicked, this,
                    &FrozenTableView::OnSectionClicked);
            connect(horizontalHeader(),&QHeaderView::sectionClicked, this,
                    &FrozenTableView::OnSectionClicked);

            connect(m_frozenTableView->verticalScrollBar(), &QAbstractSlider::valueChanged,
                    verticalScrollBar(), &QAbstractSlider::setValue);
            connect(verticalScrollBar(), &QAbstractSlider::valueChanged,
                    m_frozenTableView->verticalScrollBar(), &QAbstractSlider::setValue);
            connect(m_frozenTableView, &QAbstractItemView::doubleClicked, this, &FrozenTableView::doubleClicked);
            connect(m_frozenTableView, &QAbstractItemView::clicked, this, &FrozenTableView::clicked);
        }
        else
        {
            m_frozenTableView = NULL;
        }
    }

    void UpdateFrozenTableGeometry()
    {
        int width = 0;
        for(int i = 0; i < m_frozenColumns; i++)
        {
            width += m_frozenTableView->columnWidth(i);
        }
        m_frozenTableView->setGeometry(verticalHeader()->width() + frameWidth(),
                                    frameWidth(), width,
                                    viewport()->height() + horizontalHeader()->height());
    }

public:
    virtual void resizeEvent(QResizeEvent *event)
    {
        QTableView::resizeEvent(event);
        UpdateFrozenTableGeometry();
    }

    virtual QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
    {
        QModelIndex current = QTableView::moveCursor(cursorAction, modifiers);
        int width = 0;
        for(int i = 0; i < m_frozenColumns; i++)
        {
            width += m_frozenTableView->columnWidth(i);
        }
        // 水平拖动时设置滚动条
        if (cursorAction == MoveLeft && current.column() >= m_frozenColumns && visualRect(current).topLeft().x() < width)
        {
            const int newValue = horizontalScrollBar()->value() + visualRect(current).topLeft().x() - width;
            horizontalScrollBar()->setValue(newValue);
        }
        return current;
    }

    virtual void scrollTo (const QModelIndex & index, ScrollHint hint = EnsureVisible)
    {
        if(index.column() >= m_frozenColumns)
        {
            QTableView::scrollTo(index, hint);
        }
        else if(m_frozenTableView != NULL)
        {
            m_frozenTableView->scrollTo(index, hint); 
        }
    }
protected slots:
    void OnUpdateSectionWidth(int logicalIndex, int oldSize, int newSize)
    {
        if (logicalIndex == m_frozenColumns - 1)
        {
            // 设置冻结列前m_frozenColumns - 1列的固定宽度
            for(int i = 0; i < m_frozenColumns - 1; i++)
            {
                m_frozenTableView->setColumnWidth(i, columnWidth(i));
            }
            // 根据拖动尺寸变化设置第m_frozenColumns - 1列
            m_frozenTableView->setColumnWidth(logicalIndex, newSize);
            UpdateFrozenTableGeometry();
        }
    }

    void OnUpdateSectionHeight(int logicalIndex, int oldSize, int newSize)
    {
        m_frozenTableView->setRowHeight(logicalIndex, newSize);
    }

    void OnSectionClicked(int section)
    {
        if(m_frozenTableView != NULL)
        {
            if(section < m_frozenColumns)
            {
              horizontalHeader()->setSortIndicatorShown(false);
              m_frozenTableView->horizontalHeader()->setSortIndicatorShown(true);
            }
            else
            {
                horizontalHeader()->setSortIndicatorShown(true);
                m_frozenTableView->horizontalHeader()->setSortIndicatorShown(false);
            }
        }
    }
signals:
    void doubleClicked(const QModelIndex &index);
    void clicked(const QModelIndex &index);
protected:
    QTableView* m_frozenTableView;
    int m_frozenColumns;
};

}


#endif // FROZENTABLEVIEW_H
