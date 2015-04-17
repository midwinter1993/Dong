#include "def.h"
#include "pca.h"
#include "func.h"
#include "matrix.h"
#include "tcpthread.h"

#include <fstream>
#include <cmath>
#include <cassert>
#include <QTcpSocket>
#include <QByteArray>
#include <QtGlobal>
#include <QThread>
#include <QTime>
#include <QMutex>

using namespace std;
#define TEST

extern QVector<double> plot_buf_1;
extern QVector<double> plot_buf_2;

//buffer for receive the data through socket

int plot_flag = 0;
int packetCnt;

Matrix data_buf(1024, COL_NUM);
double fft_feature[80];
//double vec[COL_NUM];

QMutex mutex;

TcpThread::TcpThread(int sockDpt, QObject *parent):
	QThread(parent)
{
	socketDescriptor = sockDpt;
	dataPro = NULL;
#ifdef TEST
	qDebug() << "tcpthread created" << QThread::currentThreadId();
#endif
}

TcpThread::~TcpThread()
{

}

void TcpThread::run()
{
	dataPro = new DataProcessor(this->socketDescriptor);
	//-------------这是关键--------------------
	dataPro->moveToThread(this);             //|
	//-----------------------------------------
	//这里的connect用了BlockingQueuedConnection
	connect(dataPro, SIGNAL(dataFinishSignal()), this, SIGNAL(dataReadSignal())/*, Qt::BlockingQueuedConnection*/);
	connect(dataPro, SIGNAL(disconnectedSockSignal()), this, SIGNAL(disconnectedSignal()));
	qDebug() << "tcpthread run" << QThread::currentThreadId();
	exec();
}

DataProcessor::DataProcessor(int sockDpt):
	total_packet_sum(0),
	recv_packet_num(0)
{
	socketDescriptor = sockDpt;
	client = new QTcpSocket(this);
	if (!client->setSocketDescriptor(socketDescriptor))
	{
		Q_ASSERT(0);
	}
	client->setReadBufferSize(SOCKET_BUF_SIZE);
	
	connect(client, SIGNAL(readyRead()), this, SLOT(readSockDataSlot()));
	connect(client, SIGNAL(disconnected()), this, SLOT(disconnectedSockSlot()));
	qDebug() << "DataProcessor created" << QThread::currentThreadId();
}

void DataProcessor::readSockDataSlot()
{
	recv_byte_sum = client->bytesAvailable();
#ifdef TEST
	qDebug() << "client bytesAvailable-------------->" << recv_byte_sum << '\n';
#endif
	if (recv_byte_sum < MSG_SIZE || recv_byte_sum % MSG_SIZE != 0)
		Q_ASSERT(0);

	sock_recv_buf = client->readAll();
	assert(sock_recv_buf.count() == recv_byte_sum);
	// qint64 num = client->read(socket_rec_buf, recv_byte_sum); 

	dataProcess();
	if (plot_flag)
	{
		mutex.lock();
		emit this->dataFinishSignal();
	}
}

void DataProcessor::disconnectedSockSlot()
{
	// delete client;
#ifdef TEST
	qDebug() << "tmpObj disconnect\n";
#endif
	emit disconnectedSockSignal();
}

void DataProcessor::dataProcess()
{
	char *buf_tmp = sock_recv_buf.data();
	
	recv_packet_num = recv_byte_sum / 81;
	total_packet_sum += recv_packet_num;
#ifdef TEST
	qDebug() << "recv_packet_num-------------->" << recv_packet_num << '\n';
#endif

	for (int i = 0; i < recv_packet_num; i++)
	{
		//收集满PACKET_LEN长度的数据，再做PCA处理，还要记录下数据的均值
		for (int j = 0; j < COL_NUM; i++) 
		{
			data_buf[packetCnt][j] = \
				my_abs((double)(buf_tmp[21 + j]), (double)(buf_tmp[51 + j]));
		}
		packetCnt++;
#ifdef TEST
		qDebug() << "packetCnt--------->" << packetCnt;
#endif
		 
		//长度达到PACKET_LEN时，做一次PCA+fft提取特征,并取PCA第二维结果绘图
		if (packetCnt == PACKET_LEN)
		{
			packetCnt = 0;
			pca.get_feature(data_buf, fft_feature);
			for (int k = 0; k < 1024; k++) 
			{
				mutex.lock();
				if (plot_flag == 0 || plot_flag == 2)
				{
					plot_buf_1[k] = data_buf[k][2];
					plot_flag = 1;
				}
				else
				{
					plot_buf_2[k] = data_buf[k][2];
					plot_flag = 2;
				}
				mutex.unlock();
			}
		}

		buf_tmp += 81;
	}
}
