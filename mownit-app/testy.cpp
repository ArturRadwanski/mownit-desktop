//
// Created by valleron on 3/19/26.
//
#include "calculation_no_qt.cpp"
#include <math.h>
#include <tuple>
#include <vector>
#include <iostream>

constexpr double check_step = ((3 * M_PI +1) - (-2 * M_PI + 1)) / 999;
constexpr double start = -2 * M_PI + 1;
constexpr double end = 3 * M_PI +1;

/*Oblicza błąd kwadratowy
 *@param estimated - wskaźnik do tablicy wartości interpolacji w n punktach
 *@param expected - wskaźnik do tablicy wartości oczekiwanych w n punktach
 *@param n - liczba punktów do sprawdzenia
 *@return błąd kwadratowy dla danego przybliżenia
 */
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


/*Tworzy listę węzłów interpolacji
 *@param points - wskaźnik do listy współrzędnych x interpolaji
 *@param n - liczba węzłów interpolacji
 *@return wektor węzłów interpolacji
 */
std::vector<std::tuple<double,double>> sample_interpolation_points(double* points, int n) {
    std::vector<std::tuple<double,double>> result = {};
    for (int i=0;i<n;i++) {
        result.emplace_back(points[i], givenFunction(points[i]));
    }
    return result;
}

/*Funkcja tworząca interpolacje mające od 1 do n węzłów i wypisująca błąd kwadratowy oraz maksymalny dla rozkladu węzłów jednostajnego
 *oraz zgodnego z miejscami zerowymi wielomianu czebyszewa
 *@param n - maksymalna liczba węzłów dla których robić test
 */
void calcInterpolations(const int n) {

    for (int i=1;i<n;i++) {
        double *chebyshevZeros = chebyshev(i, i, start, end);
        auto chebyshevPoints = sample_interpolation_points(chebyshevZeros, i);

        auto *evenPoints = new double[i];
        const double step = (end - start) / (i - 1);
        for (int j=0;j<i;j++) {
            evenPoints[j] = start + step * j;
        }

        auto evenInterpolationPoints = sample_interpolation_points(evenPoints, i);

        auto *estimated = new double[1000];
        auto *newtonEven = new double[1000];
        auto *newtonChebyshev = new double[1000];
        auto *lagrangianEven = new double[1000];
        auto *lagrangianChebyshev = new double[1000];
        auto chebyshevCoefficients = calculateNewtonCoefficients(chebyshevPoints);
        auto evenCoefficients = calculateNewtonCoefficients(evenInterpolationPoints);
        for (int j=0;j<1000;j++) {
            const double x = start + j * check_step;
            estimated[j] = givenFunction(x);
            newtonEven[j] = calculateNewton(x,evenInterpolationPoints, evenCoefficients);
            newtonChebyshev[j] = calculateNewton(x, chebyshevPoints, chebyshevCoefficients);
            lagrangianEven[j] = calculateLagrange(x, evenInterpolationPoints);
            lagrangianChebyshev[j] = calculateLagrange(x, chebyshevPoints);
        }

        const double newtonEvenSquare = square_error(estimated, newtonEven, 1000);
        const double newtonEvenMax = maximum_error(estimated, newtonEven, 1000);
        const double newtonChebyshevSquare = square_error(estimated, newtonChebyshev, 1000);
        const double newtonChebyshevMax = maximum_error(estimated, newtonChebyshev, 1000);
        const double lagrangeEvenSquare = square_error(estimated, lagrangianEven, 1000);
        const double lagrangeEvenMax = maximum_error(estimated, lagrangianEven, 1000);
        const double lagrangeChebyshevSquare = square_error(estimated, lagrangianChebyshev, 1000);
        const double lagrangeChebyshevMax = maximum_error(estimated, lagrangianChebyshev, 1000);

        std::cout << "--------------------------------------\n" << "n = " << i << std::endl;
        std::cout << "newtonEvenSquare = " << newtonEvenSquare << "\n";
        std::cout << "newtonEvenMax = " << newtonEvenMax << "\n";
        std::cout << "newtonChebyshevSquare = " << newtonChebyshevSquare << "\n";
        std::cout << "newtonChebyshevMax = " << newtonChebyshevMax << "\n";
        std::cout << "lagrangeEvenSquare = " << lagrangeEvenSquare << "\n";
        std::cout << "lagrangeEvenMax = " << lagrangeEvenMax << "\n";
        std::cout << "lagrangeChebyshevSquare = " << lagrangeChebyshevSquare << "\n";
        std::cout << "lagrangeChebyshevMax = " << lagrangeChebyshevMax << std::endl;
        delete[] chebyshevZeros;
        delete[] estimated;
        delete[] newtonEven;
        delete[] newtonChebyshev;
        delete[] lagrangianEven;
        delete[] lagrangianChebyshev;
        delete[] evenPoints;

    }

}

