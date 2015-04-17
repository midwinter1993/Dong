#include "func.h"
#include <math.h>

double my_abs(double a, double b)
{
	return sqrt(a*a + b*b);
}

void swap (double &a, double &b)
{
    double t;
    t = a;
    a = b;
    b = t;
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

double** create_array(int m, int n)
{
	double **arr = new double*[m];
	for (int i = 0; i < m; ++i)
	{
		arr[i] = new double[n];
	}
	for (int i = 0; i < m; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			arr[i][j] = 0.0;
		}
	}
	return arr;
}

void delete_array(double **arr, int m)
{
	for (int i = 0; i < m; ++i)
	{
		delete[] arr[i];
	}
	delete[] arr;
}

/* 计算 C(mxl) = A(mxn) * B(nxl) */
void matrix_multiple(double **A, double **B, double **C, 
	                 int m, int n, int l)
{
	for (int i = 0; i < m; ++i)
	{
		for (int j = 0; j < l; ++j)
		{
			for (int k = 0; k < n; ++k)
			{
				C[i][j] += A[i][k] * B[k][j];
			}
		}
	}
}

void matrix_multiple(Matrix &A, Matrix &B, Matrix &C)
{
	matrix_multiple(A.get_array(), B.get_array(), C.get_array(),
		A.get_msize(), B.get_msize(), C.get_nsize());
}
