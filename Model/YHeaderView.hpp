#ifndef YHEADERVIEW_H
#define YHEADERVIEW_H

#include <QHeaderView>
#include <QPainter>
#include <QAbstractItemModel>
#include <QPointer>

namespace FinTechUI
{

class YHeaderView;

enum HeaderDataModelRoles
{
    HorizontalHeaderDataRole = Qt::UserRole,
    VerticalHeaderDataRole = Qt::UserRole + 1
};

class YHeaderViewData
{
public:
    QPointer<QAbstractItemModel> m_HeaderModel;
    YHeaderViewData()
    {

    }

    void InitFromNewModel(int orientation, QAbstractItemModel* model)
    {
        m_HeaderModel = QPointer<QAbstractItemModel>();
        QVariant data(model->data(QModelIndex(), (orientation == Qt::Horizontal ? HorizontalHeaderDataRole : VerticalHeaderDataRole)));
        if(data.isValid())
        {
            m_HeaderModel = qobject_cast<QAbstractItemModel*>(data.value<QObject*>());
        }
    }

    QModelIndex FindRootIndex(QModelIndex index) const
    {
        while(index.parent().isValid())
            index = index.parent();
        return index;
    }

    QModelIndexList ParentIndexes(QModelIndex index) const
    {
        QModelIndexList indexes;
        while(index.isValid())
        {
            indexes.push_front(index);
            index = index.parent();
        }
        return indexes;
    }

    QModelIndex FindLeaf(const QModelIndex& curentIndex, int sectionIndex, int& curentLeafIndex)
    {
        if(curentIndex.isValid())
        {
            int childCount = curentIndex.model()->columnCount(curentIndex);
            if(childCount)
            {
                for(int i = 0; i < childCount; ++i)
                {
                    QModelIndex res(FindLeaf(curentIndex.child(0, i), sectionIndex, curentLeafIndex));
                    if(res.isValid())
                        return res;
                }
            }
            else
            {
                ++curentLeafIndex;
                if(curentLeafIndex == sectionIndex)
                    return curentIndex;
            }
        }
        return QModelIndex();
    }

    QModelIndex LeafIndex(int sectionIndex)
    {
        if(m_HeaderModel)
        {
            int currentLeafIndex = -1;
            for(int i = 0; i < m_HeaderModel->columnCount(); ++i)
            {
                QModelIndex CurrentIndex(FindLeaf(m_HeaderModel->index(0, i), sectionIndex, currentLeafIndex));
                if(CurrentIndex.isValid())
                    return CurrentIndex;
            }
        }
        return QModelIndex();
    }

    QModelIndexList SearchLeafs(const QModelIndex& curentIndex) const
    {
        QModelIndexList res;
        if(curentIndex.isValid())
        {
            int childCount = curentIndex.model()->columnCount(curentIndex);
            if(childCount)
            {
                for(int i = 0; i < childCount; ++i)
                    res += SearchLeafs(curentIndex.child(0, i));
            }
            else
            {
                res.push_back(curentIndex);
            }
        }
        return res;
    }

    QModelIndexList Leafs(const QModelIndex& searchedIndex) const
    {
        QModelIndexList leafs;
        if(searchedIndex.isValid())
        {
            int childCount = searchedIndex.model()->columnCount(searchedIndex);
            for(int i = 0; i < childCount; ++i)
            {
                leafs += SearchLeafs(searchedIndex.child(0, i));
            }
        }
        return leafs;
    }

    void setForegroundBrush(QStyleOptionHeader& opt, const QModelIndex& index) const
    {
        QVariant foregroundBrush = index.data(Qt::ForegroundRole);
        if(foregroundBrush.canConvert(QVariant::Brush))
        {
            opt.palette.setBrush(QPalette::ButtonText, qvariant_cast<QBrush>(foregroundBrush));
        }
    }

    void setBackgroundBrush(QStyleOptionHeader& opt, const QModelIndex& index) const
    {
        QVariant backgroundBrush = index.data(Qt::BackgroundRole);
        if(backgroundBrush.canConvert(QVariant::Brush))
        {
            opt.palette.setBrush(QPalette::Button, qvariant_cast<QBrush>(backgroundBrush));
            opt.palette.setBrush(QPalette::Window, qvariant_cast<QBrush>(backgroundBrush));
        }
    }

