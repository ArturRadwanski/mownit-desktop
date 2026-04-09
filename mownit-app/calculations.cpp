//
// Created by valleron on 3/19/26.
//
#include <QVector>
#include <QPointF>
#include "interpolationDrawing.cpp"

constexpr double h = 1e-14;
struct HermitePoint {
    double x;
    double y;
    double dy; // wartość pochodnej f'(x)

    HermitePoint(double _x = 0, double _y = 0, double _dy = 0)
        : x(_x), y(_y), dy(_dy) {}
};

double givenFunction(const double x) {
    return -2 * x * sin(2*(x-1));
}
double givenDifferential(const double x) {
    return -2 * (sin(2*(x-1)) + x * 2 * cos(2*(x-1)));
}

QVector<double> calculateHermiteCoefficients(const QVector<HermitePoint>& points) {
    int n = points.size();
    int m = 2 * n;
    QVector<double> z(m);
    QVector<QVector<double>> table(m, QVector<double>(m));
    for (int i = 0; i < n; ++i) {
        z[2 * i] = points[i].x;
        z[2 * i + 1] = points[i].x;
        table[2 * i][0] = points[i].y;
        table[2 * i + 1][0] = points[i].y;

        table[2 * i + 1][1] = points[i].dy;
        if (i > 0) {
            table[2 * i][1] = (table[2 * i][0] - table[2 * i - 1][0]) / (z[2 * i] - z[2 * i - 1]);
        }
    }

    for (int j = 2; j < m; ++j) {
        for (int i = j; i < m; ++i) {
            table[i][j] = (table[i][j - 1] - table[i - 1][j - 1]) / (z[i] - z[i - j]);
        }
    }
    QVector<double> a;
    for (int i = 0; i < m; ++i) {
        a.append(table[i][i]);
    }
    return a;
}

QVector<HermitePoint> pointsToHermitePoints(const QVector<QPointF>& points) {
    QVector<HermitePoint> result;
    foreach (const QPointF& point, points) {
        const double dy = givenDifferential(point.x());
        result.append(HermitePoint(point.x(), point.y(), dy));
    }
    return result;
}

double calculateHermiteValue(double x, const QVector<HermitePoint>& points, const QVector<double>& a) {
    int n = points.size();
    int m = 2 * n;
    double result = a[m - 1];

    for (int i = m - 2; i >= 0; --i) {
        // z[i] to points[i/2].x
        double zi = points[i / 2].x;
        result = a[i] + (x - zi) * result;
    }
    return result;
}

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
        result = result * (x - points[i].x()) + a[i];
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


/*
 *@param n - ile miejsc zerowych zwrócić
 *@param k - stopień wielomianu czebyszewa
 *@param a - koniec przedziału
 *@param b - koniec przedziału
 */
QVector<double> chebyshev(const int n, const int k, const double a, const double b) {
    QVector<double> result;
    double length = b - a;
    for (int j = 0; j < n; ++j) {
        const double i = j + 1;
        double x = cos((((2*i) - 1) / (2 * k)) * M_PI);
        x+=1;
        x*=length/2;
        x+=a;
        result.append(x);
    }
    return result;

}

interpolationDrawing lagrangeInterpolation(const double minX, const double maxX, const QVector<QPointF>& points) {
    interpolationDrawing result;
    result.maxX = maxX;
    result.minX = minX;
    result.minY = -20;
    result.maxY = 20;
    const double step = (maxX - minX) / 500;
    for (double x = minX; x < maxX; x+=step) {
        double y = calculateLagrange(x, points);
        result.xValues.push_back(x);
        result.yValues.push_back(y);
        result.minY = std::min(result.minY, y);
        result.maxY = std::max(result.maxY, y);
    }
    return result;
}

interpolationDrawing newtonInterpolation(const double minX, const double maxX, const QVector<QPointF>& points) {
    interpolationDrawing result;
    const auto a = calculateNewtonCoefficients(points);
    result.maxX = maxX;
    result.minX = minX;
    result.minY = -20;
    result.maxY = 20;
    const double step = (maxX - minX) / 500;
    for (double x = minX; x < maxX; x+=step) {
        double y = calculateNewton(x, points, a);
        result.xValues.push_back(x);
        result.yValues.push_back(y);
        result.minY = std::min(result.minY, y);
        result.maxY = std::max(result.maxY, y);
    }
    return result;
}

interpolationDrawing hermiteInterpolation(const double minX, const double maxX, const QVector<QPointF>& points) {
    QVector<HermitePoint> hermitePoints = pointsToHermitePoints(points);
    interpolationDrawing result;
    const auto a = calculateHermiteCoefficients(hermitePoints);
    result.maxX = maxX;
    result.minX = minX;
    result.minY = -20;
    result.maxY = 20;
    const double step = (maxX - minX) / 500;
    for (double x = minX; x < maxX; x+=step) {
        double y = calculateHermiteValue(x, hermitePoints, a);
        result.xValues.push_back(x);
        result.yValues.push_back(y);
        result.minY = std::min(result.minY, y);
        result.maxY = std::max(result.maxY, y);
    }
    return result;
}
