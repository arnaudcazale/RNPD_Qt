#ifndef POPUPWINDOW_H
#define POPUPWINDOW_H

#include <QWidget>
#include <QPlainTextEdit>
#include "plot.h"
#include <QMainWindow>
#include <QPaintEvent>

QT_BEGIN_NAMESPACE

namespace Ui {
class PopupWindow;
}

QT_END_NAMESPACE

class PopupWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PopupWindow(QWidget *parent = nullptr);
    ~PopupWindow();
    int getNoiseMargin(void);


protected:
    QLine convertLine2pixel(QLine line);
    QPoint convertPoint2pixel(QPoint point);

public slots:
    void dataUpdate_left(QVector<QVector <double> > *dataPacket);
    void dataUpdate_right(QVector<QVector <double> > *dataPacket);
    void drawLine(QVector <QLine>);
    void drawPoint(QVector <QPoint>);

private:
    Plot *d_plot_left;
    Plot *d_plot_right;
    int d_noiseMargin;
};

#endif // POPUPWINDOW_H
