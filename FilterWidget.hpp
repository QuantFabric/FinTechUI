#ifndef FILTERWIDGET_HPP
#define FILTERWIDGET_HPP

#include <QWidget>
#include <QSplitter>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QStringList>
#include <QMap>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QSize>
#include <QVector>
#include <QHeaderView>
#include <QDebug>

namespace FinTechUI
{

class FilterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FilterWidget(QWidget *parent = NULL): QWidget(parent)
    {

    }

    void SetHeaderLabels(const QStringList& labels)
    {
        m_HeaderLabels = labels;
        QHBoxLayout* layout = new QHBoxLayout;
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        QFont serifFont("Times", 10, QFont::Bold);
        int i = 0;
        foreach(QString label, m_HeaderLabels)
        {
            QVBoxLayout* vLayout = new QVBoxLayout;
            vLayout->setContentsMargins(0, 0, 0, 0);
            vLayout->setSpacing(0);
            QLabel* labelWidget = new QLabel(label);
            labelWidget->setFixedHeight(25);
            labelWidget->setAlignment(Qt::AlignCenter);
            labelWidget->setFont(serifFont);
            vLayout->addWidget(labelWidget);
            QTableWidget* listWidget = new QTableWidget(0, 1);
            listWidget->horizontalHeader()->hide();
            listWidget->verticalHeader()->hide();
            listWidget->setShowGrid(true);
            listWidget->setObjectName(label);
            listWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
            listWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
            listWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
            listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            listWidget->horizontalHeader()->setStretchLastSection(true);
            listWidget->setAlternatingRowColors(true);
            vLayout->addWidget(listWidget);
            m_ListWidgetMap[label] = listWidget;
            layout->addLayout(vLayout);
            connect(listWidget, &QTableWidget::cellClicked, this, &FilterWidget::OnClickedFilter, Qt::UniqueConnection);
            m_IndexLabelMap[i] = label;
            m_LabelSelectedRowMap[label] = -1;
            m_LabelIndexMap[label] = i++;
        }
        layout->addStretch(1);
        setLayout(layout);
    }

    void SetData(const QString& label, const QStringList& items)
    {
        if(m_ListWidgetMap.contains(label))
         {
             QStringList list = items;
             list.sort();
             m_DataMap[label] = list;
             QTableWidget* listWidget = m_ListWidgetMap[label];
             while(listWidget->rowCount() > 0)
             {
                 listWidget->removeRow(0);
             }
             int row = listWidget->rowCount();
             listWidget->insertRow(row);
             QTableWidgetItem* allItem = new QTableWidgetItem;
             allItem->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
             allItem->setText("All");
             listWidget->setItem(row++, 0, allItem);

             foreach (QString item, list)
             {
                 listWidget->insertRow(row);
                 QTableWidgetItem* dataItem = new QTableWidgetItem;
                 dataItem->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
                 dataItem->setText(item);
                 listWidget->setItem(row++, 0, dataItem);
             }
         }
    }

    void SetColumnWidth(const QString& label, int width)
    {
        if(m_ListWidgetMap.contains(label))
        {
            m_ListWidgetMap[label]->setFixedWidth(width);
            m_ListWidgetMap[label]->setColumnWidth(0, width);
        }
        int total = 0;
        foreach(QString label, m_ListWidgetMap.keys())
        {
            total += m_ListWidgetMap[label]->width();
        }
        setMaximumWidth(total);
    }

    void SetDataRelationMap(const QVector<QMap<QString, QStringList>>& map)
    {
        m_DataRelationMap = map;
        for (int i = 0; i < map.size(); i++)
        {
            QString label = m_IndexLabelMap[i];
            QStringList items = map.at(i).keys();
            items.sort();
            SetData(label, items);
        }
        QString label = m_IndexLabelMap[map.size()];
        QStringList items;
        foreach (QString key, map.last().keys())
        {
            items.append(map.last().value(key));
        }
        items.removeDuplicates();
        items.sort();
        SetData(label, items);
    }

protected slots:
    void OnClickedFilter(int row)
    {
        QTableWidget* listWidget = dynamic_cast<QTableWidget*>(QObject::sender());
        if(listWidget != NULL)
        {
         QString label = listWidget->objectName();
         if(m_LabelSelectedRowMap[label] == row)
         {
             m_LabelSelectedRowMap[label] = -1;
             listWidget->clearSelection();
         }
         else
         {
             m_LabelSelectedRowMap[label] = row;
             if(row == 0)
             {
                 listWidget->selectAll();
             }
             else
             {
                 QList<QTableWidgetItem *> selectedItems = m_ListWidgetMap[label]->selectedItems();
                 if(selectedItems.size() == (m_ListWidgetMap[label]->rowCount() - 1) && !selectedItems.contains(m_ListWidgetMap[label]->item(0, 0)))
                 {
                     listWidget->selectAll();
                 }
             }
         }

         int count = m_LabelIndexMap.size();
         for (int i = m_LabelIndexMap[label] + 1; i < count; i++)
         {
             QString Key = m_IndexLabelMap[i];
             m_ListWidgetMap[Key]->clearSelection();
         }
         QVector<QStringList> filter;
         for (int i = 0; i < count; i++)
         {
             QString Key = m_IndexLabelMap[i];
             QList<QTableWidgetItem *> selectedItems = m_ListWidgetMap[Key]->selectedItems();
             QStringList pattern;
             foreach(QTableWidgetItem* item, selectedItems)
             {
                 if(item->text() != "All")
                 {
                     pattern.append(item->text());
                 }
             }
             if(pattern.size() == 0)
             {
                 pattern = m_DataMap[Key];
             }
             filter.push_back(pattern);
         }
         QVector<QStringList> newFilter;
         count = m_LabelIndexMap.size();
         QStringList nextPattern;
         for (int i = 0; i < count; i++)
         {
             if(i < m_LabelIndexMap[label])
             {
                 newFilter.push_back(filter.at(i));
             }
             else
             {
                 QStringList curPattern;
                 if(i == m_LabelIndexMap[label])
                 {
                     curPattern = filter.at(i);
                 }
                 else
                 {
                     curPattern = nextPattern;
                     nextPattern.clear();
                 }
                 newFilter.push_back(curPattern);
                 if(i < count - 1)
                 {
                     QMap<QString, QStringList> map = m_DataRelationMap.at(i);
                     foreach (QString key, newFilter.last())
                     {
                         nextPattern.append(map[key]);
                     }
                     nextPattern.removeDuplicates();
                     nextPattern.sort();
                     SetData(m_IndexLabelMap[i + 1], nextPattern);
                 }
             }
         }
         emit FilterChanged(newFilter);
        }
    }
signals:
    void FilterChanged(const QVector<QStringList>& filter);

protected:
    QStringList m_HeaderLabels;
    QMap<QString, QStringList> m_DataMap;
    QMap<QString, int> m_LabelIndexMap;
    QMap<int, QString> m_IndexLabelMap;
    QMap<QString, QTableWidget*> m_ListWidgetMap;
    QMap<QString, int> m_LabelSelectedRowMap;
    QVector<QMap<QString, QStringList>> m_DataRelationMap;
};

}

#endif // FILTERWIDGET_HPP
