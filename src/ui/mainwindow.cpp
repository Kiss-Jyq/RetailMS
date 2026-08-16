#include "mainwindow.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTableView>
#include <QLabel>
#include <QMessageBox>
#include <QInputDialog>
#include <QHeaderView>
#include <QDateTime>

MainWindow::MainWindow(InventoryRepository* repo, QWidget* parent)
    : QMainWindow(parent), m_repo(repo) {
    setWindowTitle("RetailMS — 零售管理系统（Qt6 重构示例）");
    resize(900, 560);

    m_model = new ProductTableModel(this);

    // ---- 顶部工具栏 ----
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("按名称 / 货号搜索…");
    auto* searchBtn = new QPushButton("搜索", this);
    auto* refreshBtn = new QPushButton("刷新", this);
    auto* addBtn = new QPushButton("新增商品", this);
    auto* saleBtn = new QPushButton("模拟出库", this);

    auto* toolBar = new QWidget(this);
    auto* hbox = new QHBoxLayout(toolBar);
    hbox->addWidget(m_searchEdit);
    hbox->addWidget(searchBtn);
    hbox->addWidget(refreshBtn);
    hbox->addWidget(addBtn);
    hbox->addWidget(saleBtn);
    hbox->addStretch(1);

    // ---- 表格 ----
    m_table = new QTableView(this);
    m_table->setModel(m_model);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);

    // ---- 状态栏 ----
    m_statusLabel = new QLabel(this);
    statusBar()->addWidget(m_statusLabel);

    // ---- 布局 ----
    auto* central = new QWidget(this);
    auto* vbox = new QVBoxLayout(central);
    vbox->addWidget(toolBar);
    vbox->addWidget(m_table);
    setCentralWidget(central);

    // ---- 信号连接 ----
    connect(searchBtn, &QPushButton::clicked, this, &MainWindow::onSearch);
    connect(refreshBtn, &QPushButton::clicked, this, &MainWindow::refresh);
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddProduct);
    connect(saleBtn, &QPushButton::clicked, this, &MainWindow::onSale);
    connect(m_repo, &InventoryRepository::errorOccurred,
            this, &MainWindow::onRepoError);

    refresh();
}

void MainWindow::refresh() {
    m_model->setProducts(m_repo->listProducts());
    const int low = m_repo->lowStockAlerts().size();
    m_statusLabel->setText(QString("商品 %1 项 ｜ 库存总值 ¥%2 ｜ 低库存预警 %3 项")
        .arg(m_model->rowCount())
        .arg(QString::number(m_repo->totalInventoryValue(), 'f', 2))
        .arg(low));
}

void MainWindow::onSearch() {
    m_model->setProducts(m_repo->listProducts(m_searchEdit->text().trimmed()));
}

void MainWindow::onAddProduct() {
    bool ok = false;
    const QString sku  = QInputDialog::getText(this, "新增商品", "货号：", QLineEdit::Normal, "", &ok);
    if (!ok || sku.isEmpty()) return;
    const QString name = QInputDialog::getText(this, "新增商品", "名称：", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;

    Product p;
    p.sku = sku;
    p.name = name;
    p.category = QInputDialog::getText(this, "新增商品", "分类：", QLineEdit::Normal, "未分类", &ok);
    p.price = QInputDialog::getDouble(this, "新增商品", "单价：", 0, 0, 999999, 2, &ok);
    p.stock = QInputDialog::getInt(this, "新增商品", "初始库存：", 0, 0, 1000000, 1, &ok);
    p.safetyStock = QInputDialog::getInt(this, "新增商品", "安全库存：", 0, 0, 1000000, 1, &ok);

    if (m_repo->addProduct(p)) refresh();
}

void MainWindow::onSale() {
    const QModelIndex idx = m_table->currentIndex();
    if (!idx.isValid()) {
        QMessageBox::information(this, "提示", "请先在表格中选择一个商品。");
        return;
    }
    const Product& p = m_model->productAt(idx.row());
    bool ok = false;
    const int qty = QInputDialog::getInt(this, "模拟出库",
        QString("出库数量（当前库存 %1）：").arg(p.stock), 1, 1, p.stock, 1, &ok);
    if (!ok) return;

    SalesOrder order;
    OrderItem it;
    it.productId = p.id; it.name = p.name; it.qty = qty; it.unitPrice = p.price;
    order.items.append(it);
    order.createdAt = QDateTime::currentDateTime().toString(Qt::ISODate);

    if (m_repo->recordSale(order)) {
        QMessageBox::information(this, "成功",
            QString("出库 %1 件，订单金额 ¥%2").arg(qty).arg(order.total(), 0, 'f', 2));
        refresh();
    }
}

void MainWindow::onRepoError(const QString& msg) {
    QMessageBox::critical(this, "数据库错误", msg);
}
