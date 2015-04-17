#include <math.h>
#include <stdio.h>
#include <assert.h>
#include "pca.h"
#include "func.h"
#include "matrix.h"

//#define TEST
/* ¶ÔÊý×é×öPCA£¬FFT£¬×îºóµÃµ½SVMµÄfeature */
void PCA::get_feature(double **data, int len, double *feature)
{
	FFT fft;
	do_pca(data, len);

	double buf[512];
	for (int i = 0; i < 80; i++)
	{
		feature[i] = 0.0;
	}

	for (int i = 0; i < 10; i++)
	{
		/* ¶ÔÇ°512¸öµã×öfft£¬È»ºóÈ¡fft½á¹ûµÄ[1-40]
		 * ×Ü¹ýÀÛ¼Æ10´Î£¬×îºó½«10´Îfft½á¹ûÈ¡¾ùÖµ
		 */
		for (int j = 0; j < 512; j++)
		{
			buf[j] = data[j][i];
		}
		fft.do_fft(buf, 512);
		for (int j = 1; j < 41; j++)
		{
			feature[j-1] += buf[j];
		}
		/* ¶Ôºó512¸öµã×öfft£¬È»ºóÈ¡fft½á¹ûµÄ[1-40]
		 * ÓëÖ®Ç°µÄ40¸öfft½á¹ûÆ´½Ó´æÈë[40-79]
		 */
		for (int j = 0; j < 512; j++)
		{
			buf[j] = data[j+512][i];
		}
		fft.do_fft(buf, 512);
		for (int j = 1; j < 41; j++)
		{
			feature[j-1+40] += buf[j];
		}
	}

	for (int i = 0; i < 80; i++)
	{
		feature[i] /= 10;
	}
}

void PCA::get_feature(Matrix &data, double *feature)
{
	get_feature(data.get_array(), data.get_msize(), feature);
}

double** PCA::cov(double **data, int len)
{
	/* initialization */
	double **data_cov = create_array(COL_NUM, COL_NUM);

	for (int i = 0; i < COL_NUM; i++)
	{
		for (int j = i; j < COL_NUM; j++)
		{
			for (int k = 0; k < len; k++)
			{
				data_cov[i][j] += (data[k][i] * data[k][j]);
			}
			data_cov[i][j] = data_cov[i][j] / (len - 1);
			data_cov[j][i] = data_cov[i][j];
		}
	}	
	return data_cov;
}

void PCA::do_pca(double **data, int len)
{	
	double *mean = new double[COL_NUM];
	/* calculate the mean */
	for (int i = 0; i < COL_NUM; ++i)
	{
		mean[i] = 0.0;
		for (int j = 0; j < len; ++j)
		{
			mean[i] += data[j][i];
		}
		mean[i] /= len;
	}
	/* È¥Æ½¾ù²Ù×÷ */
	for (int i = 0; i < COL_NUM; ++i)
	{
		for (int j = 0; j < len; ++j)
		{
			data[j][i] -= mean[i];
		}
	}

	double **eigvector = create_array(COL_NUM, COL_NUM);
	double latent[30];
	double **data_cov = cov(data, len);
	svdcmp(data_cov, latent, eigvector, COL_NUM, COL_NUM);

	int index[COL_NUM];
	for (int i = 0; i < COL_NUM; ++i)
	{
		index[i] = i;
	}
	double **reduce_data = create_array(len, 10);
	double **reduce_evg = create_array(COL_NUM, 10);

	argsort(index, latent);

	/*
	for(int i = 0; i < 30; i++)
	{
		cout << index[i] << ' ' << latent[index[i]] << endl;
	}
	*/

	for (int i = 0; i < COL_NUM; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			reduce_evg[i][j] = data_cov[i][index[j]];
			//cout << reduce_evg[i][j] << "  ";
		}
		//cout << endl;
	}
	matrix_multiple(data, reduce_evg, reduce_data, len, COL_NUM, 10);

	for (int i = 0; i < len; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			data[i][j] = reduce_data[i][j];
		}
	}

	delete[] mean;
	delete_array(reduce_evg, COL_NUM);
	delete_array(reduce_data, len);
	delete_array(eigvector, COL_NUM);
	delete_array(data_cov, COL_NUM);
}

/* PCA½á¹û¸ù¾ÝÌØÕ÷ÖµÅÅ¶¨ÌØÕ÷ÏòÁ¿µÄÐò */
void PCA::argsort(int index[], double val[])
{
	for (int i = COL_NUM-1; i > 0; --i)
	{
		for (int j = 0; j < i; ++j)
		{
			if (val[index[j]] < val[index[j+1]])
			{
				int tmp = index[j+1];
				index[j+1] = index[j];
				index[j] = tmp;
			}
		}
	}
}


