#ifndef BLOCKAREA_H
#define BLOCKAREA_H
#include<QObject>
#include<QPaintEvent>
#include<QLabel>
#include<QPainter>
#include<QApplication>

class CBlockArea : public QWidget
{
    Q_OBJECT
public:
    CBlockArea(const QString& imPath, int fontPixSize = 16, QWidget *parent = 0);
    ~CBlockArea();
    void SetText(QString text);
    void AdjustSizeFunc();

public slots:
    void ShowArea();
    void HideArea();

protected:
    void paintEvent(QPaintEvent *pe);

private:
    QPixmap m_pix;
    QLabel m_label;
};

#endif // BLOCKAREA_H
