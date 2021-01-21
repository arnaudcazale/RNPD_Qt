/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "console.h"
#include "settingsdialog.h"
#include "popupwindow.h"
#include "displaywindow.h"

#include <QLabel>
#include <QMessageBox>
#include <QtDebug>
#include <QDesktopWidget>
#include <QStyle>

#include "math.h"


//! [0]
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    m_status(new QLabel),
    m_console(new Console),
    m_settings(new SettingsDialog),
    m_popupwindow(new PopupWindow),
    m_popupwindowGravity(new PopupWindow),
    m_display(new DisplayWindow),
    m_display_pointure(new DisplayWindow),
    m_display_gravity(new DisplayWindow),
//! [1]
    m_serial(new QSerialPort(this)),
    m_data(new QVector<unsigned int>)
//! [1]
{
//! [0]
    m_count = 0;
    filling = false;
    gravity = true;
    size = false;

    for(int i = 0; i < LGN_NBR; i++) //of course you might not want to init the vectors in a loop - this is just an example
    {
        QVector<double> foo; //create a QVector of doubles
        /*for(int j = 0; j < COL_NBR; j++)
        {
            foo.append(0);
        }*/
        m_data_left.append(foo);
        m_data_right.append(foo);
        m_data_bin_left.append(foo);
        m_data_bin_right.append(foo);
        m_data_filter_left.append(foo);
        m_data_filter_right.append(foo);

    }

    m_ui->setupUi(this);
    m_console->setEnabled(false);
    m_popupwindow->resize(900, 800);
    m_popupwindow->show();
    m_popupwindowGravity->resize(900, 800);
    m_popupwindowGravity->show();
    m_display->show();
    m_display->resize(500, 250);
    m_display_pointure->show();
    m_display_pointure->resize(500, 250);
    m_display_gravity->show();
    m_display_gravity->resize(500, 250);
    setCentralWidget(m_console);

    m_popupwindow->move(0,0);
    m_popupwindowGravity->move(950,0);
    m_display->move(0,800);
    m_display_pointure->move(500,800);
    m_display_gravity->move(1000,800);

    m_ui->actionConnect->setEnabled(true);
    m_ui->actionDisconnect->setEnabled(false);
    m_ui->actionQuit->setEnabled(true);
    m_ui->actionConfigure->setEnabled(true);

    m_ui->statusBar->addWidget(m_status);

    initActionsConnections();

    connect(m_serial, &QSerialPort::errorOccurred, this, &MainWindow::handleError);

//! [2]
    connect(m_serial, &QSerialPort::readyRead, this, &MainWindow::readData);
//! [2]
    connect(m_console, &Console::getData, this, &MainWindow::writeData);

//! [3]
    connect(this, SIGNAL(dataReady_left(QVector<QVector <double> > *)), m_popupwindow, SLOT(dataUpdate_left(QVector<QVector <double> > *)));
    connect(this, SIGNAL(dataReady_right(QVector<QVector <double> > *)), m_popupwindow, SLOT(dataUpdate_right(QVector<QVector <double> > *)));
    connect(this, SIGNAL(dataReady_line(QVector <QLine>)), m_popupwindow, SLOT(drawLine(QVector <QLine>)));
    connect(this, SIGNAL(dataReadyGravity_left(QVector<QVector <double> > *)), m_popupwindowGravity, SLOT(dataUpdate_left(QVector<QVector <double> > *)));
    connect(this, SIGNAL(dataReadyGravity_right(QVector<QVector <double> > *)), m_popupwindowGravity, SLOT(dataUpdate_right(QVector<QVector <double> > *)));
    connect(this, SIGNAL(dataReadyGravity_line(QVector <QLine>)), m_popupwindowGravity, SLOT(drawLine(QVector <QLine>)));

}
//! [3]

MainWindow::~MainWindow()
{
    delete m_settings;
    delete m_ui;
}

//! [4]
void MainWindow::openSerialPort()
{
    const SettingsDialog::Settings p = m_settings->settings();
    m_serial->setPortName(p.name);
    m_serial->setBaudRate(p.baudRate);
    m_serial->setDataBits(p.dataBits);
    m_serial->setParity(p.parity);
    m_serial->setStopBits(p.stopBits);
    m_serial->setFlowControl(p.flowControl);
    if (m_serial->open(QIODevice::ReadWrite)) {
        m_console->setEnabled(true);
        m_console->setLocalEchoEnabled(p.localEchoEnabled);
        m_ui->actionConnect->setEnabled(false);
        m_ui->actionDisconnect->setEnabled(true);
        m_ui->actionConfigure->setEnabled(false);
        showStatusMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                          .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                          .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
    } else {
        QMessageBox::critical(this, tr("Error"), m_serial->errorString());

        showStatusMessage(tr("Open error"));
    }
}
//! [4]

//! [5]
void MainWindow::closeSerialPort()
{
    if (m_serial->isOpen())
        m_serial->close();
    m_console->setEnabled(false);
    m_ui->actionConnect->setEnabled(true);
    m_ui->actionDisconnect->setEnabled(false);
    m_ui->actionConfigure->setEnabled(true);
    showStatusMessage(tr("Disconnected"));
}
//! [5]

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Simple Terminal"),
                       tr("The <b>Simple Terminal</b> example demonstrates how to "
                          "use the Qt Serial Port module in modern GUI applications "
                          "using Qt, with a menu bar, toolbars, and a status bar."));
}

//! [6]
void MainWindow::writeData(const QByteArray &data)
{
    m_serial->write(data);
}
//! [6]

//! [7]
void MainWindow::readData()
{
     while(m_serial->bytesAvailable() >= 2)
     {
         //Append into buffer
         if(filling)
         {
             QByteArray data = m_serial->read(2);
             unsigned int word = (static_cast<unsigned int>(data[0]) & 0xFF) + ((static_cast<unsigned int>(data[1]) & 0xFF) << 8 ) ;
             m_data->append((double)word);
             if(m_data->size() >= 1536)//1536
             {
                 filling = false;
                 //qDebug() << "*m_data" << *m_data;

                 splitData();
                 //splitDataFillZero();
                 //fillLeftDataMeanNeightboorhood();
                 //fillRightDataMeanNeightboorhood();

                 if(gravity)
                 {
                     gravity = false;
                     size = true;
                     computeGravity();
                 }

                 else if(size)
                 {
                     m_count++;
                     computeSize();
                 }
             }
         }else
         {
             //Data goes to console
             QByteArray data_console = m_serial->peek(1);
             m_console->putData(data_console);
             //qDebug() << data_console;
             //Catch trame start
             if( (m_serial->peek(2) !=  "\xFE\xFF") )
             {
                 // si les octets déjà lus ne commencent pas
                 // par la bonne séquence, on les ignore jusqu'à ce qu'on la trouve
                 m_serial->getChar(0);
             } else
             {
                 QByteArray data = m_serial->read(2);
                 //qDebug()<<"data start"<<data;
                 filling = true;
             }
         }
     }
}

void MainWindow::computeGravity(){

    //Pronation computing
    emit dataReadyGravity_left(&m_data_left);
    emit dataReadyGravity_right(&m_data_right);

    int grav = calc_gravity();
    dataDisplay_gravity.append("GRAVITY = " + QString::number(grav) + "\n");

    int devLeft = calc_pronation_left(&m_data_filter_left);
    int devRight = calc_pronation_right(&m_data_filter_right);
    int dev = (devLeft + devRight) /2;

    emit(dataReadyGravity_line(m_linesGravity));

    qDebug() << "dev_total" << dev;
    dataDisplay_gravity.append("deviation moyenne = " + QString::number(dev) + "\n");

    if( dev < -2)
        dataDisplay_gravity.append("SUPINAL\n");
    else if( dev > 0)
        dataDisplay_gravity.append("CONTROL\n");
    else
        dataDisplay_gravity.append("NEUTRE\n");

    m_display_gravity->putData(dataDisplay_gravity);
    dataDisplay_gravity.clear();
    m_linesGravity.clear();

}

