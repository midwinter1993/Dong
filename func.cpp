#include <math.h>
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <cassert>
#include "func.h"
using namespace std;

/*--------------------------------------------------
 *|    svdcmp函数引用自<<Numerical Recipe in C>>   |
 * -------------------------------------------------
 */
// #define TEST

bool myCmp(double a, double b)
{
	return a > b;
}

double pythag(double a, double b)
//Computes (a2 + b2)1/2 without destructive underflow or overflow.
{
	double absa,absb;
	absa = fabs(a);
	absb = fabs(b);
	if (absa > absb) return absa*sqrt(1.0+SQR(absb/absa));
	else return (absb == 0.0 ? 0.0 : absb*sqrt(1.0+SQR(absa/absb)));
}

void svdcmp(double a[][MATRIX_SIZE],  double *w, double v[][MATRIX_SIZE], int m, int n)
{
	m = MATRIX_SIZE - 1;
	n = MATRIX_SIZE - 1;
//	double pythag(double a, double b);
	int flag, i, its, j, jj, k, l, nm;
	double anorm, c, f, g, h, s, scale, x, y, z;
	//rv1=vector(1,n);
	double rv1[30];
	g = scale = anorm = 0.0; //Householder reduction to bidiagonal form.
	for (i = 0; i <= n; i++) 
	{
#ifdef TEST
		cout << "--------------in svd HERE 1---------------------\n";
#endif
		l = i + 1;
		rv1[i] = scale*g;
		g = s =scale = 0.0;
		if (i <= m)
	   	{
			for (k = i; k <= m; k++)
			   	scale += fabs(a[k][i]);

			if (scale)
		   	{
				for (k = i; k <= m; k++)
			   	{
					a[k][i] /= scale;
					s += a[k][i]*a[k][i];
				}

				f = a[i][i];
				g = -SIGN(sqrt(s), f);
				h = f * g - s;
				a[i][i] = f - g;

				for (j = l; j <= n; j++) 
				{
					for (s = 0.0, k = i; k<= m; k++)
					   	s += a[k][i]*a[k][j];

					f = s / h;
					for (k = i; k<= m; k++)
					   	a[k][j] += f*a[k][i];
				}
				for (k = i; k <= m; k++) 
					a[k][i] *= scale;
			}
		}
#ifdef TEST
		cout << "---------------------in svd HERE 2---------------------\n";
#endif
		w[i] = scale * g;
		g = s = scale = 0.0;
		if (i <= m && i != n)
	   	{
			for (k = l; k <= n; k++)
			   	scale += fabs(a[i][k]);

			if (scale) 
			{
				for (k=l;k<=n;k++) 
				{
					a[i][k] /= scale;
					s += a[i][k] * a[i][k];
				}
				f=a[i][l];
				g = -SIGN(sqrt(s), f);
				h = f * g - s;
				a[i][l] = f - g;

				for (k = l; k<= n; k++) 
					rv1[k] = a[i][k] / h;

				for (j = l; j <= m; j++)
			   	{
					assert(j < 30);
					//cout << j << ' ';
					for ( s=0.0, k = l; k<= n; k++)
				   	{
						assert(k<30); 
						s += a[j][k]*a[i][k];
					}
					//cout << j << ' ';
					for ( k = l; k <= n; k++)
					   	a[j][k] += s * rv1[k];
				}
				for (k = l; k <= n; k++) 
					a[i][k] *= scale;
			}
		}
		anorm = FMAX(anorm, (fabs(w[i]) + fabs(rv1[i])));
	}
#ifdef Test
	cout << "-------------------in svd HERE3-----------------------" << endl;
#endif
	for (i = n; i >= 0; i--)
   	{ //Accumulation of right-hand transformations.
		if (i < n) 
		{
			if (g)
		   	{
				for (j=l;j<=n;j++)
					v[j][i] = (a[i][j] / a[i][l]) / g;

				for (j=l;j<=n;j++) 
				{
					for (s = 0.0, k = l; k <= n; k++)
					   	s += a[i][k] * v[k][j];
					for (k = l; k<= n; k++)
					   	v[k][j] += s * v[k][i];
				}
			}
			for (j = l; j <= n; j++)
			   	v[i][j] = v[j][i] = 0.0;
		}
		v[i][i] = 1.0;
		g = rv1[i];
		l = i;
	}
	for (i = IMIN(m, n); i >= 0; i--)
   	{ //Accumulation of left-hand transformations.
		l = i + 1;
		g = w[i];
		for (j = l; j <= n; j++) 
			a[i][j] = 0.0;

		if (g) 
		{
			g = 1.0 / g;
			for (j = l; j <= n; j++)
		   	{
				for (s = 0.0, k = l; k <= m; k++)
				   	s += a[k][i] * a[k][j];

				f = (s / a[i][i]) * g;
				for (k = i; k <= m; k++)
				   	a[k][j] += f * a[k][i];
			}
			for (j = i; j <= m; j++)
			   	a[j][i] *= g;
		} 
		else 
		{
			for (j = i; j <= m; j++) 
				a[j][i] = 0.0;
		}
		++a[i][i];
	}
	for (k = n; k >= 0; k--) 
	{ //Diagonalization of the bidiagonal form: Loop over
		for (its = 1; its <= 30; its++)
	   	{ 
			flag = 1;
			for (l = k; l>= 0; l--) 
			{ //Test for splitting.
				nm = l - 1; //Note that rv1[1] is always zero.
				if ((double)(fabs(rv1[l]) + anorm) == anorm)
			   	{
					flag=0;
					break;
				}
				if ((double)(fabs(w[nm]) + anorm) == anorm)
				   	break;
			}
			if (flag) 
			{
				c = 0.0; //Cancellation of rv1[l], if l > 1.
				s = 1.0;
				for (i = l; i <= k; i++) 
				{
					f = s * rv1[i];
					rv1[i] = c * rv1[i];
					if ((double)(fabs(f) + anorm) == anorm)
					   	break;

					g = w[i];
					h = pythag(f, g);
					w[i] = h;
					h = 1.0 / h;
					c = g * h;
					s = -f * h;

					for (j = 1;j <= m; j++)
				   	{
						y = a[j][nm];
						z = a[j][i];
						a[j][nm] = y * c + z * s;
						a[j][i] = z * c - y * s;
					}
				}
			}
			z = w[k];
			if (l == k)
		   	{ //Convergence.
				if (z < 0.0) 
				{ //Singular value is made nonnegative.
					w[k] = -z;
					for (j = 0; j <= n; j++)
					   	v[j][k] = -v[j][k];
				}
				break;
			}
			if (its == 30)
			   	printf("no convergence in 30 svdcmp iterations");

			x = w[l]; //Shift from bottom 2-by-2 minor.
			nm = k-1;
			y = w[nm];
			g = rv1[nm];
			h = rv1[k];
			f = ((y - z) * (y + z) + (g - h) * (g + h)) / (2.0 * h * y);
			g = pythag(f, 1.0);
			f = ((x - z) * (x + z) + h * ((y / (f + SIGN(g, f))) - h)) / x;
			c = s = 1.0; //Next QR transformation:

			for (j = l; j <= nm; j++)
		   	{
				i = j + 1;
				g = rv1[i];
				y = w[i];
				h = s * g;
				g = c * g;
				z = pythag(f, h);
				rv1[j] = z;
				c = f / z;
				s = h / z;
				f = x * c + g * s;
				g = g * c - x * s;
				h = y * s;
				y *= c;
				for (jj = 0; jj <= n; jj++)
			   	{
					x = v[jj][j];
					z = v[jj][i];
					v[jj][j] = x * c + z * s;
					v[jj][i] = z * c - x * s;
				}

				z = pythag(f, h);
				w[j] = z; //Rotation can be arbitrary if z = 0.
				if (z) 
				{
					z = 1.0 / z;
					c = f * z;
					s = h * z;
				}
				f = c * g + s * y;
				x = c * y - s * g;

				for (jj = 0; jj <= m; jj++)
			   	{
					y = a[jj][j];
					z = a[jj][i];
					a[jj][j] = y * c + z * s;
					a[jj][i] = z * c - y * s;
				}
			}
			rv1[l] = 0.0;
			rv1[k] = f;
			w[k] = x;
		}
	}
	//free_vector(rv1,1,n);
	//delete []rv1;
}

