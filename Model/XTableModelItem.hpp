#ifndef XTABLEMODELITEM_HPP
#define XTABLEMODELITEM_HPP

#include <QVector>
#include <QString>
#include <QFont>
#include <QColor>

namespace FinTechUI
{

class XTableModelItem;

typedef QVector<XTableModelItem*> XTableModelRow;

class XTableModelItem
{
public:
    explicit XTableModelItem(const QString& text = "",
                    Qt::Alignment align = Qt::AlignCenter,
                    const QColor& background = Qt::white,
                    const QColor& foreground = Qt::black)
    {
        m_text = text;
        m_align = align;
        m_background = background;
        m_foreground = foreground;
    }

    explicit XTableModelItem(int value,
                    Qt::Alignment align = Qt::AlignCenter,
                    const QColor& background = Qt::white,
                    const QColor& foreground = Qt::black)
    {
        m_text = QString::number(value);
        m_align = align;
        m_background = background;
        m_foreground = foreground;
    }

    explicit XTableModelItem(long value,
                    Qt::Alignment align = Qt::AlignCenter,
                    const QColor& background = Qt::white,
                    const QColor& foreground = Qt::black)
    {
        m_text = QString::number(value);
        m_align = align;
        m_background = background;
        m_foreground = foreground;
    }

    explicit XTableModelItem(double value,
                    Qt::Alignment align = Qt::AlignCenter,
                    const QColor& background = Qt::white,
                    const QColor& foreground = Qt::black)
    {
        m_text = QString::number(value);
        m_align = align;
        m_background = background;
        m_foreground = foreground;
    }

    void setText(const QString& text)
    {
        m_text = text;
    }

    void setText(int value)
    {
        m_text = QString::number(value);
    }

    void setText(long value)
    {
        m_text = QString::number(value);
    }

    void setText(long long value)
    {
        m_text = QString::number(value);
    }

    void setText(double value)
    {
        m_text = QString::number(value);
    }

    QString text()const
    {
        return m_text;
    }

    void setTextAlignment(int alignment)
    {
        m_align = alignment;
    }

    int textAlignment()const
    {
        return m_align;
    }

    void setFont(const QFont& font)
    {
        m_font = font;
    }

    QFont font()const
    {
        return m_font;
    }

    void setBackground(const QColor& color)
    {
        m_background = color;
    }

    QColor background()const
    {
        return m_background;
    }

    void setForeground(const QColor& color)
    {
        m_foreground = color;
    }

    QColor foreground()const
    {
        return m_foreground;
    }

protected:
    QString m_text;
    int m_align;
    QFont m_font;
    QColor m_background;
    QColor m_foreground;
};

}

#endif // XTABLEMODELITEM_HPP
