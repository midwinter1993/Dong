#include "tcpthread.h"
#include "def.h"
#include "func.h"
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

extern QVector<double> dataToPlot;

ofstream fout("./fftData.out");
ofstream foutOri("./fftData.txt");
QTime q_timer;
int packet_sum;
qint64 packet_byte_num;
//buffer for receive the data through socket
// char socket_rec_buf[SOCKET_REC_BUF_SIZE * 2];
QByteArray sock_rec_buf;

int dataToPlotCnt = 0;
int dataCount;
int packetCnt;
int fftCnt;

double packageBuf[MATRIX_SIZE][PACKET_LEN];
double data_cov[MATRIX_SIZE][MATRIX_SIZE];
double latent[MATRIX_SIZE];
//double vec[MATRIX_SIZE];

double fuck[CURVE_BUF_SIZE];
QVector<double> dataToPlotBuf;
QVector<double> fftResultBuf(CURVE_BUF_SIZE);
QVector<double> dataToFFT(CURVE_BUF_SIZE);

double data_mean[MATRIX_SIZE];
double data_mean_tmp[MATRIX_SIZE];
double csi[MATRIX_SIZE];
double vec_tmp[30];

QMutex mutex;

TcpThread::TcpThread(int sockDpt, QObject *parent):
	QThread(parent)
{
	socketDescriptor = sockDpt;
	tmpObj = NULL;
#ifdef TEST
	qDebug() << "tcpthread created" << QThread::currentThreadId();
#endif
}

TcpThread::~TcpThread()
{
	// if (!tmpObj)
		// delete tmpObj;
}

void TcpThread::run()
{
	tmpObj = new TmpObject(this->socketDescriptor);
	//-------------这是关键--------------------
	tmpObj->moveToThread(this);             //|
	//-----------------------------------------
	//这里的connect用了BlockingQueuedConnection
	connect(tmpObj, SIGNAL(dataReadSignal_tmpObj()), this, SIGNAL(dataReadSignal())/*, Qt::BlockingQueuedConnection*/);
	connect(tmpObj, SIGNAL(disconnectedSignal_tmpObj()), this, SIGNAL(disconnectedSignal()));
	qDebug() << "tcpthread run" << QThread::currentThreadId();
	exec();
}

void TcpThread::readDataSlot()
{

	// if (client->bytesAvailable() < MSG_SIZE)
		// return;
	// Q_ASSERT(client->read(buf, MSG_SIZE) == MSG_SIZE);
	// client->read(socket_rec_buf, MSG_SIZE);
	// qDebug() << "temp " << temp << '\n';
	// emit this->dataReadSignal();
}

TmpObject::TmpObject(int sockDpt)
{
	socketDescriptor_tmpObj = sockDpt;
	client = new QTcpSocket(this);
	if (!client->setSocketDescriptor(socketDescriptor_tmpObj))
	{
		Q_ASSERT(0);
	}
	client->setReadBufferSize(SOCKET_BUF_SIZE);
	
	connect(client, SIGNAL(readyRead()), this, SLOT(readDataSlot_tmpObj()));
	connect(client, SIGNAL(disconnected()), this, SLOT(disconnectedSlot_tmpObj()));
	qDebug() << "TmpObject created" << QThread::currentThreadId();
}

void TmpObject::readDataSlot_tmpObj()
{
	// qDebug() << "receive data thread ID: " << QThread::currentThreadId();
	//qDebug() << "bytes available: " << client->bytesAvailable() << '\n';
	
	packet_byte_num = client->bytesAvailable();
#ifdef TEST
	qDebug() << "client bytesAvailable-------------->" << packet_byte_num << '\n';
#endif
	if (packet_byte_num < MSG_SIZE || packet_byte_num % MSG_SIZE != 0)
		Q_ASSERT(0);

	sock_rec_buf = client->readAll();
	qint64 num = sock_rec_buf.count();
	// qint64 num = client->read(socket_rec_buf, packet_byte_num); 
	if (num != packet_byte_num)
	{
		qDebug() << "read error----------!   " << num << packet_byte_num;
		assert(0);
	}

#ifdef TEST
	qDebug() << "received bytes -----> " << num;
#endif
	// *(quint64 *)(socket_rec_buf + SOCKET_REC_BUF_SIZE - 9) = tmp;

	dataProcess();
#ifdef TEST
	qDebug() << "dataToPlotBuf.size() :----> " << dataToPlotBuf.size();
#endif
	if (dataToPlotCnt > 0)
	{
		mutex.lock();
		emit this->dataReadSignal_tmpObj();
	}
}

void TmpObject::disconnectedSlot_tmpObj()
{
	// delete client;
#ifdef TEST
	qDebug() << "tmpObj disconnect\n";
#endif
	emit disconnectedSignal_tmpObj();
	int time_tmp = q_timer.restart();
	qDebug() << "Time: " << time_tmp << "packet_sum: " << packet_sum;
	// foutOri.close();
	// fout.close();
}

