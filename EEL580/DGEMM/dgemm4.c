#include  <stdio.h>
#include  <stdlib.h>
#include  <time.h>
#include <x86intrin.h>
#define UNROLL (4)


double* single_dimension_matrix(int n) {
    double* matrix = (double*) _mm_malloc(n * sizeof(double), 32);
    return matrix;
}

double* random_single_dimension_matrix(int n) {
    double* matrix = single_dimension_matrix(n);
    for (int i = 0; i < n; i++) {
        matrix[i] = (double) rand() / RAND_MAX;
    }
    return matrix;
}

void dgemm (int n, double* A, double* B, double* C){
    for (int i = 0; i < n; i+=UNROLL*8){
        for (int j = 0; j < n; ++j){
            __m512d c[UNROLL];
            for (int r=0;r<UNROLL;r++){
                c[r] =  _mm512_load_pd(C+i+r*8+j*n); //[ UNROLL];
            }
            
            for( int k = 0; k < n; k++ ){
                __m512d bb = _mm512_broadcastsd_pd(_mm_load_sd(B+j*n+k));
                for (int r=0;r<UNROLL;r++){
                    c[r] = _mm512_fmadd_pd(_mm512_load_pd(A+n*k+r*8+i), bb, c[r]);
                }
            }
            for (int r=0;r<UNROLL;r++){
                _mm512_store_pd(C+i+r*8+j*n, c[r]);
            }
        }
    }
}

int main(int argc, char** argv) {
    int n = atoi(argv[1]);
    double* A = random_single_dimension_matrix(n * n);
    double* B = random_single_dimension_matrix(n * n);
    double* C = single_dimension_matrix(n * n);
    
    clock_t begin = clock();

    dgemm(n, A, B, C);

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    printf("chapter4, %d, %f", n, time_spent); 
    FILE *fp;
    fp = fopen("results/dgemm4.csv", "a");
    fprintf(fp, "chapter4, %d, %f /n", n, time_spent);
    fclose(fp);

    return 0;
}
