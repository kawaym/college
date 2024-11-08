#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <x86intrin.h>
#include <malloc.h>

double *single_dimension_matrix(int n)
{
    double *matrix = (double *)_mm_malloc(n * sizeof(double), 32);
    return matrix;
}

double *random_single_dimension_matrix(int n)
{
    double *matrix = single_dimension_matrix(n);
    for (int i = 0; i < n; i++)
    {
        matrix[i] = (double)rand() / RAND_MAX;
    }
    return matrix;
}

void dgemm(size_t n, double *A, double *B, double *C)
{
    for (size_t i = 0; i < n; i += 4)
    {
        for (size_t j = 0; j < n; j++)
        {
            __m256d c0 = _mm256_load_pd(C + i + j * n); /* c0 = C[i][j] */
            for (size_t k = 0; k < n; k++)
                c0 = _mm256_add_pd(c0, /* c0 += A[i][k]*B[k][j] */
                                   _mm256_mul_pd(_mm256_load_pd(A + i + k * n),
                                                 _mm256_broadcast_sd(B + k + j * n)));
            _mm256_store_pd(C + i + j * n, c0); /* C[i][j] = c0 */
        }
    }
}

int main(int argc, char **argv)
{
    int n = atoi(argv[1]);
    double *A = random_single_dimension_matrix(n * n);
    double *B = random_single_dimension_matrix(n * n);
    double *C = single_dimension_matrix(n * n);

    clock_t begin = clock();

    dgemm(n, A, B, C);
    _mm_free(A);
    _mm_free(B);
    _mm_free(C);

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    printf("chapter3, %d, %f", n, time_spent);
    FILE *fp;
    fp = fopen("results/dgemm3.csv", "a");
    fprintf(fp, "chapter3, %d, %f /n", n, time_spent);
    fclose(fp);

    return 0;
}