void TmpObject::dataProcess()
{
	// char *buf_tmp = socket_rec_buf;
	char *buf_tmp = sock_rec_buf.data();
	
	//received package numbers，每个包长度为81byte，在接受socket时，已将接受到的byte数存入socket_rec_buf + SOCKET_REC_BUF_SIZE - 9位置处
	// quint64 packet_num = *(quint64 *)(socket_rec_buf + SOCKET_REC_BUF_SIZE -9) / 81;
	qint64 packet_num = packet_byte_num / 81;
	packet_sum += packet_num;
#ifdef TEST
	qDebug() << "packet_num-------------->" << packet_num << '\n';
#endif

	while (packet_num > 0)
	{
		dataCount += 1;
		//收集满PACKET_LEN长度的数据，再做PCA处理，还要记录下数据的均值
		for (int i = 0; i < MATRIX_SIZE; i++) 
		{
			double tmp = sqrt(((double)buf_tmp[21 + i]) * ((double)buf_tmp[21 + i]) \
						 + ((double)buf_tmp[51 + i]) * ((double)buf_tmp[51 + i]));
			packageBuf[i][packetCnt] = tmp;
			data_mean[i] += tmp;
		}
		packetCnt++;
#ifdef TEST
		qDebug() << "packetCnt--------->" << packetCnt;
#endif
		 
		//长度达到PACKET_LEN时，做一次PCA
		if (packetCnt == PACKET_LEN)
		{
#ifdef TEST
			qDebug() << "------------------------------Do PCA-------------------------\n";
#endif
			//取平均值
			for (int i = 0; i < MATRIX_SIZE; i++)
			{
				data_mean[i] /= PACKET_LEN;
			}
			//PCA返回第二维特征向量对应下标
			int secIndex = pca(packageBuf, data_cov, data_mean, PACKET_LEN, latent);

#ifdef TEST
			qDebug() << "----------------------after PCA---------------------\n";
#endif
			//记录下第二维特征向量
			for (int i = 0; i < MATRIX_SIZE; i++)
			{
				// vec_tmp[i] = data_cov[i][secIndex];
				// TODO ??????????
				vec_tmp[i] = data_cov[i][secIndex];
				assert(vec_tmp[0] != 0);
			}

			packetCnt = 0;
			/* 重新开始收集数据，但上一次收集得到的平均值可以用来画图,做归一化
			 * 因此保留 */
			for (int i = 0; i < MATRIX_SIZE; i++) 
			{
				data_mean_tmp[i] = data_mean[i];
				data_mean[i] = 0.0;
			}
		}

		if (dataCount >= PACKET_LEN)
		{
			//做完一次PCA后可以收集数据进行绘图
			for (int i = 0; i < MATRIX_SIZE; i++)
			{
				csi[i] = sqrt(((double)buf_tmp[21 + i]) * ((double)buf_tmp[21 + i]) + ((double)buf_tmp[51 + i]) * ((double)buf_tmp[51 + i]));
			}
			
			double tmp = 0.0;
			for (int i = 0; i < MATRIX_SIZE; i++)
			{
				//做归一化，然后和第二维向量相乘，得到的就是PCA后空间的坐标
				tmp += (csi[i] - data_mean_tmp[i]) * vec_tmp[i];
			}
			assert(tmp != 0);
			
			// dataToPlot[dataToPlotCnt++] = tmp;
			/* 绘制图形的数据按照NUM_OF_AVG平均
			 */
			dataToPlotBuf.append(tmp);
			dataToFFT[fftCnt++] = tmp;
			// fft变化PCA后的结果
			if (fftCnt >= FFT_SUM_THRESHOLD)
			{
#ifdef TEST
				qDebug() << "Do FFTFFTFFTFFTFFTFFTFFTFFTFFTFFTFFTFFTFFTFFTFFTFFTFFTFFTFFTFFTFFT";
#endif
				fftCnt = 0;
				FFT(dataToFFT, CURVE_BUF_SIZE, fftResultBuf);
				// qDebug() << "FFFFFFFFFFFTTTTTTTTTTT";
				// for (int k = 0; k < CURVE_BUF_SIZE; k++)
				// {
				// 	foutOri << dataToFFT[k] << std::endl;
				// 	fout << fftResultBuf[k] << std::endl;
				// }
				// fout << "-------------------------------------------------------------------";
				// assert(0);
			}
			mutex.lock();
			dataToPlotCnt = 0;
			while (dataToPlotBuf.size() >= NUM_OF_AVG)
			{
				for (int k = 0; k < NUM_OF_AVG; k++)
					dataToPlot[dataToPlotCnt] += dataToPlotBuf[k];
				dataToPlot[dataToPlotCnt] /= NUM_OF_AVG;
				dataToPlotCnt++;
				dataToPlotBuf.remove(0, NUM_OF_AVG);

				assert(dataToPlot[dataToPlotCnt - 1] != 0);
			}
			mutex.unlock();
			//重复记录，内存出错
			// fuck[dataToPlotCnt - 1] = dataToPlot[dataToPlotCnt - 1];
			//----------------------------------
		}
		buf_tmp += 81;
		packet_num--;
	}
}
