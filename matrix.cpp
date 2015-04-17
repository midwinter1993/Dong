#include "matrix.h"
#include "func.h"

Matrix::Matrix(int m, int n)
{
	this->m = m;
	this->n = n;
	arr = create_array(m, n);
}

Matrix::~Matrix()
{
	delete_array(arr, m);
}

double **Matrix::get_array()
{
	return arr;
}

int Matrix::get_msize()
{
	return m;
}

int Matrix::get_nsize()
{
	return n;
}

double *Matrix::operator [](int m)
{
	return arr[m];
}