void cov(double data[][PACKET_LEN], double *mean, double data_cov[][MATRIX_SIZE], int len)
{
	/*
	---------------------------------------------------
	|        FUCKING THE INITIALIZATION!!!!!!!!        |
	---------------------------------------------------
	*/
	for (int i = 0; i < MATRIX_SIZE; i++)
	{
		for (int j = 0; j < MATRIX_SIZE; j++)
		{
			data_cov[i][j] = 0.0;
		}
	}
#ifdef TEST
	cout << "--------------after cov INITIALIZATION-----" << len << "---------------" << endl;
#endif
	//ofstream fout("debug.txt");
	for (int i = 0; i < MATRIX_SIZE; i++)
	{
		for (int j = i; j < MATRIX_SIZE; j++)
		{
			for (int k = 0; k < len; k++)
			{
				//fout << i << '\t' << j << '\t' << k  << '\t' << data[i][k] - mean[i] << '\t' << data[j][k] - mean[j] << '\t' << data_cov[i][j] << endl;
				data_cov[i][j] += (data[i][k] - mean[i]) * (data[j][k] - mean[j]);
				/*cout << data_cov[i][j] << ' ';
				if(data_cov[i][j]  > double(PACKET_LEN * 1000))
				{
					fout.close();
					assert(0);
				}
				*/
				assert(data[i][j] < 1e4 && data_cov[i][j] != 0);
			}
			data_cov[i][j] = data_cov[i][j] / (len - 1);
			data_cov[j][i] = data_cov[i][j];
		}
	}	
}

