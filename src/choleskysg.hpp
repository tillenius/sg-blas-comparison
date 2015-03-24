#include "sg/superglue.hpp"
#include "sg/option/instr_trace.hpp"

struct Options : public DefaultOptions<Options> {
#ifdef TRACE
    typedef Trace<Options> Instrumentation;
#endif
};

struct MatrixBlock {
    Handle<Options> handle;
    double *data;
    MatrixBlock() : data(new double[DIM*DIM]) {}
    ~MatrixBlock() { delete[] data; }
};

struct gemm : public Task<Options> {
    double *a, *b, *c;
    gemm(MatrixBlock &ba, MatrixBlock &bb, MatrixBlock &bc)
        : a(ba.data), b(bb.data), c(bc.data)
    {
        register_access(ReadWriteAdd::read, ba.handle);
        register_access(ReadWriteAdd::read, bb.handle);
        register_access(ReadWriteAdd::add, bc.handle);
    }
    void run() {
        dgemm('N', 'T', DIM, DIM, DIM, -1.0, a, DIM, b, DIM, 1.0, c, DIM);
    }
};

struct potrf : public Task<Options> {
    double *a;
    potrf(MatrixBlock &ba) : a(ba.data) {
        register_access(ReadWriteAdd::write, ba.handle);
    }
    void run() {
        int info = 0;
        dpotrf('L', DIM, a, DIM, &info);
        if (info != 0) {
            fprintf(stderr, "### ERROR %d\n", info);
            exit(0);
        }
    }
};

struct trsm : public Task<Options> {
    double *a, *b;
    trsm(MatrixBlock &ba, MatrixBlock &bb) : a(ba.data), b(bb.data) {
        register_access(ReadWriteAdd::read, ba.handle);
        register_access(ReadWriteAdd::write, bb.handle);
    }
    void run() {
        dtrsm('R', 'L', 'T', 'N', DIM, DIM, 1.0, a, DIM, b, DIM);
    }
};

struct syrk : public Task<Options> {
    double *a, *c;
    syrk(MatrixBlock &ba, MatrixBlock &bc) : a(ba.data), c(bc.data) {
        register_access(ReadWriteAdd::read, ba.handle);
        register_access(ReadWriteAdd::add, bc.handle);
    }
    void run() {
        dsyrk('L', 'N', DIM, DIM, -1.0, a, DIM, 1.0, c, DIM);
    }
};


struct copyToTile : public Task<Options> {
    const double *src;
    double *dst;
    const size_t stride;
    copyToTile(MatrixBlock &dst_, const double *src_, size_t stride_)
    : src(src_), dst(dst_.data), stride(stride_) {
        register_access(ReadWriteAdd::write, dst_.handle);
    }
    void run() {
        for (int i = 0; i < DIM; ++i)
            for (int j = 0; j < DIM; ++j)
                dst[i*DIM + j] = src[i*stride + j];
    }
};

struct copyFromTile : public Task<Options> {
    const double *src;
    double *dst;
    const size_t stride;
    copyFromTile(double *dst_, MatrixBlock &src_, size_t stride_)
    : src(src_.data), dst(dst_), stride(stride_) {
        register_access(ReadWriteAdd::read, src_.handle);
    }
    void run() {
        for (int i = 0; i < DIM; ++i)
            for (int j = 0; j < DIM; ++j)
                dst[i*stride + j] = src[i*DIM + j];
    }
};

double cholesky_sg(size_t dim, double *buf) {
#ifdef TRACE
    // To be able to call Log<Options>::log() before
    // SuperGlue is initialized.
    Log<Options>::register_thread(0);
#endif

    const double time_st = dsecnd();

#ifdef TRACE
    Time::TimeUnit start = Time::getTime();
#endif
    MatrixBlock *A = new MatrixBlock[dim * dim];
#ifdef TRACE
    Time::TimeUnit stop = Time::getTime();
    Log<Options>::log("malloc", start, stop);
#endif

    {
        SuperGlue<Options> sg;

        for (int i = 0; i < dim; ++i)
            for (int j = i; j < dim; ++j)
                sg.submit(new copyToTile(A[j*dim+i], buf+i*dim*DIM*DIM+j*DIM, dim*DIM));

        for (int k = 0; k < dim; ++k) {
            sg.submit(new potrf(A[k*dim + k]));
            for (int m = k + 1; m < dim; ++m)
                sg.submit(new trsm(A[k*dim + k], A[m*dim + k]));
            for (int m = k + 1; m < dim; ++m) {
                for (int n = k + 1; n < m; ++n)
                    sg.submit(new gemm(A[m*dim + k], A[n*dim + k], A[m*dim + n]));
                sg.submit(new syrk(A[m*dim + k], A[m*dim + m]));
            }
        }

        for (int i = 0; i < dim; ++i)
            for (int j = i; j < dim; ++j)
                sg.submit(new copyFromTile(buf + i*dim*DIM*DIM + j*DIM, A[j*dim+i], dim*DIM));
    }

#ifdef TRACE
    start = Time::getTime();
#endif
    delete[] A;
#ifdef TRACE
    stop = Time::getTime();
    Log<Options>::log("free", start, stop);
#endif

    const double time_end = dsecnd();

    return time_end - time_st;
}