/*
 * ¿ìËÙ¸µÁ¢Ò¶±ä»» Fast Fourier Transform
 * By rappizit@yahoo.com.cn
 * 2007-07-20
 * °æ±¾ 2.0
 * ¸Ä½øÁË¡¶Ëã·¨µ¼ÂÛ¡·µÄËã·¨£¬Ðý×ªÒò×ÓÈ¡ ¦Øn-kj  (¦Ønkj µÄ¹²éî¸´Êý)
 * ÇÒÖ»¼ÆËã n / 2 ´Î£¬¶øÎ´¸Ä½øÇ°ÐèÒª¼ÆËã (n * lg n) / 2 ´Î¡£
*/


void FFT::bitrp (double xreal [], double ximag [], int n)
{
	// Î»·´×ªÖÃ»» Bit-reversal Permutation
	int i, j, a, b, p;

	for (i = 1, p = 0; i < n; i *= 2)
	{
		p ++;
	}
	for (i = 0; i < n; i ++)
	{
		a = i;
		b = 0;
		for (j = 0; j < p; j ++)
		{
			b = (b << 1) + (a & 1);    // b = b * 2 + a % 2;
			a >>= 1;        // a = a / 2;
		}
		if ( b > i)
		{
			swap (xreal [i], xreal [b]);
			swap (ximag [i], ximag [b]);
		}
	}
}

void FFT::do_fft(double xreal [], int n)
{
	const int N = 1024;
	const double PI = 3.1416;
	// ¿ìËÙ¸µÁ¢Ò¶±ä»»£¬½«¸´Êý x ±ä»»ºóÈÔ±£´æÔÚ x ÖÐ£¬xreal, ximag ·Ö±ðÊÇ x µÄÊµ²¿ºÍÐé²¿
	double wreal [N / 2], wimag [N / 2], treal, timag, ureal, uimag, arg;
	double ximag [N / 2];
	int m, k, j, t, index1, index2;

	bitrp (xreal, ximag, n);

	// ¼ÆËã 1 µÄÇ° n / 2 ¸ö n ´Î·½¸ùµÄ¹²éî¸´Êý W'j = wreal [j] + i * wimag [j] , j = 0, 1, ... , n / 2 - 1
	arg = - 2 * PI / n;
	treal = cos (arg);
	timag = sin (arg);
	wreal [0] = 1.0;
	wimag [0] = 0.0;
	for (j = 1; j < n / 2; j ++)
	{
		wreal [j] = wreal [j - 1] * treal - wimag [j - 1] * timag;
		wimag [j] = wreal [j - 1] * timag + wimag [j - 1] * treal;
	}

	for (m = 2; m <= n; m *= 2)
	{
		for (k = 0; k < n; k += m)
		{
			for (j = 0; j < m / 2; j ++)
			{
				index1 = k + j;
				index2 = index1 + m / 2;
				t = n * j / m;    // Ðý×ªÒò×Ó w µÄÊµ²¿ÔÚ wreal [] ÖÐµÄÏÂ±êÎª t
				treal = wreal [t] * xreal [index2] - wimag [t] * ximag [index2];
				timag = wreal [t] * ximag [index2] + wimag [t] * xreal [index2];
				ureal = xreal [index1];
				uimag = ximag [index1];
				xreal [index1] = ureal + treal;
				ximag [index1] = uimag + timag;
				xreal [index2] = ureal - treal;
				ximag [index2] = uimag - timag;
			}
		}
	}
	for (int i = 0; i < n; i++)
	{
		xreal[i] = my_abs(xreal[i], ximag[i]);
	}
}

/* +-------------------------------------------------+
 * |    svdcmpº¯ÊýÒýÓÃ×Ô<<Numerical Recipe in C>>   |
 * +-------------------------------------------------+
 */
void PCA::svdcmp(double **a,  double *w, double **v, int m, int n)
{
	m = COL_NUM - 1;
	n = COL_NUM - 1;
//	double pythag(double a, double b);
	int flag, i, its, j, jj, k, l, nm;
	double anorm, c, f, g, h, s, scale, x, y, z;
	//rv1=vector(1,n);
	double rv1[30];
	g = scale = anorm = 0.0; //Householder reduction to bidiagonal form.
	for (i = 0; i <= n; i++) 
	{
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
