#ifndef YHEADERVIEWTEST_HPP
#define YHEADERVIEWTEST_HPP

#include <QAbstractTableModel>
#include <QStandardItemModel>
#include <QWidget>
#include <QTableView>
#include <QVBoxLayout>

#include "YHeaderView.hpp"

namespace FinTechUI
{

class ExampleModel: public QAbstractTableModel
{
        QStandardItemModel _horizontalHeaderModel;
        QStandardItemModel _verticalHeaderModel;

        void fillHeaderModel(QStandardItemModel& headerModel)
        {
            QStandardItem* rootItem = new QStandardItem("root");
            QList<QStandardItem*> l;

            QStandardItem* rotatedTextCell=new QStandardItem("Rotated text");
            rotatedTextCell->setData(1, Qt::UserRole);
            l.push_back(rotatedTextCell);
            rootItem->appendColumn(l);

            l.clear();

            QStandardItem* cell=new QStandardItem("level 2");
            l.push_back(cell);
            rootItem->appendColumn(l);

            l.clear();

            l.push_back(new QStandardItem("level 3"));
            cell->appendColumn(l);

            l.clear();

            l.push_back(new QStandardItem("level 3"));
            cell->appendColumn(l);

            l.clear();

            l.push_back(new QStandardItem("level 2"));
            rootItem->appendColumn(l);

            headerModel.setItem(0, 0, rootItem);
        }

    public:
        ExampleModel(QObject* parent=0): QAbstractTableModel(parent)
        {
            fillHeaderModel(_horizontalHeaderModel);
            fillHeaderModel(_verticalHeaderModel);
        }

    int rowCount(const QModelIndex& /*parent*/) const
    {
                return 5;
    }

    int columnCount(const QModelIndex& /*parent*/) const
    {
        return 4;
    }

    QVariant data(const QModelIndex& index, int role) const
    {
        if(role == HeaderDataModelRoles::HorizontalHeaderDataRole)
        {
            QVariant v;
            v.setValue((QObject*)&_horizontalHeaderModel);
            return v;
        }
        if(role==HeaderDataModelRoles::VerticalHeaderDataRole)
        {
            QVariant v;
            v.setValue((QObject*)&_verticalHeaderModel);
            return v;
        }
        if(role==Qt::DisplayRole && index.isValid())
        {
            return QString("index(%1, %2)").arg(index.row()).arg(index.column());
        }
        return QVariant();
    }

    Qt::ItemFlags flags ( const QModelIndex & index ) const
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
};

class MainWindow : public QWidget
{
Q_OBJECT
public:
    MainWindow(QWidget* parent = 0) : QWidget(parent)
    {
        m_TableView = new QTableView(this);
        m_ExampleModel = new ExampleModel;
        YHeaderView* hv = new YHeaderView(Qt::Horizontal, m_TableView);
        hv->setSectionsClickable(true);
        hv->setHighlightSections(true);
        hv->setSectionsClickable(true);
        m_TableView->setHorizontalHeader(hv);
        hv = new YHeaderView(Qt::Vertical, m_TableView);
        hv->setHighlightSections(true);
        hv->setSectionsClickable(true);
        m_TableView->setVerticalHeader(hv);
        m_TableView->setModel(m_ExampleModel);
        m_TableView->resizeColumnsToContents();
        m_TableView->resizeRowsToContents();
        m_TableView->setSortingEnabled(true);

        QVBoxLayout* layout = new QVBoxLayout;
        layout->addWidget(m_TableView);
        setLayout(layout);
    }

    virtual ~MainWindow()
    {

    }

private:
    QTableView* m_TableView;
    ExampleModel* m_ExampleModel;
};
}

#endif // YHEADERVIEWTEST_HPP
