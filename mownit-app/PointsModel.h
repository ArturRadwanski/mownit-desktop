//
// Created by valleron on 3/18/26.
//

#ifndef MOWNIT_DESKTOP_POINTSMODEL_H
#define MOWNIT_DESKTOP_POINTSMODEL_H
#include <qabstractitemmodel.h>
#include <QVector>
#include <QPointF>
using namespace std;

class PointsModel : public QAbstractTableModel{
public:
    explicit PointsModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void addPoint(double x, double y);
    const QVector<QPointF>& getPoints();
    void addEmptyRow();
    void newPoints(QVector<QPointF>& points);

private:
    QVector<QPointF> points;
};


#endif //MOWNIT_DESKTOP_POINTSMODEL_H