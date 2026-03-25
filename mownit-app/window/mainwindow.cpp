//
// Created by valleron on 3/13/26.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../PointsModel.h"
#include "../calculations.cpp"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    model = new PointsModel(this);
    ui -> tableView -> setModel(model);

    ui->plotterWidget->addGraph(); // linia wielomianu
    ui->plotterWidget->addGraph(); // punkty kontrolne
    ui->plotterWidget->addGraph(); // zadana funkcja

    ui->plotterWidget->graph(0)->setPen(QPen(Qt::blue)); // Niebieska linia

    ui->plotterWidget->graph(1)->setLineStyle(QCPGraph::lsNone); // Brak linii dla punktów
    ui->plotterWidget->graph(1)->setScatterStyle(QCPScatterStyle::ssCircle);
    //connect(model, &PointsModel::dataChanged, this, &MainWindow::updatePlot);
    start = -2 * M_PI + 1;
    end = 3 * M_PI +1;
    step = (end - start) / 100;
    minX = start;
    maxX = end;
    minY = start;
    maxY = start;
    QVector<double> xGraph, yGraph;
    for (int i=0;i<100;i++) {
        xGraph << start + (i * step);
        yGraph << givenFunction(xGraph.last());
        minX = std::min(minX, xGraph.last());
        maxX = std::max(maxX, xGraph.last());
        minY = std::min(minY, yGraph.last());
        maxY = std::max(maxY, yGraph.last());
        model->addPoint(xGraph.last(), yGraph.last());
    }
    ui->plotterWidget->xAxis->setRange(minX - 1, maxX + 1);
    ui->plotterWidget->yAxis->setRange(minY - 2, maxY + 2);

    ui->plotterWidget->graph(2)->setData(xGraph, yGraph);
    ui->plotterWidget->replot();

}

void MainWindow::on_drawButton_clicked() {
    printf("click\n");
    this->updatePlot();
}

void MainWindow::on_addButton_clicked() {
    model->addEmptyRow();
}

void MainWindow::updatePlot() {
    if (ui->formulaRadio ->isChecked()) {
        QVector<QPointF> *points = new QVector<QPointF>();

        for (int i=0;i<10;i++) {
            QPointF *point = new QPointF(i, givenFunction(start + (i * step)));
            points->append(*point);
            delete point;
        }
        model->newPoints(*points);
        delete points;
    }
    const QVector<QPointF>& controlPoints = model->getPoints();
    if (controlPoints.isEmpty()) return;

    QVector<double> xGraph, yGraph; // Dane dla gładkiej linii
    QVector<double> xDots, yDots;   // Dane dla samych punktów kontrolnych


    for (const auto& p : controlPoints) {
        minX = std::min(minX, p.x());
        maxX = std::max(maxX, p.x());
        xDots << p.x();
        yDots << p.y();
    }


    double step = (maxX - minX) / 500.0;
    if (ui->lagrangeRadio->isChecked()) {
        for (double x = minX; x <= maxX; x += step) {
            xGraph << x;
            yGraph << calculateLagrange(x, controlPoints);
            minY = std::min(minY, yGraph.last());
            maxY = std::max(maxY, yGraph.last());
        }
    }
    else {
        auto a = calculateNewtonCoefficients(controlPoints);
        for (double x = minX; x <= maxX; x += step) {
            xGraph << x;
            yGraph << calculateNewton(x, controlPoints, a);
            minY = std::min(minY, yGraph.last());
            maxY = std::max(maxY, yGraph.last());
        }
    }

    ui->plotterWidget->xAxis->setRange(minX - 1, maxX + 1);
    ui->plotterWidget->yAxis->setRange(minY - 2, maxY + 2);

    ui->plotterWidget->graph(0)->setData(xGraph, yGraph); // Wykres ciągły
    ui->plotterWidget->graph(1)->setData(xDots, yDots);   // Kropki w miejscach punktów

    ui->plotterWidget->replot();
}

MainWindow::~MainWindow() {
    delete ui;
}