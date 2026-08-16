#include "inventoryrepository.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QVariant>

InventoryRepository::InventoryRepository(QObject* parent)
    : QObject(parent) {
    // 使用独立的连接名，避免多实例互相干扰
    m_db = QSqlDatabase::addDatabase("QSQLITE", "retail_conn");
}

InventoryRepository::~InventoryRepository() {
    if (m_db.isOpen()) m_db.close();
    QSqlDatabase::removeDatabase("retail_conn");
}

bool InventoryRepository::open(const QString& path) {
    m_db.setDatabaseName(path);
    if (!m_db.open()) {
        emit errorOccurred("无法打开数据库：" + m_db.lastError().text());
        return false;
    }
    return migrate();
}

bool InventoryRepository::migrate() {
    QSqlQuery q(m_db);
    if (!q.exec(
            "CREATE TABLE IF NOT EXISTS products ("
            "  id            INTEGER PRIMARY KEY AUTOINCREMENT,"
            "  sku           TEXT NOT NULL UNIQUE,"
            "  name          TEXT NOT NULL,"
            "  category      TEXT,"
            "  price         REAL NOT NULL DEFAULT 0,"
            "  stock         INTEGER NOT NULL DEFAULT 0,"
            "  safety_stock  INTEGER NOT NULL DEFAULT 0"
            ");")) {
        emit errorOccurred("建表失败(products)：" + q.lastError().text());
        return false;
    }
    if (!q.exec(
            "CREATE TABLE IF NOT EXISTS sales_orders ("
            "  id         INTEGER PRIMARY KEY AUTOINCREMENT,"
            "  created_at TEXT NOT NULL,"
            "  total      REAL NOT NULL DEFAULT 0"
            ");")) {
        emit errorOccurred("建表失败(sales_orders)：" + q.lastError().text());
        return false;
    }
    return true;
}

void InventoryRepository::seedIfEmpty() {
    QSqlQuery check(m_db);
    if (check.exec("SELECT COUNT(*) FROM products") && check.next()) {
        if (check.value(0).toInt() > 0) return;
    }
    struct Seed { QString sku, name, cat; double price; int stock, safe; };
    const QVector<Seed> seeds = {
        {"SKU-1001", "云南白药牙膏 120g", "日化", 19.9, 120, 50},
        {"SKU-1002", "抽纸 3层 24包",     "纸品", 39.9, 30, 40},   // 低库存示例
        {"SKU-1003", "瓶装水 550ml*24",   "饮料", 29.0, 200, 60},
        {"SKU-1004", "洗衣液 2kg",        "清洁", 45.5, 80, 30},
    };
    for (const auto& s : seeds) {
        Product p;
        p.sku = s.sku; p.name = s.name; p.category = s.cat;
        p.price = s.price; p.stock = s.stock; p.safetyStock = s.safe;
        addProduct(p);
    }
}

bool InventoryRepository::addProduct(const Product& p) {
    if (!p.isValid()) {
        emit errorOccurred("商品数据不完整（sku / name 不能为空）");
        return false;
    }
    QSqlQuery q(m_db);
    q.prepare(
        "INSERT INTO products (sku, name, category, price, stock, safety_stock) "
        "VALUES (:sku, :name, :cat, :price, :stock, :safe)");
    q.bindValue(":sku", p.sku);
    q.bindValue(":name", p.name);
    q.bindValue(":cat", p.category);
    q.bindValue(":price", p.price);
    q.bindValue(":stock", p.stock);
    q.bindValue(":safe", p.safetyStock);
    if (!q.exec()) {
        emit errorOccurred("新增商品失败：" + q.lastError().text());
        return false;
    }
    return true;
}

bool InventoryRepository::updateStock(int productId, int delta) {
    QSqlQuery q(m_db);
    q.prepare("UPDATE products SET stock = stock + :delta WHERE id = :id");
    q.bindValue(":delta", delta);
    q.bindValue(":id", productId);
    if (!q.exec()) {
        emit errorOccurred("更新库存失败：" + q.lastError().text());
        return false;
    }
    return q.numRowsAffected() > 0;
}

QVector<Product> InventoryRepository::listProducts(const QString& filter) {
    QVector<Product> result;
    QSqlQuery q(m_db);
    QString sql =
        "SELECT id, sku, name, category, price, stock, safety_stock FROM products";
    if (!filter.isEmpty()) {
        sql += " WHERE name LIKE :f OR sku LIKE :f";
        q.prepare(sql);
        q.bindValue(":f", "%" + filter + "%");
    } else {
        q.prepare(sql);
    }
    if (!q.exec()) {
        emit errorOccurred("查询商品失败：" + q.lastError().text());
        return result;
    }
    while (q.next()) {
        Product p;
        p.id          = q.value(0).toInt();
        p.sku         = q.value(1).toString();
        p.name        = q.value(2).toString();
        p.category    = q.value(3).toString();
        p.price       = q.value(4).toDouble();
        p.stock       = q.value(5).toInt();
        p.safetyStock = q.value(6).toInt();
        result.append(p);
    }
    return result;
}

QVector<Product> InventoryRepository::lowStockAlerts() {
    QVector<Product> all = listProducts();
    QVector<Product> alerts;
    for (const auto& p : all)
        if (p.isLowStock()) alerts.append(p);
    return alerts;
}

double InventoryRepository::totalInventoryValue() const {
    QSqlQuery q(m_db);
    if (!q.exec("SELECT SUM(price * stock) FROM products")) return 0.0;
    if (q.next()) return q.value(0).toDouble();
    return 0.0;
}

bool InventoryRepository::recordSale(const SalesOrder& order) {
    if (order.items.isEmpty()) return false;
    if (!m_db.transaction()) {
        emit errorOccurred("开启事务失败：" + m_db.lastError().text());
        return false;
    }
    QSqlQuery q(m_db);
    q.prepare("INSERT INTO sales_orders (created_at, total) VALUES (:t, :total)");
    q.bindValue(":t", QDateTime::currentDateTime().toString(Qt::ISODate));
    q.bindValue(":total", order.total());
    if (!q.exec()) {
        m_db.rollback();
        emit errorOccurred("记录订单失败：" + q.lastError().text());
        return false;
    }
    const int orderId = q.lastInsertId().toInt();
    Q_UNUSED(orderId);
    for (const auto& it : order.items) {
        if (!updateStock(it.productId, -it.qty)) {
            m_db.rollback();
            return false;
        }
    }
    return m_db.commit();
}