    QSize CellSize(const QModelIndex& leafIndex, const QHeaderView* hv, QStyleOptionHeader styleOptions) const
    {
        QSize res;
        QVariant variant(leafIndex.data(Qt::SizeHintRole));
        if(variant.isValid())
        {
            res = qvariant_cast<QSize>(variant);
        }
        QFont fnt(hv->font());
        QVariant var(leafIndex.data(Qt::FontRole));
        if(var.isValid() && var.canConvert(QVariant::Font))
        {
            fnt = qvariant_cast<QFont>(var);
        }
        fnt.setBold(true);
        QFontMetrics fm(fnt);
        QSize size(fm.size(0, leafIndex.data(Qt::DisplayRole).toString()));
        if(leafIndex.data(Qt::UserRole).isValid())
        {
            size.transpose();
        }
        QSize decorationsSize(hv->style()->sizeFromContents(QStyle::CT_HeaderSection, &styleOptions, QSize(), hv));
        QSize emptyTextSize(fm.size(0, ""));
        return res.expandedTo(size+decorationsSize-emptyTextSize);
    }

    int	CurrentCellWidth(const QModelIndex& searchedIndex, const QModelIndex& leafIndex, int sectionIndex, const QHeaderView* hv) const
    {
        QModelIndexList leafsList(Leafs(searchedIndex));
        if(leafsList.empty())
        {
            return hv->sectionSize(sectionIndex);
        }
        int width = 0;
        int firstLeafSectionIndex = sectionIndex - leafsList.indexOf(leafIndex);
        for(int i = 0; i < leafsList.size(); ++i)
        {
            width += hv->sectionSize(firstLeafSectionIndex+i);
        }
        return width;
    }

    int	CurrentCellLeft(const QModelIndex& searchedIndex, const QModelIndex& leafIndex, int sectionIndex, int left, const QHeaderView* hv) const
    {
        QModelIndexList leafsList(Leafs(searchedIndex));
        if(!leafsList.empty())
        {
            int n = leafsList.indexOf(leafIndex);
            int firstLeafSectionIndex = sectionIndex - n;
            --n;
            for(; n >= 0; --n)
            {
                left -= hv->sectionSize(firstLeafSectionIndex+n);
            }
        }
        return left;
    }

    int paintHorizontalCell(QPainter *painter, const QHeaderView* hv, const QModelIndex& cellIndex, const QModelIndex& leafIndex,
                                int logicalLeafIndex, const QStyleOptionHeader& styleOptions, const QRect& sectionRect, int top) const
    {
        QStyleOptionHeader uniopt(styleOptions);
        setForegroundBrush(uniopt, cellIndex);
        setBackgroundBrush(uniopt, cellIndex);

        int height = CellSize(cellIndex, hv, uniopt).height();
        if(cellIndex == leafIndex)
        {
            height = sectionRect.height() - top;
        }
        int left = CurrentCellLeft(cellIndex, leafIndex, logicalLeafIndex, sectionRect.left(), hv);
        int width = CurrentCellWidth(cellIndex, leafIndex, logicalLeafIndex, hv);

        QRect r(left, top, width, height);

        uniopt.text = cellIndex.data(Qt::DisplayRole).toString();
        painter->save();
        uniopt.rect = r;
        if(cellIndex.data(Qt::UserRole).isValid())
        {
            hv->style()->drawControl(QStyle::CE_HeaderSection, &uniopt, painter, hv);
            QMatrix m;
            m.rotate(-90);
            painter->setWorldMatrix(m, true);
            QRect new_r(0, 0,  r.height(), r.width());
            new_r.moveCenter(QPoint(-r.center().y(), r.center().x()));
            uniopt.rect = new_r;
            hv->style()->drawControl(QStyle::CE_HeaderLabel, &uniopt, painter, hv);
        }
        else
        {
            hv->style()->drawControl(QStyle::CE_Header, &uniopt, painter, hv);
        }
        painter->restore();
        return top + height;
    }

    void paintHorizontalSection(QPainter *painter, const QRect& sectionRect, int logicalLeafIndex, const QHeaderView* hv,
                                    const QStyleOptionHeader& styleOptions, const QModelIndex& leafIndex) const
    {
        QPointF oldBO(painter->brushOrigin());
        int top = sectionRect.y();
        QModelIndexList indexes(ParentIndexes(leafIndex));
        for(int i = 0; i < indexes.size(); ++i)
        {
            QStyleOptionHeader realStyleOptions(styleOptions);
            if(i < indexes.size() - 1 && (realStyleOptions.state.testFlag(QStyle::State_Sunken) || realStyleOptions.state.testFlag(QStyle::State_On)))
            {
                QStyle::State t(QStyle::State_Sunken | QStyle::State_On);
                realStyleOptions.state &= (~t);
            }
            top = paintHorizontalCell(painter, hv, indexes[i], leafIndex, logicalLeafIndex, realStyleOptions, sectionRect, top);
        }
        painter->setBrushOrigin(oldBO);
    }