void calcInterpolationsHermite(const int n) {
    const double check_step = (end - start) / 999.0;

    for (int i = 2; i <= n; i++) { // n to liczba węzłów
        double *chebyshevZeros = chebyshev(i, i, start, end);
        auto *evenPoints = new double[i];
        const double step = (end - start) / (i - 1);
        for (int j = 0; j < i; j++) {
            evenPoints[j] = start + step * j;
        }

        auto chebyshevPointsLagrange = sample_interpolation_points(chebyshevZeros, i);
        auto evenPointsLagrange = sample_interpolation_points(evenPoints, i);

        std::vector<HermitePoint> chebyshevPointsHermite;
        std::vector<HermitePoint> evenPointsHermite;
        for (int j = 0; j < i; j++) {
            double xC = chebyshevZeros[j];
            double xE = evenPoints[j];
            chebyshevPointsHermite.emplace_back(xC, givenFunction(xC), givenDifferential(xC));
            evenPointsHermite.emplace_back(xE, givenFunction(xE), givenDifferential(xE));
        }

        auto hermiteEvenCoeffs = calculateHermiteCoefficients(evenPointsHermite);
        auto hermiteCzebCoeffs = calculateHermiteCoefficients(chebyshevPointsHermite);

        auto *estimated = new double[1000];
        auto *hermiteEven = new double[1000];
        auto *hermiteCzeb = new double[1000];
        auto *lagrangeEven = new double[1000];
        auto *lagrangeCzeb = new double[1000];

        for (int j = 0; j < 1000; j++) {
            const double x = start + j * check_step;
            estimated[j] = givenFunction(x);

            // Obliczenia wartości
            hermiteEven[j] = calculateHermiteValue(x, evenPointsHermite, hermiteEvenCoeffs);
            hermiteCzeb[j] = calculateHermiteValue(x, chebyshevPointsHermite, hermiteCzebCoeffs);
            lagrangeEven[j] = calculateLagrange(x, evenPointsLagrange);
            lagrangeCzeb[j] = calculateLagrange(x, chebyshevPointsLagrange);
        }

        // 6. Obliczanie błędów
        std::cout << "--------------------------------------\n" << "Liczba wezlow (n) = " << i << std::endl;

        // Wyniki dla węzłów równoodległych
        std::cout << "[Rownoodlegle]\n";
        std::cout << "  Lagrange MSE: " << square_error(estimated, lagrangeEven, 1000) << " MAX: " << maximum_error(estimated, lagrangeEven, 1000) << "\n";
        std::cout << "  Hermite  MSE: " << square_error(estimated, hermiteEven, 1000) << " MAX: " << maximum_error(estimated, hermiteEven, 1000) << "\n";

        // Wyniki dla węzłów Czebyszewa
        std::cout << "[Czebyszew]\n";
        std::cout << "  Lagrange MSE: " << square_error(estimated, lagrangeCzeb, 1000) << " MAX: " << maximum_error(estimated, lagrangeCzeb, 1000) << "\n";
        std::cout << "  Hermite  MSE: " << square_error(estimated, hermiteCzeb, 1000) << " MAX: " << maximum_error(estimated, hermiteCzeb, 1000) << "\n";

        // Sprzątanie
        delete[] chebyshevZeros;
        delete[] evenPoints;
        delete[] estimated;
        delete[] hermiteEven;
        delete[] hermiteCzeb;
        delete[] lagrangeEven;
        delete[] lagrangeCzeb;
    }
}

