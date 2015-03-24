#include "cholesky.hpp"
#define TRACE
#include "choleskysg.hpp"

int main() {
    double *src = new double[dim*dim*DIM*DIM];
    init_data(dim, src);

    double *B = new double[dim*dim*DIM*DIM];

    for (int i = 0; i < 2; ++i) {
        memcpy(B, src, dim*dim*DIM*DIM*sizeof(double));
        const double t_sg = cholesky_sg(dim, B);
        printf("sg..: %8.5g s   %8.5g GFLOPS\n", t_sg, flops(dim*DIM) / t_sg / 1e9);
    }

    Trace<Options>::dump("execution.trace");

    delete[] B;
    delete[] src;
    return 0;
}
