//
// Created by valleron on 3/18/26.
//

#include "PointsModel.h"

PointsModel::PointsModel(QObject *parent) : QAbstractTableModel(parent) {}

int PointsModel::rowCount(const QModelIndex &parent) const {
    return points.count();
}

int PointsModel::columnCount(const QModelIndex &parent) const {
    return 2;
}

QVariant PointsModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) return {};

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        const QPointF &pt = points.at(index.row());
        return (index.column() == 0) ? pt.x() : pt.y();
    }
    return {};
}

QVariant PointsModel::headerData(int section, const Qt::Orientation orientation, int role) const {
   // if (role == Qt::DisplayRole) return QVariant();
    if (orientation == Qt::Horizontal) {
        return section == 0 ? "Współrzędna X" : "Współrzędna Y";
    }
    return section + 1;
}

bool PointsModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!index.isValid() || role != Qt::EditRole) return false;
    bool ok;
    const double val =value.toDouble(&ok);
    if (!ok) return false;

    if (index.column() == 0) points[index.row()].setX(val);
    if (index.column() == 1) points[index.row()].setY(val);

    emit dataChanged(index, index);
    return true;
}

Qt::ItemFlags PointsModel::flags(const QModelIndex &index) const {
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

void PointsModel::addPoint(double x, double y) {
    beginInsertRows(QModelIndex(), points.count(), points.count());
    points.append(QPointF(x, y));
    endInsertRows();
}

const QVector<QPointF> &PointsModel::getPoints() {
    return points;
}

void PointsModel::addEmptyRow() {
    beginInsertRows(QModelIndex(), points.size(), points.size());

    points.append(QPointF(0.0, 0.0));

    endInsertRows();
}

void PointsModel::newPoints(QVector<QPointF> &newPoints) {
    beginResetModel();
    points = newPoints;
    endResetModel();
}
