#include "cholesky.hpp"

using namespace std;

int main() {
    double *src = new double[dim*dim*DIM*DIM];
    init_data(dim, src);

    double *B = new double[dim*dim*DIM*DIM];

    for (int i = 0; i < num_iter; ++i) {
        memcpy(B, src, dim*dim*DIM*DIM*sizeof(double));
        const double t_acml = cholesky_acml(dim, B);
        printf("acml: %8.5g s   %8.5g GFLOPS\n", t_acml, flops(dim*DIM) / t_acml / 1e9);
    }

    delete [] B;
    delete [] src;
    return 0;
}
