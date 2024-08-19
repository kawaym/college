#include  <stdio.h>
#include  <stdlib.h>
#include  <time.h>

double* single_dimension_matrix(int n) {
    double* matrix = (double*) malloc(n * sizeof(double));
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
   for (int i = 0; i < n; ++i){
     for (int j = 0; j < n; ++j){
       double cij = C[i+j*n]; /* cij = C[i][j] */
       
       for(int k = 0; k < n; k++)
         cij += A[i+k*n] * B[k+j*n]; /* cij += A[i][k]*B[k][j] */
       C[i+j*n] = cij; /* C[i][j] = cij */
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

    printf("chapter2, %d, %f", n, time_spent); 
    FILE *fp;
    fp = fopen("results/dgemm2.csv", "a");
    fprintf(fp, "chapter2, %d, %f /n", n, time_spent);
    fclose(fp);


    return 0;
}
