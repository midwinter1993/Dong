#include"settingWindow.h"

#include <QSlider>
#include <QString>
#include <QComboBox>
#include <QWidget>
#include <QMainWindow>
#include <QLabel>
#include <QGridLayout>
#include <QFile>
#include <QtDebug>
#include <QApplication>
#include <iostream>

using namespace std;

QString settingWindow:: styleItemStatic = "Theme1";
int settingWindow::opacityStatic = 90;

settingWindow::settingWindow(QWidget *parent) ://构造函数
    QMainWindow(parent)
{

    QLabel *transparencyLabel = new QLabel("透明度");
    transparencySlider = new QSlider(Qt::Horizontal);      // 设置透明度的滚动条
    transparencySlider->setRange(70,100);
    transparencySlider->setValue(opacityStatic);
    connect(transparencySlider,SIGNAL(valueChanged(int)),this,SLOT(transOpacitySlot(int)));
    //把透明度标签跟slider放到水平布局中

	QLabel *themeLabel = new QLabel("主题");

    styleBox = new QComboBox();
    styleBox->addItem("Theme1");
    styleBox->addItem("Theme2");
    styleBox->addItem("Theme3");
    styleBox->addItem("Theme4");
	styleBox->addItem("Theme5");
    styleBox->addItem("Default");
    connect(styleBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(styleChangeSlot(QString)));

    //把前面三个总的布局放置于一个栅格布局中
    QGridLayout *mainLayout=new QGridLayout();
    mainLayout->addWidget(transparencyLabel, 0, 0);
    mainLayout->addWidget(transparencySlider, 0, 1);
    mainLayout->addWidget(themeLabel, 1, 0);
	mainLayout->addWidget(styleBox, 1, 1);

    mainWidget = new QWidget();
    mainWidget->setLayout(mainLayout);
    mainWidget->setMinimumSize(300,200);
    mainWidget->setMaximumSize(300,200);
    mainWidget->move(600,200);
    mainWidget->setWindowTitle("Style Seting");

	if (styleItemStatic == "Theme1")
		styleBox->setCurrentIndex(0);
	else if (styleItemStatic == "Theme2")
		styleBox->setCurrentIndex(1);
	else if (styleItemStatic == "Theme3")
		styleBox->setCurrentIndex(2);
	else if (styleItemStatic == "Theme4")
		styleBox->setCurrentIndex(3);
	else if (styleItemStatic == "Theme5")
		styleBox->setCurrentIndex(4);
	else
		styleBox->setCurrentIndex(5);

    mainWidget->show();
}


void settingWindow::transOpacitySlot(int value)     //透明度
{
	opacityStatic = value;
    qreal opacity = qreal(value)/100.0;
    mainWidget->setWindowOpacity(opacity);
    emit setOpacitySignal(opacity);
}


void settingWindow::styleChangeSlot(QString StyleString)//定义换肤槽
{
	styleItemStatic = StyleString;
    if(StyleString=="Theme1")
    {
        // qDebug() << "Success to change style1";
        QFile file(":/qss/abc.qss");
        file.open(QFile::ReadOnly);
        styleSheet = QLatin1String(file.readAll());
        qApp->setStyleSheet(styleSheet);
        file.close();
    }
    else if(StyleString=="Theme2")
    {
        QFile file(":/qss/style2.qss");
        file.open(QFile::ReadOnly);
        styleSheet = QLatin1String(file.readAll());
        qApp->setStyleSheet(styleSheet);
        file.close();
    }
    else if(StyleString=="Theme3")
    {
        QFile file(":/qss/style3.qss");
        file.open(QFile::ReadOnly);
        styleSheet = QLatin1String(file.readAll());
        qApp->setStyleSheet(styleSheet);
        file.close();
    }
    else if(StyleString=="Theme4")
    {
        QFile file(":/qss/style4.qss");
        file.open(QFile::ReadOnly);
        styleSheet = QLatin1String(file.readAll());
        qApp->setStyleSheet(styleSheet);
        file.close();
    }
    else if(StyleString=="Theme5")
    {
        QFile file(":/qss/style.qss");
        file.open(QFile::ReadOnly);
        styleSheet = QLatin1String(file.readAll());
        qApp->setStyleSheet(styleSheet);
        file.close();
    }
    else if(StyleString=="Default")
    {
        QFile file(":/qss/no.qss");
        file.open(QFile::ReadOnly);
        styleSheet = QLatin1String(file.readAll());
        qApp->setStyleSheet(styleSheet);
        file.close();
    }
    mainWidget->close();     //关闭对话窗口
}

settingWindow::~settingWindow()
{

}