void MainWindow::computeSize(){

    //size computing
    emit dataReady_left(&m_data_left);
    emit dataReady_right(&m_data_right);

    double size = calc_size();
    m_pointure.append(size);
    emit dataReady_line(m_lines);

    if( (m_count % 2) == 0)
    {
        qDebug()<<m_pointure;
        double pointure_mean = ( m_pointure.at(m_pointure.size()-1) + m_pointure.at(m_pointure.size()-2)) / 2;
        double pointure_round = round(pointure_mean*2)/2;
        dataDisplay_pointure.append(QString::number(pointure_round) + "\n");
        m_display_pointure->putData(dataDisplay_pointure);

        size = false;
        gravity = true;
    }

    m_display->putData(dataDisplay);

    dataDisplay.clear();
    dataDisplay_pointure.clear();
    m_lines.clear();
}


void MainWindow::splitData()
{
    m_data_left.clear();
    m_data_right.clear();

    QVector <double>  left;
    QVector <double>  right;

    for(int i = 0; i < m_data->size()/2; i+=16)
    {
        for(int j = 0; j <16; j++)
        {
            left.append(m_data->at(i+j));
        }
        m_data_left.append(left);
        left.clear();
    }

    for(int i = m_data->size()/2; i < m_data->size(); i+=16)
    {
        for(int j = 0; j < 16; j++)
        {
            right.append(m_data->at(i+j));
        }
        m_data_right.append(right);
        right.clear();
    }

    m_data->clear();
}

void MainWindow::splitDataFillZero()
{
    m_data_left.clear();
    m_data_right.clear();

    QVector <QVector <double> > left;
    QVector <QVector <double> > right;

    for(int i = 0; i < LGN_NBR; i++) //of course you might not want to init the vectors in a loop - this is just an example
    {
        QVector<double> foo; //create a QVector of doubles
        left.append(foo);
        right.append(foo);
    }

    bool toggle = false;

    for(int i = 0; i < LGN_NBR ; i++)
    {
        if(!toggle)
        {
            for(int j = 0; j < COL_NBR/2; j++)
            {
                left[i].append(m_data->at( (i* (COL_NBR/2) ) + j));
                left[i].append(0);
                //qDebug()<< i*(COL_NBR/2) + j;
            }
            toggle = true;
        }else
        {
            for(int j = 0; j < COL_NBR/2; j++)
            {
                left[i].append(0);
                left[i].append(m_data->at( (i* (COL_NBR/2) ) + j));
            }
            toggle = false;
        }
        //qDebug()<< "left["<< i << "] = " << left.at(i);
    }

    for(int i = 0; i < LGN_NBR ; i++)
    {
        if(!toggle)
        {
            for(int j = 0; j < COL_NBR/2; j++)
            {
                right[i].append(0);
                right[i].append(m_data->at( (i* (COL_NBR/2) ) + j + (LGN_NBR*(COL_NBR/2))));
                //qDebug()<< (i* (COL_NBR/2) ) + j + (LGN_NBR*(COL_NBR/2));
            }
            toggle = true;
        }else
        {
            for(int j = 0; j < COL_NBR/2; j++)
            {
                right[i].append(m_data->at( (i* (COL_NBR/2) ) + j + (LGN_NBR*(COL_NBR/2))));
                right[i].append(0);
            }
            toggle = false;
        }
        //qDebug()<< "right["<< i << "] = " << right.at(i);
    }

    m_data->clear();


    //Reverse order
    for(int i=left.size()-1; i>=0;i--){
      m_data_left.append(left.at(i));
    }

    //Reverse order
    for(int i=right.size()-1; i>=0;i--){
      m_data_right.append(right.at(i));
    }

}
//! [7]

void MainWindow::fillLeftDataMeanNeightboorhood()
{
    bool toggle = false;
    int mean_data;

    for(int i = 0; i < LGN_NBR ; i++)
    {
        if(!toggle)
        {
            //qDebug() << "i"<< i;
            for(int j = 0; j < COL_NBR; j = j+2)
            {
                //qDebug() << "j"<< j;
                //Si 1ere ligne et 1ere colonne
                if( (i == 0) && (j == 0) )
                {
                    mean_data = ( m_data_left.at(i+1).at(j) + m_data_left.at(i).at(j+1) ) / 2;
                    m_data_left[i].replace(j, mean_data);
                }else if( (i == 0) && (j != 0) )
                {
                    mean_data = ( m_data_left.at(i).at(j-1) + m_data_left.at(i+1).at(j) + m_data_left.at(i).at(j+1) ) / 3;
                    m_data_left[i].replace(j, mean_data);

                //Sinon Si 1ere colonne
                }else if( (i != 0) && (j == 0) )
                {
                    mean_data = ( m_data_left.at(i+1).at(j) + m_data_left.at(i-1).at(j) + m_data_left.at(i).at(j+1) ) / 3;
                    m_data_left[i].replace(j, mean_data);
                }else if( (i != 0) && (j != 0) )
                {
                    mean_data = ( m_data_left.at(i).at(j-1) + m_data_left.at(i+1).at(j) + m_data_left.at(i-1).at(j) + m_data_left.at(i).at(j+1) ) / 4;
                    m_data_left[i].replace(j, mean_data);
                }
            }

            toggle = true;

        }else
        {
            for(int j = 1; j < COL_NBR; j = j+2)
            {
                if( (i == 47) && (j == 15) )
                {
                    mean_data = ( m_data_left.at(i).at(j-1) + m_data_left.at(i-1).at(j) ) / 2;
                    m_data_left[i].replace(j, mean_data);
                }else if( (i == 47) && (j != 15) )
                {
                    mean_data = ( m_data_left.at(i).at(j-1) + m_data_left.at(i-1).at(j) + m_data_left.at(i).at(j+1) ) / 3;
                    m_data_left[i].replace(j, mean_data);

                //Sinon Si dernière colonne
                }else if( (i != 47) && (j == 15) )
                {
                    mean_data = ( m_data_left.at(i+1).at(j) + m_data_left.at(i).at(j-1) + m_data_left.at(i-1).at(j) ) / 3;
                    m_data_left[i].replace(j, mean_data);
                }else if( (i != 47) && (j != 15) )
                {
                    mean_data = ( m_data_left.at(i).at(j-1) + m_data_left.at(i+1).at(j) + m_data_left.at(i-1).at(j) + m_data_left.at(i).at(j+1) ) / 4;
                    m_data_left[i].replace(j, mean_data);
                }
            }

            toggle = false;
        }
    }
}

void MainWindow::fillRightDataMeanNeightboorhood()
{
    bool toggle = false;
    int mean_data;

    for(int i = 0; i < LGN_NBR ; i++)
    {
        if(!toggle)
        {
            //qDebug() << "i"<< i;
            for(int j = 1; j < COL_NBR; j = j+2)
            {
                //qDebug() << "j"<< j;
                //Si 1ere ligne et 1ere colonne
                if( (i == 0) && (j == 15) )
                {
                    mean_data = ( m_data_right.at(i+1).at(j) + m_data_right.at(i).at(j-1) ) / 2;
                    m_data_right[i].replace(j, mean_data);
                }else if( (i == 0) && (j != 15) )
                {
                    mean_data = ( m_data_right.at(i).at(j-1) + m_data_right.at(i+1).at(j) + m_data_right.at(i).at(j+1) ) / 3;
                    m_data_right[i].replace(j, mean_data);

                //Sinon Si 1ere colonne
                }else if( (i != 0) && (j == 15) )
                {
                    mean_data = ( m_data_right.at(i+1).at(j) + m_data_right.at(i-1).at(j) + m_data_right.at(i).at(j-1) ) / 3;
                    m_data_right[i].replace(j, mean_data);
                }else if( (i != 0) && (j != 15) )
                {
                    mean_data = ( m_data_right.at(i).at(j-1) + m_data_right.at(i+1).at(j) + m_data_right.at(i-1).at(j) + m_data_right.at(i).at(j+1) ) / 4;
                    m_data_right[i].replace(j, mean_data);
                }
            }

            toggle = true;

        }else
        {
            for(int j = 0; j < COL_NBR; j = j+2)
            {
                if( (i == 47) && (j == 0) )
                {
                    mean_data = ( m_data_right.at(i).at(j+1) + m_data_right.at(i-1).at(j) ) / 2;
                    m_data_right[i].replace(j, mean_data);
                }else if( (i == 47) && (j != 0) )
                {
                    mean_data = ( m_data_right.at(i).at(j-1) + m_data_right.at(i-1).at(j) + m_data_right.at(i).at(j+1) ) / 3;
                    m_data_right[i].replace(j, mean_data);

                //Sinon Si dernière colonne
                }else if( (i != 47) && (j == 0) )
                {
                    mean_data = ( m_data_right.at(i+1).at(j) + m_data_right.at(i).at(j+1) + m_data_right.at(i-1).at(j) ) / 3;
                    m_data_right[i].replace(j, mean_data);
                }else if( (i != 47) && (j != 0) )
                {
                    mean_data = ( m_data_right.at(i).at(j-1) + m_data_right.at(i+1).at(j) + m_data_right.at(i-1).at(j) + m_data_right.at(i).at(j+1) ) / 4;
                    m_data_right[i].replace(j, mean_data);
                }
            }

            toggle = false;
        }
    }
}

