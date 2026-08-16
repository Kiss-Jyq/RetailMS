#include "producttablemodel.h"

#include <QColor>

const QStringList ProductTableModel::kHeaders = {
    "ID", "货号", "名称", "分类", "单价", "库存", "安全库存", "库存价值", "状态"
};

ProductTableModel::ProductTableModel(QObject* parent)
    : QAbstractTableModel(parent) {}

void ProductTableModel::setProducts(const QVector<Product>& products) {
    beginResetModel();
    m_products = products;
    endResetModel();
}

const Product& ProductTableModel::productAt(int row) const {
    static const Product kInvalid;
    if (row < 0 || row >= m_products.size()) return kInvalid;
    return m_products.at(row);
}

int ProductTableModel::rowCount(const QModelIndex&) const {
    return m_products.size();
}

int ProductTableModel::columnCount(const QModelIndex&) const {
    return kHeaders.size();
}

QVariant ProductTableModel::headerData(int section, Qt::Orientation orientation,
                                       int role) const {
    if (role != Qt::DisplayRole) return {};
    if (orientation == Qt::Horizontal && section < kHeaders.size())
        return kHeaders.at(section);
    return {};
}

QVariant ProductTableModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= m_products.size()) return {};

    const Product& p = m_products.at(index.row());

    // 低库存行：整行红色前景
    if (role == Qt::ForegroundRole && p.isLowStock())
        return QColor(Qt::red);

    if (role != Qt::DisplayRole) return {};

    switch (index.column()) {
        case 0: return p.id;
        case 1: return p.sku;
        case 2: return p.name;
        case 3: return p.category;
        case 4: return QString::number(p.price, 'f', 2);
        case 5: return p.stock;
        case 6: return p.safetyStock;
        case 7: return QString::number(p.stockValue(), 'f', 2);
        case 8: return p.isLowStock() ? "⚠ 低库存" : "正常";
        default: return {};
    }
}
