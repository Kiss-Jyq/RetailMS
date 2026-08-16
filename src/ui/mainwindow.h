#pragma once

#include "inventoryrepository.h"
#include "producttablemodel.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QPushButton;
class QTableView;
class QLabel;
QT_END_NAMESPACE

/**
 * @brief 主界面（代码布局，不依赖 .ui 文件）
 *
 * 只做三件事：展示、交互、把动作转交给 Repository。
 * 不含任何 SQL —— 这是重构后"界面瘦、逻辑胖在业务层"的体现。
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(InventoryRepository* repo, QWidget* parent = nullptr);

private slots:
    void refresh();
    void onSearch();
    void onAddProduct();
    void onSale();
    void onRepoError(const QString& msg);

private:
    InventoryRepository* m_repo;
    ProductTableModel*   m_model;

    QLineEdit*  m_searchEdit;
    QTableView* m_table;
    QLabel*     m_statusLabel;
};
