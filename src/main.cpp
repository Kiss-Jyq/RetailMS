#include "inventoryrepository.h"
#include "mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QStandardPaths>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("RetailMS");
    QApplication::setOrganizationName("qiuyoujun");

    // 数据库放在程序所在目录，便于演示与清理
    const QString dbPath = QDir::currentPath() + "/retail.db";

    InventoryRepository repo;
    if (!repo.open(dbPath)) return 1;
    repo.seedIfEmpty();

    MainWindow w(&repo);
    w.show();

    return app.exec();
}
