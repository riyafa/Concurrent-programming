#include <iostream>
#include <stdlib.h>
#include <omp.h>

using namespace std;

int main() {
    int n, i, j, k;
    cout << "Enter n: ";
    cin >> n;
    double **a = new double *[n];
    a = new double *[n];
    double **b = new double *[n];
    b = new double *[n];
    double **mult = new double *[n];
    mult = new double *[n];

    srand(time(NULL));
    for (i = 0; i < n; ++i) {
        a[i] = new double[n];
        b[i] = new double[n];
        mult[i] = new double[n];
        for (j = 0; j < n; ++j) {
            a[i][j] = rand();
            b[i][j] = rand();
            mult[i][j] = 0;
        }
    }
    double temp;
    double start = omp_get_wtime();
    // Multiplying matrix a and b and storing in array mult.
    double aik;
#pragma omp parallel for
    for (i = 0; i < n; ++i) {
#pragma omp parallel for
        for (k = 0; k < n; ++k) {
            aik = a[i][k];
            for (j = 0; j < n; ++j) {
                mult[i][j] += aik * b[k][j];
            }
        }
    }
    double finish = omp_get_wtime();
    double tDifference = finish - start;
    cout << "The time taken " << tDifference << endl;
    for (i = 0; i < n; i++) {
        delete[] a[i];
        delete[] b[i];
        delete[] mult[i];
    }
    delete[] a;
    delete[] b;
    delete[] mult;
    return 0;
}