void MainWindow::binarizeFromNoiseMargin(QVector <QVector <double> > *matrix, QVector <QVector <double> > *matrix_bin)
{
    matrix_bin->clear();

    int noiseMargin = m_popupwindow->getNoiseMargin();
    qDebug() << noiseMargin;

    for(int i = 0; i < LGN_NBR ; i++)
    {
        QVector<double> foo;

        for(int j = 0; j < COL_NBR; j++)
        {
            if( matrix->at(i).at(j) > noiseMargin)
                foo.append(1);
            else
                foo.append(0);
        }

        matrix_bin->append(foo);
        //qDebug() << matrix_bin->at(i);
    }
}

void MainWindow::binarizeFromMean(QVector <QVector <double> > *matrix, QVector <QVector <double> > *matrix_bin)
{
    matrix_bin->clear();

    // calculate means of both matrixes
    double mean = calc_mean(matrix);
    qDebug() << "matrix_mean" << mean;
    //unsigned int mean_right = calc_mean(&m_data_right);
    //unsigned int mean = (mean_left + mean_right)/2;

    for(int i = 0; i < LGN_NBR ; i++)
    {
        QVector<double> foo;

        for(int j = 0; j < COL_NBR; j++)
        {
            if( matrix->at(i).at(j) > mean)
                foo.append(1);
            else
                foo.append(0);
        }

        matrix_bin->append(foo);
    }
}

unsigned int MainWindow::get_median_line(QVector <QVector <double> > *matrix_bin)
{
    QVector <double> *sum_lines = new QVector <double> ();
    QVector <double> *extremum_line = new QVector <double> ();
    QVector <double> *extremum_value = new QVector <double> ();
    QVector <double> *extremum_type = new QVector <double> ();

    int sum = 0;
    int psum = 0;
    int dir = 1, pdir = 1;
    int max = 0, max_idx = 0;
    int min = 255, min_idx = 0;

    /* make sum tab for each lines*/
    for( int i = 0; i < LGN_NBR; i++)
    {
        for(int j = 0; j < COL_NBR; j++)
        {
            sum += matrix_bin->at(i).at(j);
        }
        sum_lines->append(sum);
        sum = 0;
    }

    //qDebug() << *sum_lines;

    /* locate extremums */
    for( int i = 0; i < sum_lines->size(); i++)
    {
        /* current direction */
        if( sum_lines->at(i) > psum)
            dir = 1;
        else if( sum_lines->at(i) < psum)
            dir = -1;
        else dir = 0;

        if( dir != pdir)
        {
            extremum_line->append(i);
            extremum_value->append(psum);
            extremum_type->append((dir<pdir) ? 1 : 0);
        }

        psum = sum_lines->at(i);
        pdir = dir;
    }

    //qDebug() << *extremum_line;
    //qDebug() << *extremum_value;
    //qDebug() << *extremum_type;

    /* find max from top of image */
    for( int i = 0; i < extremum_line->size(); i++)
    {
        if( (extremum_value->at(i) > max) && (extremum_type->at(i) == 1) )
        {
            max = extremum_value->at(i);
            max_idx = i;
        }
    }

    //qDebug() << max_idx << max;

    /* find lowest from previous max */
    for( int i = max_idx; i < extremum_value->size()-1; i++)
    {
        if( (extremum_value->at(i) <= min) && (extremum_type->at(i) == 0) )
        {
            min = extremum_value->at(i);
            min_idx = i;
        }
    }

    //qDebug() << min_idx << min;
    //qDebug() << extremum_line->at(min_idx);

    return extremum_line->at(min_idx);
}

void MainWindow::get_coor_extr_left_for_left_foot(QVector <QVector <double> > *matrix_bin, unsigned int *xa, unsigned int *ya, unsigned int *xb, unsigned int *yb )
{
    QVector <double> *tab = new QVector <double> ();
    unsigned int median_line = get_median_line(matrix_bin); //Median_line corresponding to the line number of thumb low position
    //qDebug() << "median_line" << median_line;

    // make sum tab for each lines*/
    for( int i = 0; i < LGN_NBR; i++)
    {
        tab->append(0);
        for(int j = 0; j < COL_NBR; j++)
        {
            tab->replace(i, j);
            if (matrix_bin->at(i).at(j) == 1)
            {
                break;
            }
        }
    }

    qDebug() << *tab;

    // bottom half -> xa, ya
    *xa = COL_NBR - 1  ;
    *ya = median_line;

    for( int i = 0; i < median_line; i++)
    {
        if( tab->at(i) < *xa)
        {
            *xa = tab->at(i);
            *ya = i;
            break; //Find first bottom pixel found is OK
        }
    }

    //check upper line
    /**xa = COL_NBR - 1;
    for( int i = *ya+1; i < median_line; i++)
    {
        if( (tab->at(i) < *xa) )
        {
            *xa = tab->at(i);
            *ya = i;
        }
    }*/
    *xa = tab->at(*ya+1);
    *ya = *ya+1;

    qDebug() << "xa = " << *xa << "ya = " << *ya;
    dataDisplay.append("LEFT FOR LEFT FOOT \n");
    dataDisplay.append("xa = " + QString::number(*xa) + " ya = " + QString::number(*ya) + "\n");

    // top half -> xb, yb
    *xb = COL_NBR - 1 ;
    *yb = LGN_NBR;
    for( int i = *ya; i < LGN_NBR; i++)
    {
        if( (tab->at(i) < *xb) )
        {
            *xb = tab->at(i);
            *yb = i;
        }
    }

    //check upper line
    /**xb = 0 ;
    for( int i = *yb+1; i < *ya; i++)
    {
        if( (tab->at(i) > *xb) && (tab->at(i) < 15) )
        {
            *xb = tab->at(i);
            *yb = i;
        }
    }*/

    qDebug() << "xb = " << *xb << "yb = " << *yb;
    dataDisplay.append("xb = " + QString::number(*xb) + " yb = " + QString::number(*yb) + "\n");

    QLine line(*xa, *ya, *xb, *yb);
    m_lines.append(line);
}

