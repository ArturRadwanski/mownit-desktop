//
// Created by valleron on 3/19/26.
//

#include <vector>
#include <tuple>
#include <math.h>

struct HermitePoint {
    double x;
    double y;
    double dy;

    HermitePoint(double _x = 0, double _y = 0, double _dy = 0)
        : x(_x), y(_y), dy(_dy) {}
};

double givenDifferential(const double x) {
    return -2 * (sin(2*(x-1)) + x * 2 * cos(2*(x-1)));
}

std::vector<double> calculateHermiteCoefficients(const std::vector<HermitePoint>& points) {
    int n = static_cast<int>(points.size());
    int m = 2 * n;

    std::vector<double> z(m);

    std::vector<std::vector<double>> table(m, std::vector<double>(m, 0.0));

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

    std::vector<double> a;
    a.reserve(m);
    for (int i = 0; i < m; ++i) {
        a.push_back(table[i][i]);
    }
    return a;
}

std::vector<HermitePoint> pointsToHermitePoints(const std::vector<std::tuple<double, double>>& points) {
    std::vector<HermitePoint> result;
    result.reserve(points.size());

    for (const auto& point : points) {
        // std::get<0> to x, std::get<1> to y
        const double x = std::get<0>(point);
        const double y = std::get<1>(point);

        const double dy = givenDifferential(x);
        result.emplace_back(x, y, dy);
    }
    return result;
}

double calculateHermiteValue(double x, const std::vector<HermitePoint>& points, const std::vector<double>& a) {
    int n = points.size();
    int m = 2 * n;
    double result = a[m - 1];

    for (int i = m - 2; i >= 0; --i) {
        double zi = points[i / 2].x;
        result = a[i] + (x - zi) * result;
    }
    return result;
}

/*Buduje tablicę ilorazów różnicowych
 *@param points - węzły interpolacji
 *@return - Wyniki z tablicy ilorazów różnicowych wykorzystywane we wzorze Newtona
 */
std::vector<double> calculateNewtonCoefficients(const std::vector<std::tuple<double, double>>points) {
    int n = points.size();
    std::vector<double> a(n);
    for (int i = 0; i < n; ++i) {
        a[i] = std::get<1>(points[i]);
    }

    for (int j = 1; j < n; ++j) {
        for (int i = n - 1; i >= j; --i) {
            double denominator = std::get<0>(points[i]) - std::get<0>(points[i - j]);
            if (std::abs(denominator) > 1e-9) {
                a[i] = (a[i] - a[i - 1]) / denominator;
            }
        }
    }
    return a;
}

/*Oblicza wartość wielomianu interpolującego metodą ilorazów różnicowych
 *@param x - wspórzędna x
 *@param points - węzły interpolacji
 *@param a - współczynniki z tablicy ilorazów różnicowych
 *@return wartość wielomianu interpolującego dla zadanego x
 */
double calculateNewton(double x, const std::vector<std::tuple<double, double>>& points, const std::vector<double>& a) {
    int n = points.size();
    if (n == 0) return 0.0;
    double result = a[n - 1];
    for (int i = n - 2; i >= 0; --i) {
        result = result * (x - std::get<0>(points[i])) + a[i];
    }
    return result;
}


/*Oblicza wartość wielomianu interpolującego metodą Lagrange'a
 *@param x - współrzędna x
 *@param points - węzły interpolacji
 *@result wartość wielomianu interpolującego dla zadanego x
 */
double calculateLagrange(double x, const std::vector<std::tuple<double, double>>& points) {
    double result = 0.0;
    int n = points.size();

    for (int i = 0; i < n; ++i) {
        double term = std::get<1>(points[i]);
        for (int j = 0; j < n; ++j) {
            if (i != j) {
                // Zapobieganie dzieleniu przez zero, jeśli użytkownik wpisze dwa takie same X
                double denominator = std::get<0>(points[i]) - std::get<0>(points[j]);
                if (std::abs(denominator) > 1e-9) {
                    term *= (x - std::get<0>(points[j])) / denominator;
                }
            }
        }
        result += term;
    }
    return result;
}

/*@param x - wartość x dla której obliczamy f(x)
 *@return f(x)
 */
double givenFunction(const double x) {
    return -2 * x * sin(2*(x-1));
}
/*
 *Znajduje określoną liczbę miejsc zerowych wielomianu czebyszewa i przeskalowuje je do zadanego przedziału
 *@param n - ile miejsc zerowych zwrócić
 *@param k - stopień wielomianu czebyszewa
 *@param a - koniec przedziału
 *@param b - koniec przedziału
 *@return wskaźnik do tablicy double w której są kolejne współrzędne x punktów
 */
double* chebyshev(const int n, const int k, const double a, const double b) {
    double *result = new double[n];
    double length = b - a;
    for (int j = 0; j < n; ++j) {
        const double i = j + 1;
        double x = cos((((2*i) - 1) / (2 * k)) * M_PI);
        x+=1;
        x*=length/2;
        x+=a;
        result[j] = x;
    }
    return result;

}
