#ifndef TABPAGEWIDGET_HPP
#define TABPAGEWIDGET_HPP

#include <QWidget>
#include <QScrollArea>

namespace FinTechUI{
class TabPageWidget : public QScrollArea
{
    Q_OBJECT
public:
    explicit TabPageWidget(QWidget *parent = NULL) : QScrollArea(parent)
    {
        setWidgetResizable(true);
    }

    void setTabLabel(const QString& tab)
    {
        this->tab = tab;
    }

    QString tabLabel()const
    {
        return tab;
    }
protected:
    QString tab;
};
}

#endif // TABPAGEWIDGET_HPP