void MainWindow::get_extr_axial_left(QVector <QVector <double> > *matrix_bin, unsigned int *xa, unsigned int *ya, unsigned int *xb, unsigned int *yb )
{
    QVector <double> *tab = new QVector <double> ();
    //unsigned int median_line = get_median_line(matrix_bin); //Median_line corresponding to the line number of thumb low position
    //qDebug() << "median_line" << median_line;

    // make sum tab for each lines*/
    /*for( int i = 0; i < LGN_NBR; i++)
    {
        tab->append(0);
        for(int j = 0; j < COL_NBR; j++)
        {
            tab->replace(i, j);
            if (matrix_bin->at(i).at(j) == 1)
            {
                break;
            }
        }
    }*/

    // make sum tab for each lines
    for( int i = 0; i < LGN_NBR; i++)
    {
        tab->append(0);
        for(int j = ( COL_NBR - 1 ); j >= 0; j--)
        {
            tab->replace(i, ( COL_NBR - 1 ) - j);
            if(matrix_bin->at(i).at(j) == 1)
            {
                break;
            }
        }
    }

    //qDebug() << "sumTab" << *tab;

    // bottom half -> xa, ya
    *xa = COL_NBR - 1  ;
    *ya = 0;
    for( int i = 0; i < LGN_NBR; i++)
    {
        if( tab->at(i) < *xa)
        {
            *xa = tab->at(i);
            *ya = i;
            break; //Find first bottom pixel found is OK
        }
    }

    //*xa = (COL_NBR - 1) - *xa;

    //Check upper line
    *xa = tab->at(*ya+1);
    *ya = *ya+1;

    *xa = (COL_NBR - 1) - *xa;

    qDebug() << "xa = " << *xa << "ya = " << *ya;
    dataDisplay.append("LEFT FOR LEFT FOOT \n");
    dataDisplay.append("xa = " + QString::number(*xa) + " ya = " + QString::number(*ya) + "\n");

    // top half -> xb, yb
    *xb = COL_NBR - 1 ;
    *yb = LGN_NBR - 1;
    for( int i = LGN_NBR - 1; i >= 0; i--)
    {
        if( (tab->at(i) < *xb) || (tab->at(i) == 0))    //Find upper position or if too much angle, thumb can be outside pad
        {
            *xb = tab->at(i);
            *yb = i;
            break;
        }
    }


    //Check down line
    *xb = tab->at(*yb-1);
    *yb = *yb-1;
    *xb = (COL_NBR - 1) - *xb;

    qDebug() << "xb = " << *xb << "yb = " << *yb;
    dataDisplay.append("xb = " + QString::number(*xb) + " yb = " + QString::number(*yb) + "\n");

    QLine line(*xa, *ya, *xb, *yb);
    m_lines.append(line);
}

void MainWindow::get_coor_extr_right_for_left_foot(QVector <QVector <double> > *matrix_bin, unsigned int *xc, unsigned int *yc, unsigned int *xd, unsigned int *yd )
{
    QVector <double> *tab = new QVector <double> ();
    unsigned int median_line = get_median_line(matrix_bin); //Median_line corresponding to the line number of thumb low position
    //qDebug() << "median_line" << median_line;

    // make sum tab for each lines
    for( int i = 0; i < LGN_NBR; i++)
    {
        tab->append(0);
        for(int j = ( COL_NBR - 1 ); j >= 0; j--)
        {
            tab->replace(i, ( COL_NBR - 1 ) - j);
            if(matrix_bin->at(i).at(j) == 1)
            {
                break;
            }
        }
    }

    //qDebug() << *tab;

    // bottom half -> xc, yc
    *xc = COL_NBR - 1 ;
    *yc = median_line;

    for( int i = 0; i < median_line; i++)
    {
        if( tab->at(i) < *xc)
        {
            *xc = tab->at(i);
            *yc = i;
            break;
        }
    }

    *xc = (COL_NBR - 1) - *xc;

    //Check upper line
    *xc = COL_NBR - 1 ;
    for( int i = *yc+1; i < median_line; i++)
    {
        if( tab->at(i) < *xc)
        {
            *xc = tab->at(i);
            *yc = i;
        }
    }

    *xc = (COL_NBR - 1) - *xc;

    qDebug() << "xc = " << *xc << "yc = " << *yc;
    dataDisplay.append("RIGHT FOR LEFT FOOT\n");
    dataDisplay.append("xc = " + QString::number(*xc) + " yc = " + QString::number(*yc) + "\n");

    // top half -> xb, yb
    *xd = COL_NBR - 1 ;
    *yd = LGN_NBR;
    for( int i = median_line; i < LGN_NBR; i++)
    {
        if( tab->at(i) < *xd)
        {
            *xd = tab->at(i);
            *yd = i;
        }
    }

    *xd = ( COL_NBR - 1 ) - *xd;

    qDebug() << "xd = " << *xd << "yd = " << *yd;
    dataDisplay.append("xd = " + QString::number(*xd) + " yd = " + QString::number(*yd) + "\n");

    QLine line(*xc, *yc, *xd, *yd);
    m_lines.append(line);
}

void MainWindow::get_coor_extr_left_for_right_foot(QVector <QVector <double> > *matrix_bin, unsigned int *xa, unsigned int *ya, unsigned int *xb, unsigned int *yb )
{
    QVector <double> *tab = new QVector <double> ();
    unsigned int median_line = get_median_line(matrix_bin); //Median_line corresponding to the line number of thumb low position
    //qDebug() << "median_line" << median_line;

    // make sum tab for each lines*/
    for( int i = 0; i < LGN_NBR; i++)
    {
        tab->append(0);
        for(int j = 0; j < COL_NBR; j++)
        {
            tab->replace(i, j);
            if (matrix_bin->at(i).at(j) == 1)
            {
                break;
            }
        }
    }

    //qDebug() << *tab;

    // bottom half -> xa, ya
    *xa = COL_NBR - 1;
    *ya = median_line;

    for( int i = 0; i < median_line; i++)
    {
        if( tab->at(i) < *xa) //Found first pixel on bottom
        {
            *xa = tab->at(i);
            *ya = i;
            break;
        }
    }

    //check upper line
    *xa = COL_NBR - 1;
    for( int i = *ya+1; i < median_line; i++)
    {
        if( (tab->at(i) < *xa) )
        {
            *xa = tab->at(i);
            *ya = i;
        }
    }


    qDebug() << "xa = " << *xa << "ya = " << *ya;
    dataDisplay.append("LEFT FOR RIGHT FOOT\n");
    dataDisplay.append("xa = " + QString::number(*xa) + " ya = " + QString::number(*ya) + "\n");

    // top half -> xb, yb
    *xb = COL_NBR;
    *yb = median_line;
    for( int i = median_line; i < LGN_NBR; i++)
    {
        if( tab->at(i) < *xb)
        {
            *xb = tab->at(i);
            *yb = i;
        }
    }

    qDebug() << "xb = " << *xb << "yb = " << *yb;
    dataDisplay.append("xb = " + QString::number(*xb) + " yb = " + QString::number(*yb) + "\n");

    QLine line(*xa, *ya, *xb, *yb);
    m_lines.append(line);
}

void MainWindow::get_coor_extr_right_for_right_foot(QVector <QVector <double> > *matrix_bin, unsigned int *xc, unsigned int *yc, unsigned int *xd, unsigned int *yd )
{
    QVector <double> *tab = new QVector <double> ();
    unsigned int median_line = get_median_line(matrix_bin); //Median_line corresponding to the line number of thumb low position
    //qDebug() << "median_line" << median_line;

    // make sum tab for each lines
    for( int i = 0; i < LGN_NBR; i++)
    {
        tab->append(0);
        for(int j = ( COL_NBR - 1 ); j >= 0; j--)
        {
            tab->replace(i, ( COL_NBR - 1 ) - j);
            if(matrix_bin->at(i).at(j) == 1)
            {
                break;
            }
        }
    }

    //qDebug() << *tab;

    // bottom half -> xc, yc
    *xc = COL_NBR - 1 ;
    *yc = median_line;

    for( int i = median_line; i > 0; i--)
    {
        if( tab->at(i) < *xc)
        {
            *xc = tab->at(i);
            *yc = i;
        }
    }

    *xc = (COL_NBR - 1) - *xc;

    qDebug() << "xc = " << *xc << "yc = " << *yc;
    dataDisplay.append("RIGHT FOR RIGHT FOOT\n");
    dataDisplay.append("xc = " + QString::number(*xc) + " yc = " + QString::number(*yc) + "\n");

    // top half -> xb, yb
    *xd = 0 ;
    *yd = *yc;

    for( int i = 0; i < *yc; i++)
    {
        if( (tab->at(i) > *xd) && (tab->at(i) < 15))
        {
            *xd = tab->at(i);
            *yd = i;
        }
    }

    //Check upperlign
    *xd = 0 ;
    for( int i = *yd+1; i < *yc; i++)
    {
        if( (tab->at(i) > *xd) && (tab->at(i) < 15))
        {
            *xd = tab->at(i);
            *yd = i;
        }
    }

    *xd = ( COL_NBR - 1 ) - *xd;

    qDebug() << "xd = " << *xd << "yd = " << *yd;
    dataDisplay.append("xd = " + QString::number(*xd) + " yd = " + QString::number(*yd) + "\n");


    QLine line(*xc, *yc, *xd, *yd);
    m_lines.append(line);
}

