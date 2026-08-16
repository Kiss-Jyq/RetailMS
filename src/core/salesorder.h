#pragma once

#include <QString>
#include <QVector>

/**
 * @brief 销售订单模型（脱敏示例）
 */
struct OrderItem {
    int     productId  = -1;
    QString name;
    int     qty        = 0;
    double  unitPrice  = 0.0;

    [[nodiscard]] double subtotal() const {
        return unitPrice * static_cast<double>(qty);
    }
};

struct SalesOrder {
    int           id        = -1;
    QVector<OrderItem> items;
    QString       createdAt;   // 创建时间（ISO 字符串）

    [[nodiscard]] double total() const {
        double t = 0.0;
        for (const auto& it : items) t += it.subtotal();
        return t;
    }
};
