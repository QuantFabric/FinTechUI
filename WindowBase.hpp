#ifndef WINDOWSBASE_HPP
#define WINDOWSBASE_HPP

#include <QString>
#include <QFile>
#include <QApplication>
#include <QDebug>
#include <QColor>
#include <QPalette>
#include <QFrame>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QEvent>
#include <QMouseEvent>
#include <QPoint>
#include <QPixmap>
#include <QString>
#include <QDesktopWidget>

namespace FinTechUI
{

class CommonHelper
{
public:
    static void setStyleSheet(const QString& filepath)
    {
        //加载样式文件
        QFile qss(filepath);
        if(qss.open(QFile::ReadOnly))
        {
            QString stylesheet = QLatin1String(qss.readAll());
            QString paletteColor = stylesheet.mid(20, 7);
            qApp->setPalette(QPalette(QColor(paletteColor)));
            qApp->setStyleSheet(stylesheet);
        }
    }
};

class TitleBar : public QWidget
{
    Q_OBJECT
public:
    explicit TitleBar(QWidget *parent = NULL) : QWidget(parent)
    {
        setFixedHeight(30);
        setWindowFlags(Qt::FramelessWindowHint);
        m_iconLabel = new QLabel(this);
        m_iconLabel->setFixedSize(30, 20);
        m_iconLabel->setScaledContents(true);
        m_iconLabel->setFrameShadow(QFrame::Raised);

        m_titleLabel = new QLabel(this);
        m_titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        m_titleLabel->setFrameShadow(QFrame::Raised);

        m_minimizeButton = new QPushButton(this);
        m_minimizeButton->setFixedSize(27, 22);
        m_minimizeButton->setObjectName("minimizeButton");

        m_maximizeButton = new QPushButton(this);
        m_maximizeButton->setFixedSize(27, 22);
        m_maximizeButton->setObjectName("maximizeButton");

        m_closeButton = new QPushButton(this);
        m_closeButton->setFixedSize(27, 22);
        m_closeButton->setObjectName("closeButton");

        QHBoxLayout* layout = new QHBoxLayout;
        layout->addWidget(m_iconLabel);
        layout->addStretch(1);
        layout->addWidget(m_titleLabel);
        layout->addStretch(1);
        layout->addWidget(m_minimizeButton);
        layout->addWidget(m_maximizeButton);
        layout->addWidget(m_closeButton);
        setLayout(layout);

        setProperty("titleBar", true);
        setObjectName("titleBar");

        connect(m_minimizeButton, SIGNAL(clicked(bool)), this, SLOT(OnClicked()));
        connect(m_maximizeButton, SIGNAL(clicked(bool)), this, SLOT(OnClicked()));
        connect(m_closeButton, SIGNAL(clicked(bool)), this, SLOT(OnClicked()));
    }

    void setWindowTitle(const QString& title)
    {
        m_titleLabel->setAlignment(Qt::AlignCenter);
        m_titleLabel->setText(title);
    }

    void setTitleBarIcon(const QString& iconPath)
    {
        QPixmap map(iconPath);
        m_iconLabel->setPixmap(map);
    }

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event)
    {
        m_maximizeButton->click();
    }

    virtual void mousePressEvent(QMouseEvent *event)
    {
        // 鼠标左键按下事件
        if (event->button() == Qt::LeftButton)
        {
            // 记录鼠标左键状态
            m_leftButtonPressed = true;
            //记录鼠标在屏幕中的位置
            m_start = event->globalPos();
        }
    }

    virtual void mouseMoveEvent(QMouseEvent *event)
    {
        // 持续按住才做对应事件
        if(m_leftButtonPressed)
        {
            //将父窗体移动到父窗体原来的位置加上鼠标移动的位置：event->globalPos()-m_start
            parentWidget()->move(parentWidget()->geometry().topLeft() +
                                event->globalPos() - m_start);
            //将鼠标在屏幕中的位置替换为新的位置
            m_start = event->globalPos();
        }
    }

    virtual void mouseReleaseEvent(QMouseEvent *event)
    {
        // 鼠标左键释放
        if (event->button() == Qt::LeftButton)
        {
            // 记录鼠标状态
            m_leftButtonPressed = false;
        }
    }