void MainWindow::get_extr_axial_right(QVector <QVector <double> > *matrix_bin, unsigned int *xc, unsigned int *yc, unsigned int *xd, unsigned int *yd )
{
    QVector <double> *tab = new QVector <double> ();
    //unsigned int median_line = get_median_line(matrix_bin); //Median_line corresponding to the line number of thumb low position
    //qDebug() << "median_line" << median_line;

    // make sum tab for each lines*/
    for( int i = 0; i < LGN_NBR; i++)
    {
        tab->append(0);
        for(int j = 0; j < COL_NBR; j++)
        {
            tab->replace(i, j);
            if (matrix_bin->at(i).at(j) == 1)
            {
                break;
            }
        }
    }

    //qDebug() << *tab;

    // bottom half -> xc, yc
    *xc = COL_NBR - 1 ;
    *yc = 0;

    for( int i = 0; i < LGN_NBR; i++)
    {
        if( tab->at(i) < *xc)  //Find first pixel
        {
            *xc = tab->at(i);
            *yc = i;
            break;
        }
    }

    //Check upper line
    *xc = tab->at(*yc+1);
    *yc = *yc+1;

    qDebug() << "xc = " << *xc << "yc = " << *yc;
    dataDisplay.append("RIGHT FOR RIGHT FOOT\n");
    dataDisplay.append("xc = " + QString::number(*xc) + " yc = " + QString::number(*yc) + "\n");

    // top half -> xb, yb
    *xd = COL_NBR - 1 ;
    *yd = LGN_NBR - 1;

    for( int i = LGN_NBR - 1; i >= 0; i--)
    {
        if( (tab->at(i) < *xd) ) //Find upper position or if too much angle, thumb can be outside pad
        {
            *xd = tab->at(i);
            *yd = i;
            break;
        }
    }

    //Check down line
    *xd = tab->at(*yd-1);
    *yd = *yd-1;

    qDebug() << "xd = " << *xd << "yd = " << *yd;
    dataDisplay.append("xd = " + QString::number(*xd) + " yd = " + QString::number(*yd) + "\n");


    QLine line(*xc, *yc, *xd, *yd);
    m_lines.append(line);
}

unsigned int MainWindow::calc_mean(QVector <QVector <double> > *matrix)
{
    double mean = 0.0;

    for(int i = 0; i < LGN_NBR ; i++)
    {
        for(int j = 0; j < COL_NBR; j++)
        {
            mean += matrix->at(i).at(j);
        }
    }

    mean /= (double)LGN_NBR*COL_NBR;

    return mean;
}

double MainWindow::calc_size()
{
    int hi = 0, low = 0;
    unsigned int xa, xb, ya, yb, xc, yc, xd, yd;
    double  a1 = 0;
    double left_angle, right_angle, sum_angle = 0;
    //double xy_ratio = 3.3;
    double left_size, right_size;
    double offset = 5;

    //LEFT FOOT
    binarizeFromNoiseMargin(&m_data_left, &m_data_bin_left);
    //get_coor_extr_left_for_left_foot(&m_data_bin_left, &xa, &ya, &xb, &yb);
    //get_coor_extr_right_for_left_foot(&m_data_bin_left, &xc, &yc, &xd, &yd);
    get_extr_axial_left(&m_data_bin_left, &xa, &ya, &xb, &yb);
    get_hilo_pos(&m_data_left, &hi, &low);

    /*xa *= xy_ratio;
    xb *= xy_ratio;
    xc *= xy_ratio;
    xd *= xy_ratio;*/

    if( (yb - ya) != 0)
        a1 = atan(((double) xb - (double) xa) / ((double) yb - (double) ya));
    else
        a1 = 0;

    if ( abs(a1*(180/M_PI)) > 25) a1 = 0; //detection problem

    left_angle =  (M_PI/2) - a1;
    left_size = ( (hi - low) / sin( left_angle)) + offset;

    qDebug() << "left_angle" << left_angle*(180/M_PI);
    qDebug() << "left_size" << left_size;

    dataDisplay.append("LEFT ANGLE = " + QString::number(left_angle*(180/M_PI)) + "\n");
    dataDisplay.append("LEFT SIZE = " + QString::number(left_size) + "\n");

    //RIGHT FOOT
    binarizeFromNoiseMargin(&m_data_right, &m_data_bin_right);
    //get_coor_extr_left_for_right_foot(&m_data_bin_right, &xa, &ya, &xb, &yb);
    //get_coor_extr_right_for_right_foot(&m_data_bin_right, &xc, &yc, &xd, &yd);
    get_extr_axial_right(&m_data_bin_right, &xa, &ya, &xb, &yb);
    get_hilo_pos(&m_data_right, &hi, &low);

    /*xa *= xy_ratio;
    xb *= xy_ratio;
    xc *= xy_ratio;
    xd *= xy_ratio;*/

    if( (yb - ya) != 0)
        a1 = atan(((double) xa - (double) xb) / ((double) ya - (double) yb));
    else
        a1 = 0;

    if ( abs(a1*(180/M_PI)) > 25) a1 = 0; //detection problem

    right_angle = (M_PI/2)-a1;
    right_size = ( (hi - low) / sin(right_angle)) + offset;

    qDebug() << "right_angle" << right_angle*(180/M_PI);
    qDebug() << "right_size" << right_size;

    dataDisplay.append("RIGHT ANGLE = " + QString::number(right_angle*(180/M_PI)) +"\n");
    dataDisplay.append("RIGHT SIZE = " + QString::number(right_size) +"\n");

    double pointure = (left_size + right_size ) / 2;
    //double pointure_arrondie = round( (pointure)*2)/2;

    qDebug() << "total_size" << pointure;
    //qDebug() << "rounded total_size" << pointure_arrondie;

    dataDisplay.append("POINTURE = " + QString::number(pointure) + "\n");
    //dataDisplay.append("POINTURE ARRONDIE = " + QString::number(pointure_arrondie) + "\n");

    return pointure;
}

int MainWindow::calc_gravity(void){

    binarizeFromMean(&m_data_left, &m_data_bin_left);
    binarizeFromMean(&m_data_right, &m_data_bin_right);
    filterMatrix(&m_data_left, &m_data_bin_left, &m_data_filter_left);
    filterMatrix(&m_data_right, &m_data_bin_right, &m_data_filter_right);

    point_t Aleft, Bleft, Aright, Bright;
    gvtGet(&m_data_filter_left, &Aleft, &Bleft);
    gvtGet(&m_data_filter_right, &Aright, &Bright);
    qDebug() << "Aleft line = " << Aleft.line << "Aleft col = " << Aleft.col;
    qDebug() << "Bleft line = " << Bleft.line << "Bleft col = " << Bleft.col;
    qDebug() << "Aright line = " << Aright.line << "Aright col = " << Aright.col;
    qDebug() << "Bright line = " << Bright.line << "Bright col = " << Bright.col;

    int leftMedianLine = (Aleft.line + Bleft.line) / 2;
    int rightMedianLine = (Aright.line + Bright.line) / 2;
    qDebug()<< "leftMedianLine" << leftMedianLine;
    qDebug()<< "rightMedianLine" << rightMedianLine;

    long leftLowerSum = sumMatrix(&m_data_filter_left, 0, leftMedianLine);
    long leftUpperSum = sumMatrix(&m_data_filter_left, leftMedianLine, LGN_NBR);
    long rightLowerSum = sumMatrix(&m_data_filter_right, 0, rightMedianLine);
    long rightUpperSum = sumMatrix(&m_data_filter_right, rightMedianLine, LGN_NBR);

    qDebug() << "leftLowerSum = " << leftLowerSum;
    qDebug() << "leftUpperSum = " << leftUpperSum;
    qDebug() << "rightLowerSum = " << rightLowerSum;
    qDebug() << "rightUpperSum = " << rightUpperSum;

    long totalSum = leftLowerSum + leftUpperSum + rightLowerSum + rightUpperSum;
    qDebug() << "totalSum = " << totalSum;
    double gravity = (double)(leftLowerSum + rightLowerSum) / (double)totalSum;
    qDebug() << "gravity = " << gravity;

    double alpha = 2.0 / 3.0;
    double igravity = 0;
    if( gravity >= alpha)
        igravity = 0;
    else
        igravity = (uint8_t)(((alpha - gravity) / alpha) * 17);
    qDebug() << "igravity = " << igravity;

    return igravity;
}

