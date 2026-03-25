//
// Created by valleron on 3/13/26.
//

#ifndef UNTITLE_MAINWINDOW_H
#define UNTITLE_MAINWINDOW_H

#include <QMainWindow>
#include "../PointsModel.h"


QT_BEGIN_NAMESPACE

namespace Ui {
    class MainWindow;
}

QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    void updatePlot();

    ~MainWindow() override;

private slots:
    void on_drawButton_clicked();
    void on_addButton_clicked();

private:
    PointsModel *model;
    Ui::MainWindow *ui;
    double start;
    double end;
    double step;
    double maxX;
    double minX;
    double maxY;
    double minY;
};


#endif //UNTITLE_MAINWINDOW_H