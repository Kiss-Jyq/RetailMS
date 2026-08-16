#pragma once

#include "product.h"

#include <QAbstractTableModel>
#include <QVector>

/**
 * @brief 商品表格模型（Model/View 核心）
 *
 * 把 QVector<Product> 暴露给 QTableView，UI 与数据解耦：
 * 仓库数据变了，调用 setProducts() 即可，视图自动刷新。
 */
class ProductTableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit ProductTableModel(QObject* parent = nullptr);

    void setProducts(const QVector<Product>& products);
    const Product& productAt(int row) const;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

private:
    QVector<Product> m_products;
    static const QStringList kHeaders;
};
