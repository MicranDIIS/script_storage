#include "blockarea.h"

CBlockArea::CBlockArea(const QString &imPath, int fontPixSize, QWidget *parent) : QWidget(parent)
{
    m_pix.load(imPath);
    resize(parent->geometry().size());
    m_label.setParent(this);
    m_label.setStyleSheet(QString("QLabel{font: %1pt \"Times New Roman\" bold; color:white;background:transparent}").arg(fontPixSize));
    hide();
}

CBlockArea::~CBlockArea()
{

}

void CBlockArea::SetText(QString text)
{
    m_label.setText(QT_TR_NOOP(text));
    AdjustSizeFunc();
}

void CBlockArea::ShowArea()
{
    QWidget* parentWidget = qobject_cast<QWidget*>(this->parent());
    if(parentWidget)
        resize(parentWidget->geometry().size());
    raise();
    show();
    activateWindow();
    repaint();
    qApp->processEvents();
}

void CBlockArea::HideArea()
{
    hide();
}

void CBlockArea::paintEvent(QPaintEvent *pe)
{
    Q_UNUSED(pe)
    if(m_pix.isNull()) return;
    QPainter paint(this);
    m_pix = m_pix.scaled(width(), height(), Qt::KeepAspectRatioByExpanding);
    QPoint centerOfWidget = rect().center();
    QRect rectOfPixmap = m_pix.rect();
    rectOfPixmap.moveCenter(centerOfWidget);
    paint.drawPixmap(rectOfPixmap.topLeft(), m_pix);
}

void CBlockArea::AdjustSizeFunc()
{
    QWidget* parentWidget = qobject_cast<QWidget*>(this->parent());
    if(parentWidget)
        resize(parentWidget->geometry().size());
    m_label.adjustSize();
    m_label.move(width()/2 - m_label.geometry().width()/2,
              height()/3 - m_label.geometry().height()/3);
}
