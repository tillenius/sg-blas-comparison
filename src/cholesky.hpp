#include <acml.h>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <cstdlib>

using namespace std;

const int num_iter = 20;
const size_t DIM = 320;
const size_t dim = 25;

double flops(size_t n) {
    // ref: LAPACK Working Note 41: Installation Guide for LAPACK
    return n*n*n / 3.0 + n*n / 2.0 + n / 6.0;
}

void init_data(size_t dim, double *A) {
    int seed[] = { 0, 0, 0, 1 };
    dlarnv(1, seed, dim*dim*DIM*DIM, A);
    for (int i = 0; i < dim*DIM; ++i)
        A[i*dim*DIM + i] += dim*DIM;
}

double absdiff(size_t dim, const double *A, const double *B) {
    double s = 0.0;
    for (int i = 0; i < dim*DIM; ++i) {
        for (int j = i; j < dim*DIM; ++j) {
            const double d = abs(A[i*dim*DIM + j] - B[i*dim*DIM + j]);
            s += d*d;
        }
    }
    return s;
}

double cholesky_acml(size_t dim, double *B) {
    int info = 0;
    const double time_st = dsecnd();
    dpotrf('L', dim*DIM, B, dim*DIM, &info);
    const double time_end = dsecnd();
    if (info != 0) {
        fprintf(stderr, "### ERROR %d\n", info);
        exit(0);
    }
    return time_end - time_st;
}
