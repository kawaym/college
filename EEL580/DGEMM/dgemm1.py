import random
import time
import sys


def generate_random_matrix(n):
    matrix = []
    for i in range(n):
        row = []
        for j in range(n):
            row.append(random.randint(0, 100))
        matrix.append(row)
    return matrix


def generate_empty_matrix(n):
    matrix = []
    for i in range(n):
        row = []
        for j in range(n):
            row.append(0)
        matrix.append(row)
    return matrix


def multiply_matrix_vector(matrixA, matrixB):

    n = len(matrixA)
    result_matrix = generate_empty_matrix(n)

    for i in range(n):
        for j in range(n):
            for k in range(n):
                result_matrix[i][j] += matrixA[i][k] * matrixB[k][j]

    return result_matrix


def main(n):
    matrixA, matrixB = generate_random_matrix(n), generate_random_matrix(n)

    start = time.time()
    result_matrix = multiply_matrix_vector(matrixA, matrixB)
    end = time.time()

    print("chapter1", n, end - start)
    with open("results/dgemm1.csv", "a") as f:
        f.write("chapter1, " + str(n) + ", " + str(end - start))

if __name__ == '__main__':
    try:
        n = int(sys.argv[1])
    except:
        raise TypeError("n must be an integer")

    main(n)