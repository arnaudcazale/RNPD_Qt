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

#include "math.h"


//! [0]
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    m_status(new QLabel),
    m_console(new Console),
    m_settings(new SettingsDialog),
    m_popupwindow(new PopupWindow),
    m_display(new DisplayWindow),
    m_display_pointure(new DisplayWindow),
//! [1]
    m_serial(new QSerialPort(this)),
    m_data(new QVector<unsigned int>)
//! [1]
{
//! [0]
    m_count = 0;
    filling = false;

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
    }

    m_ui->setupUi(this);
    m_console->setEnabled(false);
    m_popupwindow->setMinimumSize(767, 1024);
    m_popupwindow->show();
    m_display->show();
    m_display_pointure->show();
    setCentralWidget(m_console);

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
             if(m_data->size() >= 768)
             {
                 filling = false;
                 m_count++;

                 //splitData();
                 splitDataFillZero();
                 fillLeftDataMeanNeightboorhood();
                 fillRightDataMeanNeightboorhood();

                 emit dataReady_left(&m_data_left);
                 emit dataReady_right(&m_data_right);
                 double size = calc_size();
                 m_pointure.append(size);

                 if( (m_count % 2) == 0)
                 {
                     qDebug()<<m_pointure;
                     double pointure_mean = ( m_pointure.at(m_pointure.size()-1) + m_pointure.at(m_pointure.size()-2)) / 2;
                     double pointure_round = round(pointure_mean*2)/2;
                     dataDisplay_pointure.append(QString::number(pointure_round) + "\n");
                     m_display_pointure->putData(dataDisplay_pointure);
                 }
                 m_display->putData(dataDisplay);

                 emit dataReady_line(m_lines);

                 dataDisplay.clear();
                 dataDisplay_pointure.clear();
                 m_lines.clear();
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

void MainWindow::splitData()
{
    /*m_data_left->clear();
    m_data_right->clear();

    for(int i = (m_data->size() / 2); i > 0 ; i-=8)
    {
        for(int j = 8; j > 0; j--)
        {
            m_data_left->append(m_data->at(i-j));
        }
    }
    qDebug()<< "data_left" <<*m_data_left;

    for(int i = m_data->size(); i > (m_data->size() / 2) ; i-=8)
    {
        for(int j = 8; j > 0; j--)
        {
            m_data_right->append(m_data->at(i-j));
        }
    }
    qDebug()<< "data_right" <<*m_data_right;

    m_data->clear();*/
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

void MainWindow::binarize(QVector <QVector <double> > *matrix, QVector <QVector <double> > *matrix_bin)
{
    matrix_bin->clear();

    // calculate means of both matrixes
    unsigned int mean_left = calc_mean(&m_data_left);
    unsigned int mean_right = calc_mean(&m_data_right);
    unsigned int mean = (mean_left + mean_right)/2;

    //qDebug() << mean;

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

    qDebug() << *tab;

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
    binarize(&m_data_left, &m_data_bin_left);
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
    binarize(&m_data_right, &m_data_bin_right);
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


