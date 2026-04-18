//
// Created by valleron on 3/19/26.
//
#include <QVector>
#include <QPointF>
#include <QtLogging>
#include "interpolationDrawing.cpp"

constexpr double h = 1e-14;
struct HermitePoint {
    double x;
    double y;
    double dy; // wartość pochodnej f'(x)

    HermitePoint(double _x = 0, double _y = 0, double _dy = 0)
        : x(_x), y(_y), dy(_dy) {}
};

struct SplineSegment {
    double a, b, c, d, x_start;
};

struct QuadraticSplineSegment {
    double a, b, c, x_start;
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
    for (int j = n-1; j >= 0; j--) {
        const double i = j + 1;
        double x = cos((((2*i) - 1) / (2 * k)) * M_PI);
        x+=1;
        x*=length/2;
        x+=a;
        result.append(x);
        qInfo("%f\n", x);
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


QVector<SplineSegment> calculateNaturalSpline(const QVector<QPointF>& points, double leftSigma, double rightSigma, bool notAKnot) {
    int n = points.size();
    if (n < 4 && notAKnot) return {};
    if (n < 3) return {};

    int m = n - 1;
    QVector<double> h(m);
    for (int i = 0; i < m; ++i) h[i] = points[i+1].x() - points[i].x();

    QVector<double> a_diag(n), b_sub(n), c_sup(n), d_vec(n);

    for (int i = 1; i < n - 1; ++i) {
        b_sub[i] = h[i-1];
        a_diag[i] = 2.0 * (h[i-1] + h[i]);
        c_sup[i] = h[i];
        double diffNext = (points[i+1].y() - points[i].y()) / h[i];
        double diffPrev = (points[i].y() - points[i-1].y()) / h[i-1];
        d_vec[i] = 6.0 * (diffNext - diffPrev);
    }

    //Warunki brzegowe (Wiersz 0 i n-1)
    if (notAKnot) {
        // Not-a-knot: S'''_0 = S'''_1  oraz  S'''_{n-2} = S'''_{n-1}
        a_diag[0] = h[1];
        c_sup[0] = -(h[0] + h[1]);
        double m0 = h[0] / a_diag[1];
        a_diag[0] -= m0 * b_sub[1];
        c_sup[0] -= m0 * a_diag[1];
        c_sup[0] += (h[0] - m0 * c_sup[1]);
        d_vec[0] = -m0 * d_vec[1];

        // Analogicznie dla końca przedziału:
        a_diag[n-1] = h[m-2];
        b_sub[n-1] = -(h[m-1] + h[m-2]);
        double mn = h[m-1] / a_diag[n-2];
        a_diag[n-1] -= mn * c_sup[n-2];
        b_sub[n-1] -= mn * a_diag[n-2];
        d_vec[n-1] = -mn * d_vec[n-2];
    } else {
        // User Defined Sigmas (np. Natural Spline)
        a_diag[0] = 1.0;
        c_sup[0] = 0.0;
        d_vec[0] = leftSigma;

        a_diag[n-1] = 1.0;
        b_sub[n-1] = 0.0;
        d_vec[n-1] = rightSigma;
    }

    for (int i = 1; i < n; ++i) {
        double weight = b_sub[i] / a_diag[i-1];
        a_diag[i] -= weight * c_sup[i-1];
        d_vec[i] -= weight * d_vec[i-1];
    }

    QVector<double> sigmas(n);
    sigmas[n-1] = d_vec[n-1] / a_diag[n-1];
    for (int i = n - 2; i >= 0; --i) {
        sigmas[i] = (d_vec[i] - c_sup[i] * sigmas[i+1]) / a_diag[i];
    }

    QVector<SplineSegment> segments(m);
    for (int i = 0; i < m; ++i) {
        segments[i].x_start = points[i].x();
        segments[i].a = points[i].y();
        segments[i].b = (points[i+1].y() - points[i].y()) / h[i] - h[i] * (sigmas[i+1] + 2.0 * sigmas[i]) / 6.0;
        segments[i].c = sigmas[i] / 2.0;
        segments[i].d = (sigmas[i+1] - sigmas[i]) / (6.0 * h[i]);
    }

    return segments;
}

double getSplineValue(double x, const QVector<SplineSegment>& segments) {
    if (segments.isEmpty()) return 0;

    // Znalezienie odpowiedniego przedziału
    int idx = 0;
    if (x <= segments[0].x_start) {
        idx = 0;
    } else if (x >= segments.last().x_start) {
        idx = segments.size() - 1;
    } else {
        // Wyszukiwanie binarne
        int low = 0, high = segments.size() - 1;
        while (low <= high) {
            int mid = (low + high) / 2;
            if (segments[mid].x_start <= x) {
                idx = mid;
                low = mid + 1;
            } else {
                high = mid - 1;
            }
        }
    }

    double dx = x - segments[idx].x_start;
    return segments[idx].a +
           segments[idx].b * dx +
           segments[idx].c * dx * dx +
           segments[idx].d * dx * dx * dx;
}


QVector<QuadraticSplineSegment> calculateQuadraticSpline(const QVector<QPointF>& points, double initialDerivative, bool notAKnot){
    int n = points.size();
    if (n < 2) return {};

    QVector<QuadraticSplineSegment> segments(n-1);

    double current_b;

    if (notAKnot) {
        double h0 = points[1].x() - points[0].x();
        double h1 = points[2].x() - points[1].x();
        double dy0 = (points[1].y() - points[0].y()) / h0;
        double dy1 = (points[2].y() - points[1].y()) / h1;

        // Formuła na b0 dla warunku a0 = a1:
        current_b = (dy0 * h1 * (2*h0 + h1) - dy1 * h0 * h0) / (h1 * (h0 + h1));
    } else {
        current_b = initialDerivative;
    }

    for (int i = 0; i < n-1; ++i) {
        double h = points[i+1].x() - points[i].x();
        double dy = points[i+1].y() - points[i].y();

        segments[i].x_start = points[i].x();
        segments[i].c = points[i].y(); // c_i to po prostu y_i
        segments[i].b = current_b;

        // Z równania S_i(x_i+1) = y_i+1 wyznaczamy a_i:
        // a_i * h^2 + b_i * h + c_i = y_i+1
        segments[i].a = (dy - current_b * h) / (h * h);

        // Wyliczamy pochodną na początku następnego przedziału
        current_b = 2 * segments[i].a * h + current_b;
    }

    return segments;
}

double getQuadraticSplineValue(double x, const QVector<QuadraticSplineSegment>& segments) {
    if (segments.isEmpty()) return 0;

    int idx = 0;
    if (x <= segments[0].x_start) {
        idx = 0;
    } else if (x >= segments.last().x_start) {
        idx = segments.size() - 1;
    } else {
        // Wyszukiwanie binarne
        int low = 0, high = segments.size() - 1;
        while (low <= high) {
            int mid = (low + high) / 2;
            if (segments[mid].x_start <= x) {
                idx = mid;
                low = mid + 1;
            } else {
                high = mid - 1;
            }
        }
    }

    double dx = x - segments[idx].x_start;
    return segments[idx].a * dx * dx + segments[idx].b * dx + segments[idx].c;
}

interpolationDrawing spilineInterpolation(const double minX, const double maxX, const QVector<QPointF>& points, double leftSigma, double rightSigma, bool knotAKnot) {
    QVector<SplineSegment> spline_segments = calculateNaturalSpline(points, leftSigma, rightSigma, knotAKnot);
    interpolationDrawing result;
    result.maxX = maxX;
    result.minX = minX;
    result.minY = -20;
    result.maxY = 20;
    const double step = (maxX - minX) / 500;
    for (double x = minX; x < maxX; x+=step) {
        double y = getSplineValue(x, spline_segments);
        result.xValues.push_back(x);
        result.yValues.push_back(y);
        result.minY = std::min(result.minY, y);
        result.maxY = std::max(result.maxY, y);
    }
    return result;
}
interpolationDrawing quadraticInterpolation(const double minX, const double maxX, const QVector<QPointF>& points, double leftSigma, bool notAKnot) {
    QVector<QuadraticSplineSegment> spline_segments = calculateQuadraticSpline(points, leftSigma, notAKnot);
    interpolationDrawing result;
    result.maxX = maxX;
    result.minX = minX;
    result.minY = -20;
    result.maxY = 20;
    const double step = (maxX - minX) / 500;
    for (double x = minX; x < maxX; x+=step) {
        double y = getQuadraticSplineValue(x, spline_segments);
        result.xValues.push_back(x);
        result.yValues.push_back(y);
        result.minY = std::min(result.minY, y);
        result.maxY = std::max(result.maxY, y);
    }
    return result;
}

double square_error(const double *estimated, const double *expected, const int n) {
    double sum = 0.0;
    for (int i=0;i<n;i++) {
        sum += (estimated[i] - expected[i]) * (estimated[i] - expected[i]);
    }
    return sqrt(sum) / n;

}

/*Oblicza błąd maksymalny
*@param estimated - wskaźnik do tablicy wartości interpolacji w n punktach
*@param expected - wskaźnik do tablicy wartości oczekiwanych w n punktach
*@param n - liczba punktów do sprawdzenia
*@return błąd maksymalny dla danego przybliżenia
*/
double maximum_error(const double *estimated, const double *expected, const int n) {
    double maximum = 0.0;
    for (int i=0;i<n;i++) {
        maximum = std::max(maximum, abs(estimated[i] - expected[i]));
    }
    return maximum;
}