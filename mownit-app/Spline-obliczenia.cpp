//
// Created by valleron on 4/16/26.
//

#include <iostream>

#include "calculations.cpp"
constexpr double check_step = ((3 * M_PI +1) - (-2 * M_PI + 1)) / 999;
constexpr double start = -2 * M_PI + 1;
constexpr double end = 3 * M_PI +1;


void tests(int n);

int main() {
    tests(50);
    return 0;
}
void tests(const int n) {

    for (int i = 5; i <= n; i+=5) { // n to liczba węzłów
        auto points = QVector<QPointF>();
        const double step = (end - start) / (i - 1);
        for (int j = 0; j < i; j++) {
            const double x = start + step * j;
            const double y = givenFunction(x);
            const QPointF point = QPointF(x, y);
            points.append(point);
        }
        QVector<SplineSegment> cubicSegmentsNatural = calculateNaturalSpline(points, 0, 0, false);
        QVector<SplineSegment> cubicSegmentsDerivative = calculateNaturalSpline(points, 8.0160724, -7.996323919,false);
        QVector<SplineSegment> cubicSegmentsKnot = calculateNaturalSpline(points, 0, 0, true);


        QVector<QuadraticSplineSegment> quadraticSegmentsNatural = calculateQuadraticSpline(points, 0, false);
        QVector<QuadraticSplineSegment> quadraticSegmentsDerivative = calculateQuadraticSpline(points, 21.147026328, false);
        QVector<QuadraticSplineSegment> quadraticSegmentsKnot = calculateQuadraticSpline(points, 0, true);


        auto *estimated = new double[1000];
        auto *splineNatural = new double[1000];
        auto *splineDerivative = new double[1000];
        auto *splineKnot = new double[1000];
        auto *quadraticNatural = new double[1000];
        auto *quadraticDerivative = new double[1000];
        auto *quadraticKnot = new double[1000];

        for (int j = 0; j < 1000; j++) {
            const double x = start + j * check_step;
            estimated[j] = givenFunction(x);

            splineNatural[j] = getSplineValue(x, cubicSegmentsNatural);
            splineDerivative[j] = getSplineValue(x, cubicSegmentsDerivative);
            splineKnot[j] = getSplineValue(x, cubicSegmentsKnot);

            quadraticNatural[j] = getQuadraticSplineValue(x, quadraticSegmentsNatural);
            quadraticDerivative[j] = getQuadraticSplineValue(x, quadraticSegmentsDerivative);
            quadraticKnot[j] = getQuadraticSplineValue(x, quadraticSegmentsKnot);
        }

        std::cout << "--------------------------------------\n" << "Liczba wezlow (n) = " << i << std::endl;

        std::cout << "[3-stopien]\n";
        std::cout << "  Natural MSE: " << square_error(estimated, splineNatural, 1000) << " MAX: " << maximum_error(estimated, splineNatural, 1000) << "\n";
        std::cout << "  Derivative  MSE: " << square_error(estimated, splineDerivative, 1000) << " MAX: " << maximum_error(estimated, splineDerivative, 1000) << "\n";
        std::cout << "  Knot  MSE: " << square_error(estimated, splineKnot, 1000) << " MAX: " << maximum_error(estimated, splineKnot, 1000) << "\n";

        std::cout << "[2-stopien]\n";
        std::cout << "  Natural MSE: " << square_error(estimated, quadraticNatural, 1000) << " MAX: " << maximum_error(estimated, quadraticNatural, 1000) << "\n";
        std::cout << "  Derivative  MSE: " << square_error(estimated, quadraticDerivative, 1000) << " MAX: " << maximum_error(estimated, quadraticDerivative, 1000) << "\n";
        std::cout << "  Knot MSE: " << square_error(estimated, quadraticKnot, 1000) << " MAX: " << maximum_error(estimated, quadraticKnot, 1000) << "\n";

        delete[] estimated;
        delete[] splineNatural;
        delete[] splineDerivative;
        delete[] splineKnot;
        delete[] quadraticNatural;
        delete[] quadraticDerivative;
        delete[] quadraticKnot;
    }
}