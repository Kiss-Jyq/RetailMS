#pragma once

#include "product.h"
#include "salesorder.h"

#include <QObject>
#include <QSqlDatabase>
#include <QVector>

/**
 * @brief 库存数据访问层（Repository 模式）
 *
 * 重构前：SQL 语句散落在 17 个界面槽函数里，改一个字段要全网搜索。
 * 重构后：所有数据访问收敛到本类，UI 永远不直接写 SQL。
 *         便于单元测试（可替换为内存库），也便于切换数据库。
 */
class InventoryRepository : public QObject {
    Q_OBJECT

public:
    explicit InventoryRepository(QObject* parent = nullptr);
    ~InventoryRepository() override;

    /// 打开（或创建）SQLite 数据库文件
    bool open(const QString& path);

    /// 建表（幂等）
    bool migrate();

    /// 写入演示数据（仅首次为空时）
    void seedIfEmpty();

    // ---- 库存操作 ----
    bool addProduct(const Product& p);
    bool updateStock(int productId, int delta);
    QVector<Product> listProducts(const QString& filter = QString());
    QVector<Product> lowStockAlerts();
    [[nodiscard]] double totalInventoryValue() const;

    // ---- 销售（脱敏简化）----
    bool recordSale(const SalesOrder& order);

signals:
    /// 业务/数据库异常统一上抛，UI 负责弹窗，避免到处写 QMessageBox
    void errorOccurred(const QString& message);

private:
    QSqlDatabase m_db;
};
