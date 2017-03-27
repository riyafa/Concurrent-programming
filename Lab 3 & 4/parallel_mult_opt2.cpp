#include <iostream>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
using namespace std;

int main() {
    int n, i, j, k, iterations;
    cout << "Enter n: ";
    cin >> n;
    cout << "Enter number of iterations: ";
    cin >> iterations;
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
    double timespent[n];
    for (int l = 0; l < iterations; l++) {
        double start = omp_get_wtime();
    // Multiplying matrix a and b and storing in array mult.
    double sum;
#pragma omp parallel for
    for (i = 0; i < n; ++i) {
#pragma omp parallel for
        for (j = 0; j < n; ++j) {
            sum = 0;
            for (k = 0; k < n; ++k) {
                sum += a[i][k] * b[k][j];
            }
            mult[i][j] =sum;
        }
    }
    double finish = omp_get_wtime();
    double tDifference = finish - start;
        timespent[l] = tDifference;
//        cout << "The time taken " << tDifference << endl;
    }
    for (i = 0; i < n; i++) {
        delete[] a[i];
        delete[] b[i];
        delete[] mult[i];
    }
    delete[] a;
    delete[] b;
    delete[] mult;
    /** Calculate average and Std **/
    double sum = 0, sum_var = 0, average = 0, std_deviation = 0, variance = 0;
    for (int i = 0; i < iterations; i++) {
        sum += timespent[i];
    }
    average = sum / (float) iterations;
    /*  Compute  variance  and standard deviation  */
    for (int i = 0; i < iterations; i++) {
        sum_var = sum_var + pow((timespent[i] - average), 2);
    }
    variance = sum_var / (float) (iterations - 1);
    std_deviation = sqrt(variance);
    cout << "Average: " << average << " Standard deviation: " << std_deviation << endl;
    return 0;
}