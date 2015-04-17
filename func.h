#ifndef FUNC_H
#define FUC_H
#include "matrix.h"

static double sqrarg;
#define SQR(a) ((sqrarg=(a)) == 0.0 ? 0.0 : sqrarg*sqrarg)

static double maxarg1,maxarg2;
#define FMAX(a,b) (maxarg1=(a),maxarg2=(b),(maxarg1) > (maxarg2) ?\
(maxarg1) : (maxarg2))

static int iminarg1,iminarg2;
#define IMIN(a,b) (iminarg1=(a),iminarg2=(b),(iminarg1) < (iminarg2) ?\
	(iminarg1) : (iminarg2))
#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))

double pythag(double a, double b);

void swap (double &a, double &b);

double my_abs(double a, double b);

double** create_array(int m, int n);

void delete_array(double **arr, int m);


void matrix_multiple(double **A, double **B, double **C, 
					 int m, int n, int l);

void matrix_multiple(Matrix &A, Matrix &B, Matrix &C);

#endif