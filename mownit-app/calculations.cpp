//
// Created by valleron on 3/19/26.
//
#include <QVector>
#include <QPointF>


QVector<double> calculateNewtonCoefficients(const QVector<QPointF>& points) {
    int n = points.size();
    QVector<double> a(n);
    for (int i = 0; i < n; ++i) {
        a[i] = points[i].y();
    }

    for (int j = 1; j < n; ++j) {
        for (int i = n - 1; i >= j; --i) {
            double denominator = points[i].x() - points[i - j].x();
            if (std::abs(denominator) > 1e-9) {
                a[i] = (a[i] - a[i - 1]) / denominator;
            }
        }
    }
    return a;
}

double calculateNewton(double x, const QVector<QPointF>& points, const QVector<double>& a) {
    int n = points.size();
    if (n == 0) return 0.0;
    double result = a[n - 1];
    for (int i = n - 2; i >= 0; --i) {
        result = result * (x - points[i].x()) / (1.0) + a[i];
    }
    return result;
}

double calculateLagrange(double x, const QVector<QPointF>& points) {
    double result = 0.0;
    int n = points.count();

    for (int i = 0; i < n; ++i) {
        double term = points[i].y();
        for (int j = 0; j < n; ++j) {
            if (i != j) {
                // Zapobieganie dzieleniu przez zero, jeśli użytkownik wpisze dwa takie same X
                double denominator = points[i].x() - points[j].x();
                if (std::abs(denominator) > 1e-9) {
                    term *= (x - points[j].x()) / denominator;
                }
            }
        }
        result += term;
    }
    return result;
}

double givenFunction(const double x) {
    return -2 * x * sin(x-1);
}
/*
 *@param n - ile miejsc zerowych zwrócić
 *@param k - stopień wielomianu czebyszewa
 *@param a - koniec przedziału
 *@param b - koniec przedziału
 */
double* chebyshev(const int n, const int k, const double a, const double b) {
    double *result = new double[n];
    double length = b - a;
    for (int j = 0; j < n; ++j) {
        double i = j;
        double x = cos((((2*i) - 1) / 2 * k) * M_PI);
        x+=1;
        x*=length/2;
        x+=a;
        result[j] = x;
    }
    return result;

}
