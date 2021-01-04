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
    QLine convert2pixel(QLine line);

public slots:
    void dataUpdate_left(QVector<QVector <double> > *dataPacket);
    void dataUpdate_right(QVector<QVector <double> > *dataPacket);
    void drawLine(QVector <QLine>);

private:
    Plot *d_plot_left;
    Plot *d_plot_right;
    int d_noiseMargin;
};

#endif // POPUPWINDOW_H
