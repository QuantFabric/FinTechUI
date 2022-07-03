#ifndef XHEADERVIEWTEST_HPP
#define XHEADERVIEWTEST_HPP

#include <QWidget>
#include <QTableView>
#include <QVBoxLayout>
#include "XHeaderView.hpp"

namespace FinTechUI{

class MainWindow : public QWidget
{
Q_OBJECT
public:
    MainWindow(QWidget* parent = 0) : QWidget(parent)
    {
        QStandardItemModel* dataModel = new QStandardItemModel;
        QList<QVariantMap> headerData;
        {
            QVariantMap data;
            data["name"] = "Colo";
            QStringList items;
            items.append("Colo");
            data["items"] = items;
            headerData.append(data);
        }
        {
            QVariantMap data;
            data["name"] = "OSVersion";
            QStringList items;
            items.append("OSVersion");
            data["items"] = items;
            headerData.append(data);
        }
        {
            QVariantMap data;
            data["name"] = "LoadAverage";
            QStringList items;
            items.append("min1");
            items.append("min5");
            items.append("min15");
            items.append("CPUS");
            data["items"] = items;
            headerData.append(data);
        }

        {
            QVariantMap data;
            data["name"] = "CPUUsage";
            QStringList items;
            items.append("UserRate");
            items.append("SysRate");
            items.append("IdleRate");
            items.append("IOWaitRate");
            items.append("IrqRate");
            items.append("SoftIrqRate");
            items.append("UsedRate");
            data["items"] = items;
            headerData.append(data);
        }

        {
            QVariantMap data;
            data["name"] = "MemoryInfo";
            QStringList items;
            items.append("Total");
            items.append("Free");
            items.append("UsedRate");
            data["items"] = items;
            headerData.append(data);
        }

        {
            QVariantMap data;
            data["name"] = "DiskInfo";
            QStringList items;
            items.append("Total");
            items.append("Free");
            items.append("UsedRate");
            items.append("Mount1UsedRate");
            items.append("Mount2UsedRate");
            data["items"] = items;
            headerData.append(data);
        }

        XHeaderView* hHead = new XHeaderView(headerData);

//        XHeaderView* hHead = new XHeaderView(Qt::Horizontal, 3, 4);
//        QAbstractItemModel* hModel = hHead->model();
//        hHead->setSpan(0,0,3,0);
//        hHead->setSpan(0,1,2,2);
//        hHead->setSpan(1,3,2,0);
//        hModel->setData(hModel->index(0,0),QString("cell1"),Qt::DisplayRole);
//        hModel->setData(hModel->index(0,1),QString("cell2"),Qt::DisplayRole);
//        hModel->setData(hModel->index(2,1),QString("cell3"),Qt::DisplayRole);
//        hModel->setData(hModel->index(2,2),QString("cell4"),Qt::DisplayRole);
//        hModel->setData(hModel->index(0,3),QString("cell5"),Qt::DisplayRole);
//        hModel->setData(hModel->index(1,3),QString("cell6"),Qt::DisplayRole);

        XHeaderView* vHead = new XHeaderView(Qt::Vertical,4,3);
        QAbstractItemModel* vModel = vHead->model();
        vHead->setSpan(0,0,0,3);
        vHead->setSpan(1,0,3,0);
        vHead->setSpan(1,1,2,0);
        vModel->setData(vModel->index(0,0),QString("cell1"),Qt::DisplayRole);
        vModel->setData(vModel->index(1,0),QString("cell2"),Qt::DisplayRole);
        vModel->setData(vModel->index(1,1),QString("cell3"),Qt::DisplayRole);
        vModel->setData(vModel->index(3,1),QString("cell4"),Qt::DisplayRole);
        vModel->setData(vModel->index(1,2),QString("cell5"),Qt::DisplayRole);
        vModel->setData(vModel->index(2,2),QString("cell6"),Qt::DisplayRole);
        vModel->setData(vModel->index(3,2),QString("cell7"),Qt::DisplayRole);

//        hHead->setRowHeight(0,30);
//        hHead->setRowHeight(1,30);
//        hHead->setRowHeight(2,30);

        vHead->setRowHeight(0,30);
        vHead->setRowHeight(1,30);
        vHead->setRowHeight(2,30);
        vHead->setColumnWidth(0,50);
        vHead->setColumnWidth(1,50);
        vHead->setColumnWidth(2,50);

        hHead->setSectionsClickable(true);
        vHead->setSectionsClickable(true);

//        hHead->setCellBackgroundColor(hModel->index(0,0),0xcfcfcf);
//        hHead->setCellBackgroundColor(hModel->index(0,1),0xcfcfcf);

        vHead->setCellBackgroundColor(vModel->index(0,0),Qt::cyan);
        vHead->setCellBackgroundColor(vModel->index(1,0),0xcfcfcf);
        for (int i=0;i<4;i++)
        {
            QList<QStandardItem*> items;
            for (int j=0;j<21;j++)
            {
                items.append(new QStandardItem(QString("item(%1,%2)").arg(i).arg(j)));
            }
            dataModel->appendRow(items);
        }

        m_TableView = new QTableView(this);
        m_TableView->setModel(dataModel);
        m_TableView->setHorizontalHeader(hHead);
        m_TableView->setVerticalHeader(vHead);
        m_TableView->setSortingEnabled(true);
        QVBoxLayout* layout = new QVBoxLayout;
        layout->addWidget(m_TableView);
        setLayout(layout);

        connect(hHead, SIGNAL(sectionPressed(int, int)), this, SLOT(OnHorizontalHeaderSectionPressed(int,int)));
        connect(vHead, SIGNAL(sectionPressed(int, int)), this, SLOT(OnVerticalHeaderSectionPressed(int,int)));
    }

    virtual ~MainWindow()
    {

    }
protected slots:
    void OnHorizontalHeaderSectionPressed(int beginSection, int endSection)
    {
        m_TableView->clearSelection();
        QAbstractItemView::SelectionMode oldSelectionMode = m_TableView->selectionMode();
        m_TableView->setSelectionMode(QAbstractItemView::MultiSelection);
        for (int i=beginSection;i<endSection+1;++i)
            m_TableView->selectColumn(i);
        m_TableView->setSelectionMode(oldSelectionMode);
    }

    void OnVerticalHeaderSectionPressed(int beginSection, int endSection)
    {
        m_TableView->clearSelection();
        QAbstractItemView::SelectionMode oldSelectionMode = m_TableView->selectionMode();
        m_TableView->setSelectionMode(QAbstractItemView::MultiSelection);
        for (int i=beginSection;i<endSection+1;++i)
            m_TableView->selectRow(i);
        m_TableView->setSelectionMode(oldSelectionMode);
    }

private:
    QTableView* m_TableView;
};

}

#endif // XHEADERVIEWTEST_HPP