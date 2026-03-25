//
// Created by valleron on 3/19/26.
//
#include "calculations.cpp"
#include <math.h>
#include <tuple>

double square_error(const double *estimated, const double *expected, const int n) {
    double sum = 0.0;
    for (int i=0;i<n;i++) {
        sum += (estimated[i] - expected[i]) * (estimated[i] - expected[i]);
    }
    return sqrt(sum) / n;

}
double maximum_error(const double *estimated, const double *expected, const int n) {
    double maximum = 0.0;
    for (int i=0;i<n;i++) {
        maximum = std::max(maximum, abs(estimated[i] - expected[i]));
    }
    return maximum;
}

std::tuple<double,double>* sample_interpolation_points(double* points, int n) {
    auto result = new std::tuple<double, double>[n];
    for (int i=0;i<n;i++) {
        result[i] = std::make_tuple(points[i], givenFunction(points[i]));
    }
    return result;
}


void calc_interpolations() {
    int n = 100;
    double start = -2 * M_PI + 1;
    double end = 3 * M_PI +1 ;
    //czebyszew:
    for (int i=1;i<n;i++) {
        double *chebyshevZeros = chebyshev(i, i, start, end);
        auto points = sample_interpolation_points(chebyshevZeros, i);
        delete chebyshevZeros;
        delete points;
    }

}

