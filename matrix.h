#ifndef MATRIX_H
#define MATRIX_H

class Matrix
{
private:
	double **arr;

	int m;
	int n;
public:
	Matrix(int m, int n);

	~Matrix();

	double **get_array();

	int get_msize();

	int  get_nsize();

	double *operator [](int m);

};

#endif