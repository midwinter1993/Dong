//皮肤设置窗体

#ifndef SETTING_WINDOW
#define SETTING_WINDOW

#include <QMainWindow>

class QMainWindow;
class QSlider;
class QString;
class QComboBox;
class QWidget;


namespace Ui
{
    class settingWindow;
}

class settingWindow:public QMainWindow
{
    Q_OBJECT

public:
    settingWindow(QWidget *parent = NULL) ;
    ~settingWindow();

private slots:
    void styleChangeSlot(QString);         //改变样式
    void transOpacitySlot(int);            //改变透明度

signals:
    void setOpacitySignal(qreal);

private:
    QSlider *transparencySlider;    //透明度条
    QString styleSheet;             //用于设置样式表
    QComboBox *styleBox;             //用于选择样式
    QWidget * mainWidget;
	static QString styleItemStatic;
	static int opacityStatic;
};

#endif 
