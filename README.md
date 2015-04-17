创新项目gui
==============================

### 1.项目由Qt4以及qwt5.2.1第三方库完成需安装好

### 2.运行make即可生成可执行文件

### 3.make出错，利用qmake重新生成pro文件，由于有qwt所以要手动添加qwt路径

	INCLUDEPATH += /usr/local/qwt-5.2.1/includ
	LIBS += -L"/usr/local/qwt-5.2.1/lib/" -lqwt

### 4.基本已完成动态画图，读取逗号文件绘图,其他功能还在添加，具体的绘图细节可以再更改