int MainWindow::calc_pronation_left(QVector <QVector <double> > *matrix_filter){

    line_zone_t   zx[10];
    column_zone_t zy[10];
    QVector <double> *linSum = new QVector <double> ();
    QVector <double> *colSum = new QVector <double> ();
    int moy = 0;
    int val = 0;
    int index = 0;

    // make sum tab for each lines*/
    for( int i = 0; i < LGN_NBR; i++)
    {
        int sum = 0;
        for(int j = 0; j < COL_NBR; j++)
        {
            sum += matrix_filter->at(i).at(j);
        }
        linSum->append(sum);
    }

    // mean of lines sum
    moy = 0;
    for( int i = 0; i < LGN_NBR; i++)
    {
        moy += linSum->at(i);
    }
    moy /= LGN_NBR;
    //qDebug() << moy;

    //find zones
    for( int i = 0; i < LGN_NBR; i++)
    {
        if( (linSum->at(i) > moy) && (val < moy))
        {
            zx[index].index = index;
            zx[index].start_line = i;
        }

        if( ( (linSum->at(i) < moy) || (i==LGN_NBR-1)) && (val > moy))
        {
            zx[index].end_line = i;
            zx[index].n_lines = zx[index].end_line - zx[index].start_line;
            qDebug() << "zone X " << index << "start from " << zx[index].start_line << "to line " << zx[index].end_line;
            index ++;
        }
        val = linSum->at(i);
    }

    /* if more than 2 zones, sort so as to keep the two biggest and order from top */
    if( index > 2)
    {
        /* sort and take the two biggest zones */
        qsort( (void *)zx, index, sizeof(line_zone_t), compare_n_lines);
        qsort( (void *)zx, 2, sizeof(line_zone_t), compare_index);
        index = 2;
    }
    else if( index <= 1)
    {
        //Problem of positionnement
    }

    // Zone Highest
    // make column sum for each columns*/
    colSum->clear();
    for( int j = 0; j < COL_NBR; j++)
    {
        int sum = 0;
        for(int i = zx[1].start_line; i < zx[1].end_line; i++)
        {
            sum += matrix_filter->at(i).at(j);
        }
        colSum->append(sum);
    }

    // mean of columns sum
    moy = 0;
    for( int i = 0; i < COL_NBR; i++)
    {
        moy += colSum->at(i);
    }
    moy /= COL_NBR;

    //find zones
    val = index = 0;
    memset( (void *)zy, 0, sizeof(zy));
    for( int i = 0; i < COL_NBR; i++)
    {
        if( (colSum->at(i) > moy) && (val < moy))
        {
            zy[index].index = index;
            zy[index].start_col = i;
        }
        if( ((colSum->at(i) < moy)||(i == COL_NBR-1)) && (val > moy))
        {
            zy[index].end_col = i;
            zy[index].n_col = zy[index].end_col - zy[index].start_col;
            //qDebug() << "zone Y " << index << "start from " << zy[index].start_col << "to line " << zy[index].end_col;
            index ++;
        }
        val = colSum->at(i);
    }

    //Count zone number
    if( index > 1)
    {
        /* sort and take biggest one */
        qsort( (void *)zy, index, sizeof(column_zone_t), compare_n_cols);
        //qsort( (void *)zy, index, sizeof(column_zone_t), compare_index);
        index = 1;
    }
    else if( !index)
    {
        //Problem of positionnement
    }

    qDebug() << "TOPZONE LEFT is lines [" << zx[1].start_line << "," << zx[1].end_line << "] and columns [" << zy[0].start_col << "," << zy[0].end_col << "]" ;

    /* calcul des coordonnées du barycentre */
    uint32_t bi = 0, bj = 0, coef_sum = 0;

    for( uint8_t i = zx[1].start_line; i < zx[1].end_line; i++)
    {
        for( uint8_t j = zy[0].start_col; j < zy[0].end_col; j++)
        {
            bi += matrix_filter->at(i).at(j) * i;
            bj += matrix_filter->at(i).at(j) * j;
            coef_sum += matrix_filter->at(i).at(j);
        }
    }

    bi /= coef_sum;
    bj /= coef_sum;

    qDebug() << "barycentre" << bj << bi;

    //Find center(neutral) line
    point_t A, B;
    gvtGet(&m_data_filter_left, &A, &B);
    int medianLine = (A.line + B.line) / 2;
    QVector <int> sumCol;
    int sum = 0;

    binarizeFromNoiseMargin(&m_data_left, &m_data_bin_left);

    //Make sum of each lines
    for(int i = 0; i<LGN_NBR; i++)
    {
        for(int j = 0 ; j < COL_NBR; j++){
            sum+=m_data_bin_left.at(i).at(j);
        }
        sumCol.append(sum);
        sum = 0;
        //qDebug() << m_data_bin_left.at(i);
    }

    //Find center of largest line below median line, priority to lower line:
    //Find index of larger line
    int max = 0;
    int lineMaxLow = 0;
    for(int i = medianLine; i>=0; i--)
    {
        if(sumCol.at(i) >= max)
        {
            lineMaxLow = i;
            max = sumCol.at(i);
        }
    }

    qDebug() << "sumCol" << sumCol;

    //Find left beggining of largest line
    int startCol = 0;
    for(int j = 0; j<COL_NBR; j++)
    {
        if(m_data_bin_left.at(lineMaxLow).at(j) == 1){
            startCol = j;
         break;
        }
    }

    //Find right end of largest line
    int stopCol = 0;
    for(int j = COL_NBR-1; j>=0; j--)
    {
        if(m_data_bin_left.at(lineMaxLow).at(j) == 1){
            stopCol = j;
         break;
        }
    }

    double centerColLow = ceil( ((double)stopCol-(double)startCol)/2 ) + startCol;
    qDebug() << "lineMaxLow" << lineMaxLow;
    qDebug() << "startCol" << startCol << "stopCol" << stopCol;
    qDebug() << "center of biggest lower line" << centerColLow << lineMaxLow;

    point_t pB;
    pB.col = (int)centerColLow;
    pB.line = lineMaxLow;

    //Find center largest line above median line, priority to lower line:
    //Find index of larger line
    max = 0;
    int lineMaxHi = 0;
    for(int i = medianLine; i<LGN_NBR; i++)
    {
        if(sumCol.at(i) >= max)
        {
            lineMaxHi = i;
            max = sumCol.at(i);
        }
    }

    //Find left beggining of largest line
    startCol = 0;
    for(int j = 0; j<COL_NBR; j++)
    {
        if(m_data_bin_left.at(lineMaxHi).at(j) == 1){
            startCol = j;
         break;
        }
    }

    //Find right end of largest line
    stopCol = 0;
    for(int j = COL_NBR-1; j>=0; j--)
    {
        if(m_data_bin_left.at(lineMaxHi).at(j) == 1){
            stopCol = j;
         break;
        }
    }

    double centerColHi = ceil( ((double)stopCol-(double)startCol)/2 ) + startCol;
    qDebug() << "lineMaxHi" << lineMaxHi;
    qDebug() << "startCol" << startCol << "stopCol" << stopCol;
    qDebug() << "center of biggest upper line" << centerColHi << lineMaxHi ;

    point_t pA;
    pA.col = (int)centerColHi;
    pA.line = lineMaxHi;

    QLine line(pA.col, pA.line, pB.col, pB.line);
    m_linesGravity.append(line);

    //Find extremums
    /*bool found = false;
    point_t A, B;

    //Find first toe point
    for( int i = LGN_NBR - 1; (i >= 0) && (found == false); i--)
            for( int j = COL_NBR - 1; j >= 0; j--)
                if( m_data_bin_left.at(i).at(j))
                {
                    A.line = i;
                    A.col  = j;
                    found   = true;
                    break;
                }

    found = false;

    //Find first heel point
    for( int i = 0; (i < LGN_NBR) && (found == false); i++)
        for( int8_t j = COL_NBR - 1; j >= 0; j--)
            if( m_data_bin_left.at(i).at(j))
            {
                B.line = i;
                B.col  = j;
                found   = true;
                break;
            }

    qDebug() << "toe point" << A.col << A.line;
    qDebug() << "heel point" << B.col <<  B.line;*/

    double a = ( (double)pA.line - (double)pB.line ) / ( (double)pA.col - (double)pB.col )  ;
    double b = (double)pA.col - (a * (double)A.line);
    qDebug() << "Y = " << a <<"x +"<<b;

    double tcol = ((double)bi - b) / a;
    tcol = ceil(tcol);
    //tcol -= 2.;
    //double tcol = (double)bi * a + b;
    qDebug() << "tcol" << tcol;

    double dev = bj - tcol;
    qDebug() << "dev left" << dev;

    return dev;

}