int pca(double data[][PACKET_LEN], double data_cov[][MATRIX_SIZE], double mean[], int data_len, double *latent)
{	
	double eigvector[30][30];
#ifdef TEST
	cout << "------------------------before cov-------------------------" << endl;
#endif
	cov(data, mean, data_cov, data_len);
#ifdef TEST
	cout << "------------------------after cov before svd--------------------------" << endl;
	
#endif
	svdcmp(data_cov,  latent, eigvector, MATRIX_SIZE, MATRIX_SIZE);
	// cout << findSecondMax(latent);
	//sort(latent, latent + 30, myCmp);
	/*
	if (findSecondMax(latent) != 1)
	{
		cout << "xiabiao:"<<findSecondMax(latent) << endl;
		for (int k = 0; k < 30; k++)
			cout << latent[k] << ' ';
		assert(0);
	}
	*/
	
	for (int i = 0; i < MATRIX_SIZE; i++) 
	{
		for (int j = 0; j < MATRIX_SIZE; j++)
		{
			assert(data_cov[i][j] < 1e4 && data_cov[i][j] != 0);
		}
		// cout << endl;
	}
#ifdef TEST
	cout << "------------------------after svd-------------------------------------" << endl;
#endif
	return findSecondMax(latent);
	/*
	ofstream fout_tmp(".\\cov_tmp.txt");
	for (int i = 0; i < 30; i++)
	{
		for (int j = 0; j < 30; j++)
		{
			fout_tmp << data_cov[i][j] << ' ';
		}
		fout_tmp << endl;
	}
	fout_tmp.close();
	
	svdcmp(data_cov,  latent, eigvector, MATRIX_SIZE, MATRIX_SIZE);
	
	fout_tmp.open(".\\pca_tmp.txt");
	for (int i = 0; i < 30; i++)
	{
		for (int j = 0; j < 30; j++)
		{
			fout_tmp << data_cov[i][j] << ' ';
		}
		fout_tmp << endl;
	}
	fout_tmp.close();
	*/
}

//{{{
/*
int main()
{
	*
	double A[30][30];
	//double U[30][30];
	double W[30];
	double V[30][30];	

	ifstream fin(".\\data_ori.txt");
	ofstream fout(".\\data_svd.txt");
	for (int i = 0; i < 30; i++)
	{
		for (int j = 0; j < 30; j++)
			fin >> A[i][j];
	}

	cout << "------------------------------------here----------------------------" << endl;
	svdcmp(A, W, V, MATRIX_SIZE, MATRIX_SIZE);
	for (int i = 0; i < 30; i++)
	{
		for (int j = 0; j < 30; j++)
			fout << A[i][j] << ' ';
		fout << endl;
	}

	fin.close();
	fout.close();
	//system("pause");
	return 0;
	*
	
	*
	ifstream fin(".\\data_ori.txt");
	ofstream fout(".\\data_cov.txt");
	double data[30][30];
	double mean[30];
	double data_cov[30][30];

	int i, j;
	double tmp = 0.0;
	for (i = 0; i < 30; i++) {
		tmp = 0.0;
		for (j = 0; j < 30; j++) {
			fin >> data[i][j];
			tmp += data[i][j];
		}
		mean[i] = tmp / 30.0;
	}
	cov(data, mean, data_cov, 30);

	for (i = 0; i < 30; i++) {
		for (j = 0; j < 30; j++) {
			fout << data_cov[i][j] << ' ';
		}
		fout << endl;
	}

	fin.close();
	fout.close();
	return 0;
	*
	
	
	ifstream fin(".\\data_ori.txt");
	ofstream fout(".\\data_pca.txt");
	double data[30][PACKET_LEN];
	double mean[30];
	double latent[30];
	//double eigenvector[30][30];
	double data_cov[30][30];
	//double data_cov[30][30];

	
	int i, j;
	double tmp = 0.0;
	for (i = 0; i < 30; i++) 
	{
		tmp = 0.0;
		for (j = 0; j < PACKET_LEN; j++) 
		{
			fin >> data[i][j];
			tmp += data[i][j];
		}
		mean[i] = tmp / (double)PACKET_LEN;
	}

	pca(data, data_cov, mean, PACKET_LEN, latent);

	for (i = 0; i < 30; i++) 
	{
		for (j = 0; j < 30; j++) {
			fout << data_cov[i][j] << ' ';
		}
		fout << endl;
	}
	
	fin.close();
	fout.close();
	return 0;
	
}
*/
//{{{

int findSecondMax(double latent[])
{
	double max = latent[0];
	int index = 0;
	for (int i = 0; i < MATRIX_SIZE; i++)
	{
		if (latent[i] > max)
		{
			index = i;
			max = latent[i];
		}
	}

	int secIndex = 0;
	int secMax = -10000.0;
	for (int i = 0; i < MATRIX_SIZE; i++)
	{
		if (i == index)
			continue;
		if (latent[i] > secMax)
		{
			secIndex = i;
			secMax = latent[i];
		}
	}

	/*if (secIndex != 1)
	{
		for (int k = 0; k < 30; k++)
			cout << latent[k] << ' ';
		cout << "----------------fuck-------------------" << endl;
		// assert(0);
	}*/

	return secIndex;
}
