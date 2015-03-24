#include "cholesky.hpp"
#include "choleskysg.hpp"

int main() {
    double *src = new double[dim*dim*DIM*DIM];
    init_data(dim, src);

    double *B = new double[dim*dim*DIM*DIM];

    {
        double *C = new double[dim*dim*DIM*DIM];
        memcpy(C, src, dim*dim*DIM*DIM*sizeof(double));
        const double t_acml = cholesky_acml(dim, C);
        printf("acml: %8.5g s   %8.5g GFLOPS\n", t_acml, flops(dim*DIM) / t_acml / 1e9);

        memcpy(B, src, dim*dim*DIM*DIM*sizeof(double));
        const double t_sg = cholesky_sg(dim, B);
        printf("sg..: %8.5g s   %8.5g GFLOPS\n", t_sg, flops(dim*DIM) / t_sg / 1e9);

        const double diff = absdiff(dim, C, B);
        printf("  diff=%8.5g\n", diff);

        delete[] C;
    }

    for (int i = 0; i < num_iter; ++i) {
        memcpy(B, src, dim*dim*DIM*DIM*sizeof(double));
        double t_sg = cholesky_sg(dim, B);
        printf("sg..: %8.5g s   %8.5g GFLOPS\n", t_sg, flops(dim*DIM) / t_sg / 1e9);
    }

    delete[] B;
    delete[] src;
    return 0;
}