    int paintVerticalCell(QPainter *painter, const QHeaderView* hv, const QModelIndex& cellIndex, const QModelIndex& leafIndex,
                            int logicalLeafIndex, const QStyleOptionHeader& styleOptions, const QRect& sectionRect, int left) const
    {
        QStyleOptionHeader uniopt(styleOptions);
        setForegroundBrush(uniopt, cellIndex);
        setBackgroundBrush(uniopt, cellIndex);

        int width = CellSize(cellIndex, hv, uniopt).width();
        if(cellIndex == leafIndex)
        {
            width = sectionRect.width() - left;
        }
        int top = CurrentCellLeft(cellIndex, leafIndex, logicalLeafIndex, sectionRect.top(), hv);
        int height = CurrentCellWidth(cellIndex, leafIndex, logicalLeafIndex, hv);

        QRect r(left, top, width, height);

        uniopt.text = cellIndex.data(Qt::DisplayRole).toString();
        painter->save();
        uniopt.rect = r;
        if(cellIndex.data(Qt::UserRole).isValid())
        {
            hv->style()->drawControl(QStyle::CE_HeaderSection, &uniopt, painter, hv);
            QMatrix m;
            m.rotate(-90);
            painter->setWorldMatrix(m, true);
            QRect new_r(0, 0,  r.height(), r.width());
            new_r.moveCenter(QPoint(-r.center().y(), r.center().x()));
            uniopt.rect = new_r;
            hv->style()->drawControl(QStyle::CE_HeaderLabel, &uniopt, painter, hv);
        }
        else
        {
            hv->style()->drawControl(QStyle::CE_Header, &uniopt, painter, hv);
        }
        painter->restore();
        return left+width;
    }

    void paintVerticalSection(QPainter *painter, const QRect& sectionRect, int logicalLeafIndex, const QHeaderView* hv,
                                const QStyleOptionHeader& styleOptions, const QModelIndex& leafIndex) const
    {
        QPointF oldBO(painter->brushOrigin());
        int left = sectionRect.x();
        QModelIndexList indexes(ParentIndexes(leafIndex));
        for(int i = 0; i < indexes.size(); ++i)
        {
            QStyleOptionHeader realStyleOptions(styleOptions);
            if(i < indexes.size() - 1 && (realStyleOptions.state.testFlag(QStyle::State_Sunken) || realStyleOptions.state.testFlag(QStyle::State_On)))
            {
                QStyle::State t(QStyle::State_Sunken | QStyle::State_On);
                realStyleOptions.state &= (~t);
            }
            left = paintVerticalCell(painter, hv, indexes[i], leafIndex, logicalLeafIndex, realStyleOptions, sectionRect, left);
        }
        painter->setBrushOrigin(oldBO);
    }
};

class YHeaderView : public QHeaderView
{
    Q_OBJECT
public:
    YHeaderView(Qt::Orientation orientation, QWidget* parent = 0) :QHeaderView(orientation, parent), _data(new YHeaderViewData())
    {
        connect(this, SIGNAL(sectionResized(int, int, int)), this, SLOT(OnSectionResized(int)));
    }

    void setHeaderLabels();

    ~YHeaderView()
    {
        delete _data;
    }

    void setModel(QAbstractItemModel* model)
    {
        _data->InitFromNewModel(orientation(), model);
        QHeaderView::setModel(model);
        int cnt = (orientation() == Qt::Horizontal ? model->columnCount() : model->rowCount());
        if(cnt)
        {
            initializeSections(0, cnt-1);
        }
    }
protected:
    void paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const
    {
        if (rect.isValid())
        {
            QModelIndex leafIndex(_data->LeafIndex(logicalIndex));
            if(leafIndex.isValid())
            {
                if(orientation() == Qt::Horizontal)
                    _data->paintHorizontalSection(painter, rect, logicalIndex, this, styleOptionForCell(logicalIndex), leafIndex);
                else
                    _data->paintVerticalSection(painter, rect, logicalIndex, this, styleOptionForCell(logicalIndex), leafIndex);
                return;
            }
        }
        QHeaderView::paintSection(painter, rect, logicalIndex);
    }
    QSize sectionSizeFromContents(int logicalIndex) const
    {
        if(_data->m_HeaderModel)
        {
            QModelIndex curLeafIndex(_data->LeafIndex(logicalIndex));
            if(curLeafIndex.isValid())
            {
                QStyleOptionHeader styleOption(styleOptionForCell(logicalIndex));
                QSize s(_data->CellSize(curLeafIndex, this, styleOption));
                curLeafIndex = curLeafIndex.parent();
                while(curLeafIndex.isValid())
                {
                    if(orientation() == Qt::Horizontal)
                        s.rheight() += _data->CellSize(curLeafIndex, this, styleOption).height();
                    else
                        s.rwidth() += _data->CellSize(curLeafIndex, this, styleOption).width();
                    curLeafIndex = curLeafIndex.parent();
                }
                return s;
            }
        }
        return QHeaderView::sectionSizeFromContents(logicalIndex);
    }