    virtual bool eventFilter(QObject *obj, QEvent *event)
    {
        switch(event->type())
        {
        //设置标题
        case QEvent::WindowTitleChange:
        {
            QWidget *pWidget = qobject_cast<QWidget *>(obj);
            if (pWidget)
            {
                m_titleLabel->setText(pWidget->windowTitle());
                return true;
            }
        }
        //设置图标
        case QEvent::WindowIconChange:
        {
            QWidget *pWidget = qobject_cast<QWidget *>(obj);
            if (pWidget)
            {
                QIcon icon = pWidget->windowIcon();
                m_iconLabel->setPixmap(icon.pixmap(m_iconLabel->size()));
                return true;
            }
        }
        // 窗口状态变化、窗口大小变化
        case QEvent::WindowStateChange:
        case QEvent::Resize:
            updateMaximize();
            return true;
        }
        return QWidget::eventFilter(obj, event);
    }

    virtual void showEvent(QShowEvent* event)
    {
        this->setAttribute(Qt::WA_Mapped);
        QWidget::showEvent(event);
    }

    void updateMaximize()
    {
        QWidget *pWindow = parentWidget()->window();
        if (pWindow->isTopLevel())
        {
            bool bMaximize = pWindow->isMaximized();
            if (bMaximize)
            {
                m_maximizeButton->setToolTip(tr("Restore"));
                m_maximizeButton->setProperty("maximizeProperty", "restore");
            }
            else
            {
                m_maximizeButton->setProperty("maximizeProperty", "maximize");
                m_maximizeButton->setToolTip(tr("Maximize"));
            }
            pWindow->updateGeometry();
            m_maximizeButton->setStyle(QApplication::style());
        }
    }
protected slots:
    void OnClicked()
    {
        QPushButton *pButton = qobject_cast<QPushButton *>(sender());
        QWidget *pWindow = parentWidget()->window();
        if(pWindow->isTopLevel())
        {
            if (pButton == m_minimizeButton)
            {
                pWindow->showMinimized();
            }
            else if (pButton == m_maximizeButton)
            {
                pWindow->isMaximized() ? pWindow->showNormal() : pWindow->showMinimized();
            }
            else if (pButton == m_closeButton)
            {
                pWindow->close();
            }
        }
    }

private:
    QLabel* m_iconLabel;
    QLabel* m_titleLabel;
    QPushButton* m_minimizeButton;
    QPushButton* m_maximizeButton;
    QPushButton* m_closeButton;
    QPoint m_start;//起始点
    QPoint m_end;//结束点
    bool m_leftButtonPressed;//鼠标左键按下标记
};
    

class WindowBase : public QFrame
{
    Q_OBJECT
public:
    WindowBase(QWidget* parent = NULL) : QFrame(parent)
    {
        m_TitleBar = new TitleBar(this);
        m_ContentWidget = new QWidget(this);
        m_ContentWidget->setObjectName("Contents");
        m_Layout = new QVBoxLayout;
        m_Layout->addWidget(m_TitleBar);
        m_Layout->addWidget(m_ContentWidget);
        m_Layout->setSpacing(0);
        m_Layout->setContentsMargins(0, 0, 0, 0);

        setLayout(m_Layout);
        #if defined(Q_OS_LINUX)
            setWindowFlags(windowFlags() | Qt::Dialog | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
            m_TitleBar->hide();
        #elif defined(Q_OS_WIN64)
            setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
        #endif
    }

    void setWindowTitle(const QString& title)
    {
        #if defined(Q_OS_LINUX)
            QWidget::setWindowTitle(title);
        #elif defined(Q_OS_WIN64)
            m_TitleBar->setWindowTitle(title);
        #endif
    }

    void setTitleBarIcon(const QString& iconPath)
    {
        m_TitleBar->setTitleBarIcon(iconPath);
    }

    QWidget* contentWidget()
    {
        return m_ContentWidget;
    }

    void setWindowTitleHeight(int h)
    {   
        m_TitleBar->setFixedHeight(h);
    }
protected:
    QWidget* m_ContentWidget;//内容组件
private:
    TitleBar* m_TitleBar;//标题栏
    QVBoxLayout* m_Layout;//布局管理器
};

}


#endif // WINDOWSBASE_HPP
