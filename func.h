#ifndef FUNC_H
#define FUNC_H

#include "def.h"

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

/*Given a matrix a[0..m-1][0..n-1], this routine computes its singular value decomposition, A =
U¡¤W¡¤V T. Thematrix U replaces a on output. The diagonal matrix of singular values W is output
as a vector w[0..n-1]. Thematrix V (not the transpose V T ) is output as v[0..n-1][0..n-1].
*/
/*
void svdcmp(double a[][MATRIX_SIZE],  double w[], double v[][MATRIX_SIZE], int m, int n);

void cov(double data[][PACKAGE_LEN], double *mean, double data_cov[][MATRIX_SIZE], int len);

void pca(double data[][PACKAGE_LEN], double data_cov[][MATRIX_SIZE], double mean[], int data_len , double latent[]);
*/
void svdcmp(double a[][MATRIX_SIZE],  double *w, double v[][MATRIX_SIZE], int m, int n);

void cov(double data[][PACKAGE_LEN], double *mean, double data_cov[][MATRIX_SIZE], int len);

void pca(double data[][PACKAGE_LEN], double data_cov[][MATRIX_SIZE], double mean[], int data_len , double *latent);



#endif
