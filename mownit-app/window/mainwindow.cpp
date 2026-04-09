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

    ui->plotterWidget->graph(0)->setPen(QPen(Qt::red));

    ui->plotterWidget->graph(1)->setLineStyle(QCPGraph::lsNone); // Brak linii dla punktów
    ui->plotterWidget->graph(1)->setScatterStyle(QCPScatterStyle::ssCircle);
    //connect(model, &PointsModel::dataChanged, this, &MainWindow::updatePlot);
    start = -2 * M_PI + 1;
    end = 3 * M_PI +1;
    step = (end - start) / 99;
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
    ui->plotterWidget->legend->setVisible(true);

    ui->plotterWidget->legend->setFont(QFont("Helvetica", 9));
    ui->plotterWidget->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft | Qt::AlignBottom);


    ui->plotterWidget->graph(0)->setName("Interpolacja");
    ui->plotterWidget->graph(1)->setName("Węzły");
    ui->plotterWidget->graph(2)->setName("Funkcja bazowa");
    ui->plotterWidget->replot();

}

void MainWindow::on_drawButton_clicked() {
    printf("click\n");
    this->updatePlot();
    ui->plotterWidget->savePng("wykres.png",800,600);
}

void MainWindow::on_addButton_clicked() {
    model->addEmptyRow();
}

void MainWindow::updatePlot() {
    if (ui->formulaRadio ->isChecked()) {
        QVector<QPointF> *points = new QVector<QPointF>();
        int n = ui -> spinBox ->value();
        double step = (maxX - minX) / (n - 1);

            if (ui -> chebyshevRadio->isChecked()) {
                QVector<double> zeros = chebyshev(n, n, start, end);
                foreach (double x, zeros) {
                    points->append(QPointF(x, givenFunction(x)));
                }
            }
            else {
                for (int i=0;i<n;i++) {
                QPointF *point = new QPointF(start + (i * step), givenFunction(start + (i * step)));
                points->append(*point);
                delete point;
            }

        }
        model->newPoints(*points);
        delete points;
    }
    const QVector<QPointF>& controlPoints = model->getPoints();
    if (controlPoints.isEmpty()) return;

    QVector<double> xDots, yDots;   // Dane dla samych punktów kontrolnych


    for (const auto& p : controlPoints) {
        minX = std::min(minX, p.x());
        maxX = std::max(maxX, p.x());
        xDots << p.x();
        yDots << p.y();
    }

    interpolationDrawing draw;
    if (ui->lagrangeRadio->isChecked()) {
        draw = lagrangeInterpolation(minX, maxX, model->getPoints());
    }
    else if (ui->newtonRadio->isChecked()){
        draw = newtonInterpolation(minX, maxX, model->getPoints());
    }
    else {
        draw = hermiteInterpolation(minX, maxX, model->getPoints());
    }

    minY = draw.minY;
    maxY = draw.maxY;

    ui->plotterWidget->xAxis->setRange(minX - 1, maxX + 1);
    ui->plotterWidget->yAxis->setRange(minY - 2, maxY + 2);

    ui->plotterWidget->graph(0)->setData(draw.xValues, draw.yValues); // Wykres ciągły
    ui->plotterWidget->graph(1)->setData(xDots, yDots);   // Kropki w miejscach punktów

    ui->plotterWidget->replot();
}

MainWindow::~MainWindow() {
    delete ui;
}