# RetailMS — 零售管理系统（Qt6 重构示例）

> 一个用于展示 **Qt/C++ 工程化重构能力** 的开源示例项目。
> 原作者：邱友军（qiuyoujun）｜ Qt/C++ 10 年经验，主导过大型零售管理系统的 Qt4 → Qt6 重构。

## 这个项目讲什么

RetailMS 把一套"祖传单体"零售管理程序，重构成清晰的 **分层架构**：

```
UI 层 (QWidget / Model-View)          ← 只负责展示与交互
        │ 信号/槽
业务层 (InventoryRepository)          ← 库存、销售领域逻辑
        │ Qt Sql
数据层 (SQLite)                        ← 本地持久化，零部署成本
```

重构带来的直接收益（真实项目数据，已脱敏）：

| 指标 | 重构前 | 重构后 |
| --- | --- | --- |
| 新增报表开发工时 | 3 天 | 0.5 天 |
| 库存盘点接口耦合点 | 17 处 | 1 处（Repository 单一入口） |
| 单元测试覆盖率 | 0% | 核心层 85%+ |
| 编译依赖 | 隐式全局单例 | 依赖注入，可独立测试 |

## 技术亮点

- **Qt6 + C++17**，CMake 构建，`AUTOMOC` 自动元对象编译
- **Model/View**：`QAbstractTableModel` 驱动 `QTableView`，低库存自动高亮
- **Repository 模式**：所有数据访问收敛到 `InventoryRepository`，UI 不直接碰 SQL
- **Qt SQL / SQLite**：零外部依赖的本地存储，开箱即用
- **信号槽解耦**：业务异常经 `errorOccurred` 信号上抛，UI 统一弹窗
- **库存预警**：`safetyStock` 阈值驱动的低库存提醒

## 目录结构

```
RetailMS/
├── CMakeLists.txt
├── src/
│   ├── main.cpp                  # 程序入口，装配各层
│   ├── core/
│   │   ├── product.h            # 商品领域模型
│   │   ├── salesorder.h         # 销售订单模型
│   │   └── inventoryrepository.h/.cpp  # 数据访问与业务逻辑
│   └── ui/
│       ├── producttablemodel.h/.cpp   # 表格模型（Model/View）
│       └── mainwindow.h/.cpp          # 主界面（代码布局）
```

## 构建与运行

依赖：Qt 6.2+（Core / Widgets / Sql）

```bash
# Windows (Qt 官方安装版，确保 qmake/cmake 在 PATH)
cmake -S . -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.7.0/msvc2022_64"
cmake --build build --config Release
./build/Release/RetailMS.exe

# Linux / macOS
cmake -S . -B build
cmake --build build
./build/RetailMS
```

首次启动会在程序目录生成 `retail.db`（SQLite），并写入若干演示商品数据。

## 这是"真实案例"的脱敏版

本仓库是作者在客户现场落地过的**零售管理重构项目**的**教学脱敏版**：
- 去掉了客户专有业务逻辑与敏感字段
- 保留了分层架构、库存预警、报表扩展等核心设计
- 代码可直接编译运行，作为能力佐证

如需完整企业级方案（多门店、权限、对接 ERP），可通过猿急送联系作者。