    QStyleOptionHeader styleOptionForCell(int logicalInd) const
    {
        QStyleOptionHeader opt;
        initStyleOption(&opt);
        if(window()->isActiveWindow())
        {
            opt.state |= QStyle::State_Active;
        }
        opt.textAlignment = Qt::AlignCenter;
        opt.iconAlignment = Qt::AlignVCenter;
        opt.section = logicalInd;

        int visual = visualIndex(logicalInd);

        if (count() == 1)
            opt.position = QStyleOptionHeader::OnlyOneSection;
        else
        {
            if (visual == 0)
                opt.position = QStyleOptionHeader::Beginning;
            else
                opt.position = (visual == count()-1 ? QStyleOptionHeader::End : QStyleOptionHeader::Middle);
        }

        if(sectionsClickable())
        {
            if(highlightSections() && selectionModel())
            {
                if(orientation()==Qt::Horizontal)
                {
                    if(selectionModel()->columnIntersectsSelection(logicalInd, rootIndex()))
                        opt.state |= QStyle::State_On;
                    if(selectionModel()->isColumnSelected(logicalInd, rootIndex()))
                        opt.state |= QStyle::State_Sunken;
                }
                else
                {
                    if(selectionModel()->rowIntersectsSelection(logicalInd, rootIndex()))
                        opt.state |= QStyle::State_On;
                    if(selectionModel()->isRowSelected(logicalInd, rootIndex()))
                        opt.state |= QStyle::State_Sunken;
                }
            }
        }
        if(selectionModel())
        {
            bool previousSelected=false;
            if(orientation() == Qt::Horizontal)
                previousSelected = selectionModel()->isColumnSelected(logicalIndex(visual - 1), rootIndex());
            else
                previousSelected = selectionModel()->isRowSelected(logicalIndex(visual - 1), rootIndex());
            bool nextSelected=false;
            if(orientation() == Qt::Horizontal)
                nextSelected = selectionModel()->isColumnSelected(logicalIndex(visual + 1), rootIndex());
            else
                nextSelected = selectionModel()->isRowSelected(logicalIndex(visual + 1), rootIndex());
            if (previousSelected && nextSelected)
                opt.selectedPosition = QStyleOptionHeader::NextAndPreviousAreSelected;
            else
            {
                if (previousSelected)
                    opt.selectedPosition = QStyleOptionHeader::PreviousIsSelected;
                else
                {
                    if (nextSelected)
                        opt.selectedPosition = QStyleOptionHeader::NextIsSelected;
                    else
                        opt.selectedPosition = QStyleOptionHeader::NotAdjacent;
                }
            }
        }
        return opt;
    }
private slots:
    void OnSectionResized(int logicalIndex)
    {
        if(isSectionHidden(logicalIndex))
            return;

        QModelIndex leafIndex(_data->LeafIndex(logicalIndex));
        if(leafIndex.isValid())
        {
            QModelIndexList leafsList(_data->Leafs(_data->FindRootIndex(leafIndex)));
            for(int n = leafsList.indexOf(leafIndex); n > 0; --n)
            {
                --logicalIndex;

                int w = viewport()->width();
                int h = viewport()->height();
                int pos = sectionViewportPosition(logicalIndex);
                QRect r(pos, 0, w - pos, h);
                if(orientation() == Qt::Horizontal)
                {
                    if (isRightToLeft())
                    {
                        r.setRect(0, 0, pos + sectionSize(logicalIndex), h);
                    }
                }
                else
                {
                    r.setRect(0, pos, w, h - pos);
                }
                viewport()->update(r.normalized());
            }
        }
    }
private:
    YHeaderViewData* _data;
};

}

#endif // YHEADERVIEW_H