int MainWindow::calc_pronation_right(QVector <QVector <double> > *matrix_filter){

    line_zone_t   zx[10];
    column_zone_t zy[10];
    QVector <double> *linSum = new QVector <double> ();
    QVector <double> *colSum = new QVector <double> ();
    int moy = 0;
    int val = 0;
    int index = 0;

    // make sum tab for each lines*/
    for( int i = 0; i < LGN_NBR; i++)
    {
        int sum = 0;
        for(int j = 0; j < COL_NBR; j++)
        {
            sum += matrix_filter->at(i).at(j);
        }
        linSum->append(sum);
    }

    // mean of lines sum
    moy = 0;
    for( int i = 0; i < LGN_NBR; i++)
    {
        moy += linSum->at(i);
    }
    moy /= LGN_NBR;
    //qDebug() << moy;

    //find zones
    for( int i = 0; i < LGN_NBR; i++)
    {
        if( (linSum->at(i) > moy) && (val < moy))
        {
            zx[index].index = index;
            zx[index].start_line = i;
        }

        if( ( (linSum->at(i) < moy) || (i==LGN_NBR-1)) && (val > moy))
        {
            zx[index].end_line = i;
            zx[index].n_lines = zx[index].end_line - zx[index].start_line;
            //qDebug() << "zone X " << index << "start from " << zx[index].start_line << "to line " << zx[index].end_line;
            index ++;
        }
        val = linSum->at(i);
    }

    //Count zone number
    if( index > 2)
    {
        /* sort and take the two biggest zones */
        qsort( (void *)zx, index, sizeof(line_zone_t), compare_n_lines);
        qsort( (void *)zx, 2, sizeof(line_zone_t), compare_index);
        index = 2;
    }
    else if( index <= 1)
    {
        //Problem of positionnement
    }

    // Zone Highest
    // make column sum for each columns*/
    colSum->clear();
    for( int j = 0; j < COL_NBR; j++)
    {
        int sum = 0;
        for(int i = zx[1].start_line; i < zx[1].end_line; i++)
        {
            sum += matrix_filter->at(i).at(j);
        }
        colSum->append(sum);
    }

    // mean of columns sum
    moy = 0;
    for( int i = 0; i < COL_NBR; i++)
    {
        moy += colSum->at(i);
    }
    moy /= COL_NBR;

    //find zones
    val = index = 0;
    memset( (void *)zy, 0, sizeof(zy));
    for( int i = 0; i < COL_NBR; i++)
    {
        if( (colSum->at(i) > moy) && (val < moy))
        {
            zy[index].index = index;
            zy[index].start_col = i;
        }
        if( ((colSum->at(i) < moy)||(i == COL_NBR-1)) && (val > moy))
        {
            zy[index].end_col = i;
            zy[index].n_col = zy[index].end_col - zy[index].start_col;
            qDebug() << "zone Y " << index << "start from " << zy[index].start_col << "to line " << zy[index].end_col;
            index ++;
        }
        val = colSum->at(i);
    }

    //Count zone number
    if( index > 1)
    {
        /* sort and take biggest one */
        qsort( (void *)zy, index, sizeof(column_zone_t), compare_n_cols);
        //qsort( (void *)zy, index, sizeof(column_zone_t), compare_index);
        index = 1;
    }
    else if( !index)
    {
        //Problem of positionnement
    }

    qDebug() << "TOPZONE RIGHT is lines [" << zx[1].start_line << "," << zx[1].end_line << "] and columns [" << zy[0].start_col << "," << zy[0].end_col << "]" ;

    /* calcul des coordonnées du barycentre */
    uint32_t bi = 0, bj = 0, coef_sum = 0;

    for( uint8_t i = zx[1].start_line; i < zx[1].end_line; i++)
    {
        for( uint8_t j = zy[0].start_col; j < zy[0].end_col; j++)
        {
            bi += matrix_filter->at(i).at(j) * i;
            bj += matrix_filter->at(i).at(j) * j;
            coef_sum += matrix_filter->at(i).at(j);
        }
    }

    bi /= coef_sum;
    bj /= coef_sum;

    qDebug() << "barycentre" << bj << bi;

    //Find extremums
    bool found = false;
    point_t A, B;

    //Find first toe point
    for( int i = LGN_NBR - 1; (i >= 0) && (found == false); i--)
            for( int j = 0; j < COL_NBR; j++)
                if( m_data_bin_right.at(i).at(j))
                {
                    A.line = i;
                    A.col  = j;
                    found   = true;
                    break;
                }

    found = false;

    for( int i = 0; (i < LGN_NBR) && (found == false); i++)
        for( int8_t j = 0; j < COL_NBR; j++)
            if( m_data_bin_right.at(i).at(j))
            {
                B.line = i;
                B.col  = j;
                found   = true;
                break;
            }

    qDebug() << "toe point" << A.col << A.line;
    qDebug() << "heel point" << B.col << B.line;

    QLine line(A.col, A.line, B.col, B.line);
    m_linesGravity.append(line);

    double a = ((double)B.col - (double)A.col) / ((double)B.line - (double)A.line);
    double b =  (double)A.col - (a * (double)A.line);

    double tcol = (double)bi * a + b;
    //tcol += 2.;

    double dev = tcol - bj;
    qDebug() << "dev right" << dev;

    return dev;

}

void MainWindow::filterMatrix(QVector <QVector <double> > *matrix, QVector <QVector <double> > *matrix_bin, QVector <QVector <double> > *matrix_filter)
{
    matrix_filter->clear();

    for( int i = 0; i < LGN_NBR; i++)
    {
        QVector<double> foo;

        for( int j = 0; j < COL_NBR; j++)
        {

            if(matrix_bin->at(i).at(j) == 1)
               foo.append(matrix->at(i).at(j));
            else foo.append(0);
        }

        matrix_filter->append(foo);
    }

}

