#ifndef PCA_H
#define PCA_H

#include "def.h"
#include "matrix.h"

class PCA
{
private:
	/*Given a matrix a[0..m-1][0..n-1], this routine computes its singular value decomposition, A =
	U¡¤W¡¤V T. Thematrix U replaces a on output. The diagonal matrix of singular values W is output
	as a vector w[0..n-1]. The matrix V (not the transpose V T ) is output as v[0..n-1][0..n-1].
	*/
	//void svdcmp(double a[][MATRIX_SIZE],  double *w, double v[][MATRIX_SIZE], int m, int n);
	void svdcmp(double **a,  double *w, double **v, int m, int n);

	//void cov(double data[][PACKAGE_LEN], double *mean, double data_cov[][MATRIX_SIZE], int len);
	double** cov(double **data, int len);

	//void pca(double data[][PACKAGE_LEN], double data_cov[][MATRIX_SIZE], double mean[], int data_len , double *latent);
	void do_pca(double **data, int len);

	void argsort(int index[], double val[]);

public:
	void get_feature(double **data, int len, double *feature);

	void get_feature(Matrix &data, double *feature);
};

class FFT
{
private:
	void bitrp (double xreal [], double ximag [], int n);
public:
	void do_fft(double xreal [], int n);
};

#endif
