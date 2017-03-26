#include <iostream>
#include <stdlib.h>

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
    clock_t tStart = clock();
    // Multiplying matrix a and b and storing in array mult.
    for (i = 0; i < n; ++i)
        for (j = 0; j < n; ++j)
            for (k = 0; k < n; ++k) {
                mult[i][j] += a[i][k] * b[k][j];
            }
    clock_t tEnd = clock();
    double tDifference = (double) (tEnd - tStart) / CLOCKS_PER_SEC;
    cout << "The time taken " << tDifference;
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