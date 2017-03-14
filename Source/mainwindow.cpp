#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QDataStream>
#include <QTimer>
#include <QDateTime>
#include <unistd.h>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>

QSerialPort serial;
QTimer *t = new QTimer;
QTime t3;
int qFlag = 0;
double curVal, maxVal1, maxVal2;
int maxTime1=0, maxTime2=0, thresh, thresh2, thresh3;

bool ok;
int counts;
QString comR = "com1";


QByteArray res;
double totalVolume;
QString grandTotal;
bool flag = false;
int counter = 0;
double freq, lastFreq;
double zero = 0.000;

int t1=0, t2=0;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    t3.start();
    ui->setupUi(this);
    init();
    ui->pushButton_2->setEnabled(false);
    ui->statusBar->showMessage("Ready to go! Click to start!", 0);
    ui->spinBox->setEnabled(true);
    //*ui->pushButton_2->setDisabled();
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(startM()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(stopM()));
    connect(ui->actionExit, SIGNAL(triggered(bool)),this,SLOT(close()));
    connect(ui->actionInfo, SIGNAL(triggered(bool)),this,SLOT(info()));
    connect(ui->pushButton_3, SIGNAL(clicked(bool)),this,SLOT(comChanged()));
    connect(ui->pushButton_4, SIGNAL(clicked(bool)),this,SLOT(close()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init(){

    serial.setPortName("com6");
    serial.setBaudRate(QSerialPort::Baud38400);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);
    serial.open(QIODevice::ReadWrite);

}
void MainWindow::askV(){
    serial.write("DAFxx0001\r");
    connect(&serial, SIGNAL(readyRead()),this,SLOT(answer()));
}

void MainWindow::answer(){
    QByteArray temp1 = serial.readAll();
    QByteArray temp2;

          \

    if(!temp1.isEmpty()){
    temp2 += temp1;
    check(temp2);
        if (temp2.contains(".")){
        temp2.remove(((temp2.size())-2),2);
        t2 = t3.elapsed();
        totalVolume += ( temp2.toDouble() * 1000) / ( (1000 / (t2-t1)) * 60 );
        curVal = temp2.toDouble();

        temp2="";

                if (qFlag == 0){
                    if (curVal > 0) { thresh3++; }

                    if (curVal > maxVal1){
                     thresh2++;
                     maxVal1 = curVal;
                     maxTime1 = t3.elapsed();
                     thresh=0;

                  }

                    if ((curVal == 0) && (thresh2 >= 3) && (thresh3 >=20)) {
                        thresh++;

                    }

                    if (thresh >= 12){
                        qFlag = 1;
                       //  qDebug() << "A triggered";
                         thresh=0;
                         thresh2=0;
                         thresh3=0;
                         maxVal1=0;
                         maxVal2=0;
                         counts++;
                         ui->lcdNumber_3->display(counts);

                     }


                }

                if (qFlag == 1){
                    if (curVal > 0) { thresh3++; }

                    if (curVal > maxVal2){
                     thresh2++;
                     maxVal2 = curVal;
                     maxTime2 = t3.elapsed();
                     thresh=0;

                  }

                    if ((curVal == 0) && (thresh2 >= 3) && (thresh3 >=20)){
                        thresh++;

                    }

                    if (thresh >= 12){
                        qFlag = 2;
                        // qDebug() << "B triggered";
                         thresh=0;
                         thresh2=0;
                         thresh3=0;
                         maxVal1=0;
                         maxVal2=0;
                         counts++;
                         ui->lcdNumber_3->display(counts);


                     }


                }
                if (qFlag == 2){
                    if ((maxTime2-maxTime1) && !(lastFreq)){
                        int diff = (maxTime2-maxTime1);
                        if (!(diff == 0)){
                        freq = 60 / ((maxTime2 - maxTime1) / 1000);
                        }
                        maxVal1=0;
                        maxVal2=0;
                        lastFreq = freq;
                        qFlag = 0;
                        ui->lcdNumber_2->display(freq);

                    }
                   if ((maxTime2-maxTime1) && (lastFreq > 0)){
                       int diff = (maxTime2-maxTime1);
                       if (!(diff == 0)){
                      freq = ( (lastFreq * 3) + (60 / ((maxTime2 - maxTime1) / 1000)))/4;
                       }
                      maxVal1=0;
                      maxVal2=0;
                      lastFreq = freq;
                      qFlag = 0;
                      ui->lcdNumber_2->display(freq);


                    }
                }

                t1 = t2;
                t2=0;

            }
        }
    temp1 = "";
    temp2 = "";

    counter++;
    if (counter == 40){
      ui->lcdNumber->display(totalVolume);
         counter = 0;

    }
    }




void MainWindow::startM(){
    if(serial.isOpen()){
    flag = true;
    ui->pushButton->setDisabled(flag);
    ui->pushButton_2->setEnabled(flag);
    counter = 0;
    totalVolume = 0;
    freq = 0;
    counts = 0;
    ui->lcdNumber_2->display(freq);
    ui->lcdNumber_3->display(counts);

    t1 = t3.elapsed();


        connect(t, SIGNAL(timeout()), this, SLOT(test()));
        t->start(25);

ui->statusBar->showMessage("Calculating...", 0);
    }
    else QMessageBox::information(this,"Wrong COM port!","Check the device connection or verify COM port is selected!");


}

void MainWindow::stopM(){
t->stop();
flag = false;
ui->pushButton->setDisabled(flag);
ui->pushButton_2->setEnabled(flag);
ui->statusBar->clearMessage();
ui->statusBar->showMessage("Done! Click to start next test!", 10000);
}

void MainWindow::test(){

  //  qDebug() << flag;
  //  qDebug() << t3.elapsed();
    serial.write("DAFxx0001\r");
    connect(&serial, SIGNAL(readyRead()),this,SLOT(answer()));




}

void MainWindow::check(QByteArray ba){
    if (ba.contains("O")){
        ba.remove((ba.indexOf("O",0)),1);
    }
    else if (ba.contains("K")){
        ba.remove((ba.indexOf("K",0)),1);
    }
    else if (ba.contains("r")){
        ba.remove((ba.indexOf("r",0)),1);
    }
    else if (ba.contains("n")){
        ba.remove((ba.indexOf("n",0)),1);
    }
}

void MainWindow::info(){
 QMessageBox::information(this,"Info","Device should be on COM6 as deafult. This programm is free to use. Author: Misha Ivanov mikhail.ivanov@rocketmail.com");
}

void MainWindow::comChanged(){
 comR = "com" + ui->spinBox->cleanText();
 QMessageBox::information(this,"COM port","New COM port is selected!");
 serial.close();
 serial.setPortName(comR);
 serial.setBaudRate(QSerialPort::Baud38400);
 serial.setDataBits(QSerialPort::Data8);
 serial.setParity(QSerialPort::NoParity);
 serial.setStopBits(QSerialPort::OneStop);
 serial.setFlowControl(QSerialPort::NoFlowControl);
 serial.open(QIODevice::ReadWrite);

}