void MainWindow::gvtGet(QVector <QVector <double> > *matrix_filter, point_t *A, point_t *B)
{
    line_zone_t   zx[10];
    column_zone_t zy[10];
    QVector <double> *linSum = new QVector <double> ();
    QVector <double> *colSum = new QVector <double> ();
    int moy = 0;
    int val = 0;
    int index = 0;

    // make sum tab for each lines*/
    for( int i = 0; i < LGN_NBR; i++)
    {
        int sum = 0;
        for(int j = 0; j < COL_NBR; j++)
        {
            sum += matrix_filter->at(i).at(j);
        }
        linSum->append(sum);
    }
    //qDebug() << *linSum;

    // mean of lines sum
    moy = 0;
    for( int i = 0; i < LGN_NBR; i++)
    {
        moy += linSum->at(i);
    }
    moy /= LGN_NBR;
    //qDebug() << moy;

    //find zones
    for( int i = 0; i < LGN_NBR; i++)
    {
        if( (linSum->at(i) > moy) && (val < moy))
        {
            zx[index].index = index;
            zx[index].start_line = i;
        }

        if( ( (linSum->at(i) < moy) || (i==LGN_NBR-1)) && (val > moy))
        {
            zx[index].end_line = i;
            zx[index].n_lines = zx[index].end_line - zx[index].start_line;
            //qDebug() << "zone X " << index << "start from " << zx[index].start_line << "to line " << zx[index].end_line;
            index ++;
        }
        val = linSum->at(i);
    }

    //Count zone number
    if( index > 2)
    {
        /* sort and take the two biggest zones */
        qsort( (void *)zx, index, sizeof(line_zone_t), compare_n_lines);
        qsort( (void *)zx, 2, sizeof(line_zone_t), compare_index);
        index = 2;
    }
    else if( index <= 1)
    {
        //Problem of positionnement
    }

    //qDebug()<< "zone X heel = " << zx[0].start_line << "to " << zx[0].end_line;
    //qDebug()<< "zone X toe = "  << zx[1].start_line << "to " << zx[1].end_line;

    //Zone heel
    // make column sum for each columns*/
    colSum->clear();
    for( int j = 0; j < COL_NBR; j++)
    {
        int sum = 0;
        for(int i = zx[0].start_line; i < zx[0].end_line; i++)
        {
            sum += matrix_filter->at(i).at(j);
        }
        colSum->append(sum);
    }

    // mean of columns sum
    moy = 0;
    for( int i = 0; i < COL_NBR; i++)
    {
        moy += colSum->at(i);
    }
    moy /= COL_NBR;
    //qDebug() << moy;

    //find zones
    val = index = 0;
    memset( (void *)zy, 0, sizeof(zy));
    for( int i = 0; i < COL_NBR; i++)
    {
        if( (colSum->at(i) > moy) && (val < moy))
        {
            zy[index].index = index;
            zy[index].start_col = i;
        }
        if( ((colSum->at(i) < moy)||(i == COL_NBR-1)) && (val > moy))
        {
            zy[index].end_col = i;
            zy[index].n_col = zy[index].end_col - zy[index].start_col;
            //qDebug() << "zone Y " << index << "start from " << zy[index].start_col << "to line " << zy[index].end_col;
            index ++;
        }
        val = colSum->at(i);
    }

    //Count zone number
    if( index > 1)
    {
        /* sort and take biggest one */
        qsort( (void *)zy, index, sizeof(column_zone_t), compare_n_cols);
        qsort( (void *)zy, index, sizeof(column_zone_t), compare_index);
        index = 1;
    }
    else if( !index)
    {
        //Problem of positionnement
    }

    A->line = (zx[0].start_line + zx[0].end_line) / 2;
    A->col  = (zy[0].start_col + zy[0].end_col) / 2;

    //qDebug() << "A line " << A->line << "A col " << A->col;

    //Zone toe
    // make column sum for each columns
    colSum->clear();
    for( int j = 0; j < COL_NBR; j++)
    {
        int sum = 0;
        for(int i = zx[1].start_line; i < zx[1].end_line; i++)
        {
            sum += matrix_filter->at(i).at(j);
        }
        colSum->append(sum);
    }

    // mean of columns sum
    moy = 0;
    for( int i = 0; i < COL_NBR; i++)
    {
        moy += colSum->at(i);
    }
    moy /= COL_NBR;
    //qDebug() << moy;

    //find zones
    val = index = 0;
    memset( (void *)zy, 0, sizeof(zy));
    for( int i = 0; i < COL_NBR; i++)
    {
        if( (colSum->at(i) > moy) && (val < moy))
        {
            zy[index].index = index;
            zy[index].start_col = i;
        }
        if( ((colSum->at(i) < moy)||(i == COL_NBR-1)) && (val > moy))
        {
            zy[index].end_col = i;
            zy[index].n_col = zy[index].end_col - zy[index].start_col;
            //qDebug() << "zone Y " << index << "start from " << zy[index].start_col << "to line " << zy[index].end_col;
            index ++;
        }
        val = colSum->at(i);
    }

    //Count zone number
    if( index > 1)
    {
        /* sort and take biggest one */
        qsort( (void *)zy, index, sizeof(column_zone_t), compare_n_cols);
        qsort( (void *)zy, index, sizeof(column_zone_t), compare_index);
        index = 1;
    }
    else if( !index)
    {
        //Problem of positionnement
    }

    B->line = (zx[1].start_line + zx[1].end_line) / 2;
    B->col  = (zy[0].start_col + zy[0].end_col) / 2;

    //qDebug() << "B line " << B->line << "B col " << B->col;

}

void MainWindow::get_hilo_pos(QVector <QVector <double> > *matrix, int *hi, int *low)
{
    int noiseMargin = m_popupwindow->getNoiseMargin();
    qDebug() << noiseMargin;
    bool found = false;
    /* find low position */
    for( int i = 0; i < LGN_NBR; i++)
    {
        for( int j = 0; j < COL_NBR; j++)
        {
            if (found == false)
            {
                if (matrix->at(i).at(j) > noiseMargin)
                {
                    found = true;
                    *low = i;
                    break;
                }
            }
            else
                break;
        }
    }

    found = false;
    /* find hi position */
    for( int i = LGN_NBR-1; i > 0; i--)
    {
        for( int j = 0; j < COL_NBR; j++)
        {
            if (found == false)
            {
                if (matrix->at(i).at(j) > noiseMargin)
                {
                    found = true;
                    *hi = i;
                    break;
                }
            }
            else
                break;
        }
    }

    qDebug() << "hi = " << *hi << "low = " << *low;
}

//! [8]
void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), m_serial->errorString());
        closeSerialPort();
    }
}
//! [8]

void MainWindow::initActionsConnections()
{
    connect(m_ui->actionConnect, &QAction::triggered, this, &MainWindow::openSerialPort);
    connect(m_ui->actionDisconnect, &QAction::triggered, this, &MainWindow::closeSerialPort);
    connect(m_ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(m_ui->actionConfigure, &QAction::triggered, m_settings, &SettingsDialog::show);
    connect(m_ui->actionClear, &QAction::triggered, m_console, &Console::clear);
    connect(m_ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(m_ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
}

void MainWindow::showStatusMessage(const QString &message)
{
    m_status->setText(message);
}

int MainWindow::compare_n_lines(const void *a, const void *b)
{
    struct line_zone_t{
        int 	index;
        int		start_line;
        int 	end_line;
        int		n_lines;
    };

    uint8_t	n_line_a = ((line_zone_t *)a)->n_lines;
    uint8_t	n_line_b = ((line_zone_t *)b)->n_lines;

    return (n_line_a < n_line_b) - (n_line_a > n_line_b);

}

int MainWindow::compare_index( const void *a, const void *b)
{
    struct line_zone_t{
        int 	index;
        int		start_line;
        int 	end_line;
        int		n_lines;
    };

    uint8_t index_a = ((line_zone_t *)a)->index;
    uint8_t index_b = ((line_zone_t *)b)->index;
    return (index_a > index_b) - (index_a < index_b);
}

int MainWindow::compare_n_cols( const void *a, const void *b)
{
    struct column_zone_t{
        int 	index;
        int		start_col;
        int 	end_col;
        int		n_col;
    };

    uint8_t n_col_a = ((column_zone_t *)a)->n_col;
    uint8_t n_col_b = ((column_zone_t *)b)->n_col;
    return (n_col_a < n_col_b) - (n_col_a > n_col_b);
}

long MainWindow::sumMatrix(QVector <QVector <double> > *matrix, int startLine, int endLine){

    long sum = 0;

    for(int i = startLine; i < endLine; i++)
    {
        for( int j = 0; j < COL_NBR; j++)
        {
               sum += matrix->at(i).at(j);
        }

    }

    return sum;
}
