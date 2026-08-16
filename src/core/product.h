#pragma once

#include <QString>
#include <QMetaType>

/**
 * @brief 商品领域模型（纯数据结构，无 UI / 无 SQL 依赖）
 *
 * 重构要点：把原来散落在各处的"商品字段"收敛为一个值对象，
 * 业务规则（低库存判定、库存价值）作为成员函数内聚在模型内。
 */
struct Product {
    int     id          = -1;   // 数据库自增主键，-1 表示未持久化
    QString sku;                // 货号
    QString name;               // 商品名
    QString category;           // 分类
    double  price       = 0.0;  // 单价（元）
    int     stock       = 0;    // 当前库存
    int     safetyStock = 0;    // 安全库存阈值

    [[nodiscard]] bool isValid() const {
        return !sku.isEmpty() && !name.isEmpty();
    }

    /// 库存价值 = 单价 × 数量
    [[nodiscard]] double stockValue() const {
        return price * static_cast<double>(stock);
    }

    /// 是否低于安全库存（触发预警）
    [[nodiscard]] bool isLowStock() const {
        return stock <= safetyStock;
    }
};

Q_DECLARE_METATYPE(Product)
