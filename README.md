# Parallel ACO for TSP – Project Start
<!-- # push test -->

**🎯 Current Status**: BDD Scenarios 1-8 實作完成 | 完整 ACO 引擎 | TSP文件加載 | 收斂監控 | 73/73 測試 | 95.9% 通過率 (70/73)

## 1. Project Goal

Implement a **shared‑memory parallel Ant Colony Optimization (ACO)** solver for the Traveling‑Salesman Problem on an Intel multi‑core CPU using **OpenMP 5.1**, developed with clean OOA/OOD/OOP principles and driven by **TDD + BDD**.  The repository will be public to the teaching staff from day 0 so that progress is fully transparent.

## 📊 Development Progress

| BDD Scenario | Status | Features | Tests |
|--------------|--------|----------|-------|
| ✅ **01_walking_skeleton** | 完成 | 建置環境驗證 | 1 |
| ✅ **02_construct_tour** | 完成 | Hamiltonian 迴路構建 | 6 |
| ✅ **03_probabilistic_choice** | 完成 | ACO 機率選擇 (τ^α·η^β) | 20 |
| ✅ **04_evaporation** | 完成 | 費洛蒙蒸發機制 | 10 |
| ✅ **05_delta_accumulation** | 完成 | 費洛蒙累積機制 | 6 |
| ✅ **06_delta_merge** | 完成 | 平行費洛蒙合併 | 4 |
| ✅ **07_parallel_consistency** | 完成 | OpenMP 平行化 | 4 |
| 🔄 **08_convergence** | 實作中 | 演算法收斂監控 | 4 (2 調整中) |
| ⏳ **09_performance_budget** | 待開發 | 性能最佳化 | - |
| ⏳ **10_numa_awareness** | 待開發 | NUMA 最佳化 | - |
| ⏳ **11_reproducibility** | 待開發 | 可重現性 | - |

**📈 測試統計**: 73 tests total | 70 passed ✅ | 1 skipped ⏭️ | 2 adjusting 🔄 | **95.9% pass rate**

## 🚀 Quick Start

```powershell
# 建置專案
cd "d:\D_backup\2025\tum\Parallel ACO for TSP"
mkdir build -ErrorAction SilentlyContinue
cd build
cmake ..
cmake --build .

# 運行測試 (包含 BDD 場景)
.\unit_tests.exe

# 運行特定測試群組
.\unit_tests.exe --gtest_filter="BDDScenariosTest.*"

# 執行主程式
.\aco_main.exe
```

## 🧪 Testing Framework

### ✅ **純 GoogleTest 方案**
- **統一測試框架**: 全部使用 GoogleTest (無外部依賴)
- **BDD 風格命名**: 保持可讀性 (e.g., `ConstructTour_ValidTour_HasPositiveLength`)
- **30 個測試**: 27 個通過 + 3 個未來功能佔位符
- **100% 通過率**: 所有實作功能完全驗證

### 🏗️ **測試結構**
- **單元測試**: Graph, Tour, Ant, PheromoneModel, AcoEngine (20 tests)
- **BDD 場景測試**: Walking Skeleton + Construct Tour (7 tests)  
- **未來功能**: Probabilistic Choice, Evaporation, Parallel (3 skipped)

## 🏗️ Implemented Core Features

### ✅ **Graph Representation**
- 對稱距離矩陣 (N×N)
- 隨機距離初始化
- 自訂距離設置 (測試用)

### ✅ **ACO Probabilistic Selection**
- 標準 ACO 機率公式: `P(i→j) = [τ(i,j)^α · η(i,j)^β] / Σ[τ(i,k)^α · η(i,k)^β]`
- 輪盤選擇演算法 (Roulette Wheel Selection)  
- 參數化 α (費洛蒙重要性) 和 β (距離重要性)
- 啟發式資訊計算 (η = 1/distance)

### ✅ **Pheromone Management**
- 費洛蒙矩陣初始化與管理
- 費洛蒙讀取/設置操作
- 最小費洛蒙值限制
- **費洛蒙蒸發機制**: `τ(i,j) ← (1-ρ) · τ(i,j)`
- **費洛蒙累積機制**: `τ(i,j) ← τ(i,j) + Δτ` where `Δτ = Q/L`

### ✅ **Tour Construction**
- Hamiltonian 迴路構建
- 自動路徑長度計算
- 訪問狀態管理

## 📈 Test Coverage: 45/45 (100%)

```
[==========] Running 45 tests from 6 test suites.
[  PASSED  ] 43 tests.
[  SKIPPED ] 2 tests.
```

**🎯 測試亮點**:
- **零外部依賴**: 純 GoogleTest 方案，無需 cucumber-cpp
- **BDD 可讀性**: 保持 BDD 風格的測試命名和結構  
- **完整覆蓋**: 所有實作功能 100% 驗證通過
- **蒸發機制**: 5 個單元測試 + 5 個 BDD 場景全面驗證
- **累積機制**: 4 個單元測試 + 2 個 BDD 場景驗證費洛蒙沉積

**詳細開發報告**: 請參閱 [`DEVELOPMENT_REPORT.md`](./DEVELOPMENT_REPORT.md)

---

## 2. High‑Level Architecture

Implement a **shared‑memory parallel Ant Colony Optimization (ACO)** solver for the Traveling‑Salesman Problem on an Intel multi‑core CPU using **OpenMP 5.1**, developed with clean OOA/OOD/OOP principles and driven by **TDD + BDD**.  The repository will be public to the teaching staff from day 0 so that progress is fully transparent.

---

## 2. High‑Level Architecture

```mermaid
classDiagram
    direction LR
    class Graph {
        +distance(i,j)
        +size()
    }
    class Tour {
        +length()
        +path : vector<int>
    }
    class Ant {
        +buildTour()
        -rngState
    }
    class PheromoneModel {
        +tau[i][j]
        +evaporate()
        +deposit()
    }
    class IScheduler {
        <<interface>>
        +configure()
    }
    class DynamicScheduler {
        +configure()
    }
    class ACOEngine {
        +run(iters)
        +bestTour()
        -ants : vector<Ant>
        -phero : PheromoneModel
        -sched : unique_ptr<IScheduler>
    }
    class MetricCollector {
        +onIteration()
        +report()
    }
    Graph <.. Ant
    Tour <.. Ant
    PheromoneModel <.. ACOEngine
    IScheduler <|.. DynamicScheduler
    IScheduler <.. ACOEngine
    MetricCollector <.. ACOEngine : observes
```

---

## 2.2 Algorithm Flow (Common ACO Loop)

```mermaid
flowchart TD
    S0[Start] --> S1[Initialise pheromone matrix τ]
    S1 --> S2[Iteration k = 1..K]
    S2 --> S3[Construct tours]
    S3 --> S4[Update local / global best]
    S4 --> S5[Evaporate pheromone]
    S5 --> S6{Stopping criteria?}
    S6 -- no --> S2
    S6 -- yes --> S7[Return best tour]
    style S0 fill:#dfefff,stroke:#369
    style S7 fill:#dfefff,stroke:#369
```

### 2.3 Strategy Micro‑flow: Serial vs. Parallel

The overall loop is identical; only **Step S3 – “Construct tours”** is replaced by different micro‑flows.

#### (a) Serial Construction Strategy

```mermaid
flowchart TD
    C0[For each ant i = 1..m] --> C1[Initialise starting city]
    C1 --> C2[Repeat until tour complete]
    C2 --> C3[Choose next city using roulette‑wheel rule]
    C3 --> C2
    C2 --> C4[Store finished tour i]
    C4 --> C5[Next i]
```

#### (b) Parallel Construction Strategy (OpenMP)

```mermaid
flowchart TD
    P0[omp parallel for (dynamic)] --> P1[Thread t builds tour for ant i]
    P1 --> P2[Thread‑local Δτ update]
    P2 --> P3{All threads done?}
    P3 -- no --> P1
    P3 -- yes --> P4[omp critical: merge Δτ]
    P4 --> P5[Reduction(min): update global best]
```

*Note:*  The parallel version keeps the same pheromone decision rule but isolates updates via **thread‑local buffers** and uses `reduction(min:bestCost)` for the global best path.

---

## 3. Development Flow

```mermaid
flowchart LR
    A[Baseline Serial ACO] --> B[Unit Tests Pass]
    B --> C[OpenMP Parallel Loop]
    C --> D[Profiling & Roofline]
    D --> E[Optimisation Round]
    E --> F[Documentation & CI]
    style A fill:#dfefff,stroke:#369
    style C fill:#dfefff,stroke:#369
    style D fill:#dfefff,stroke:#369
    style F fill:#dfefff,stroke:#369
```

---

## 4. Repository Layout (initial)

```
.
├── CMakeLists.txt
├── README.md
├── run_tests.sh
├── include/
│   ├── graph.hpp
│   ├── tour.hpp
│   ├── ant.hpp
│   ├── pheromone.hpp
│   ├── scheduler.hpp
│   ├── engine.hpp
│   └── metrics.hpp
├── src/
│   ├── main.cpp
│   └── ...
├── tests/
│   ├── test_graph.cpp
│   ├── test_ant.cpp
│   └── ...
├── data/
│   ├── eil101.tsp
│   ├── pcb442.tsp
│   └── pr1002.tsp
└── scripts/
    ├── benchmark_serial.sh
    └── roofline.py
```

---

## 5. Milestones & Timeline

| Week | Tag    | Deliverable                                      |
| ---- | ------ | ------------------------------------------------ |
| 1    | `v0.1` | Graph & Tour classes + unit tests + repo layout  |
| 2    | `v0.2` | Serial ACO complete; baseline CSV; CI green      |
| 3    | `v0.3` | OpenMP ACO ≥ 6× speed‑up; TDD coverage ≥ 80 %    |
| 4    | `v0.4` | Profiling & Roofline; optimised version          |
| 5    | `v1.0` | Final report, slides, and reproducibility script |

---

## 6. Coding & Process Principles

* **SOLID / DRY / KISS** adhered to in all classes.
* **TDD** with Catch2; tests live under `/tests` and run in CI.
* **BDD** Gherkin `.feature` files in `/features`; optional automation later.
* **CI** via GitLab CI: build, format check, unit tests, coverage.
* **Coverage** tracked with `gcov + lcov`, badge in README.

---

*Last updated: {{DATE}}*

## 3. Test Strategy – Granular "Pyramid"

> **Why**: Ensure each interchangeable component behaves correctly in isolation **before** validating combined behaviour in Serial/Parallel engines.

### 3.1 Test Layers

| Layer                        | Scope                          | Target Classes / Templates                  | Example Assertions                                 |
| ---------------------------- | ------------------------------ | ------------------------------------------- | -------------------------------------------------- |
| **Unit**                     | Pure functions / small classes | `Graph`, `DistanceMatrix`, `MatrixMul`      | `EXPECT_DOUBLE_EQ(distance(3,7), distance(7,3))`   |
| **Component**                | Multiple objects interact      | `Ant`, `PheromoneModel`, `SchedulerDynamic` | Ant visits n cities once, Δτ accumulates correctly |
| **Integration**              | Full engine (Seq / Par)        | `SerialACOEngine`, `ParallelACOEngine`      | Tour quality within 1 %, speed‑up ≥ 6×             |
| **System / Behaviour (BDD)** | CLI + config                   | `aco --omp 8 pr1002.tsp`                    | Exit 0, CSV output, runtime threshold              |

### 3.2 BDD Framework Choice

* **Framework** : **cucumber‑cpp** (2025‑04) + **Google Test** backend.
* **Scenario naming rule** : `<Layer>_<Given>_<Then>` ‑ e.g. `Integration_SerialSolveEil51_QualityWithin1pct`.
* **Runtime dependency** : Ruby ≥ 3.0 (for Cucumber CLI). GitLab CI image will install Ruby via `apt`.

### 3.3 Minimal Working Example

```bash
# features/serial_eil51.feature
Feature: Serial engine basic correctness
  Scenario: Integration_SerialSolveEil51_QualityWithin1pct
    Given a TSP file "eil51.tsp"
    When I solve it with "aco --seq"
    Then the tour length should be within 1% of 426
```

```cpp // features/steps/serial_steps.cpp
#include <gtest/gtest.h>
#include "bdd_macros.hpp"

GIVEN("a TSP file \"eil51.tsp\"") {
    graph = Graph::fromFile("eil51.tsp");
}
WHEN("I solve it with \"aco --seq\"") {
    SerialACOEngine eng(graph);
    eng.run(200);
    cost = eng.bestTour().length();
}
THEN("the tour length should be within 1% of 426") {
    EXPECT_NEAR(cost, 426.0, 4.26);
}
```

### 3.4 CMake & CI Snippets

```cmake
# CMakeLists.txt (excerpt)
find_package(GTest REQUIRED)
add_subdirectory(external/cucumber-cpp)
add_executable(bdd_runner
    features/steps/serial_steps.cpp
    ...)
target_link_libraries(bdd_runner
    cucumber-cpp::cucumber gtest gtest_main pthread)
add_test(NAME bdd COMMAND cucumber features)
```

```yaml
# .gitlab-ci.yml (excerpt)
stages: [build, test]

image: ubuntu:24.04

before_script:
  - apt-get update && apt-get install -y build-essential cmake ruby ruby-dev git
  - gem install cucumber --no-document

build:
  stage: build
  script:
    - cmake -B build -DCMAKE_CXX_STANDARD=20
    - cmake --build build -j$(nproc)

test:bdd:
  stage: test
  script:
    - cd build && ctest -L bdd --output-on-failure
```

### 3.5 Test‑Execution Order

1. **Unit tests** (Google Test) ‑ always run.
2. **Component tests** (Google Test fixtures) ‑ always run.
3. **Integration & BDD scenarios** (cucumber/cpp) ‑ run on push; mark heavy cases with `@slow` and schedule nightly.

### 3.6 Coverage

* Compile with `-fprofile-arcs -ftest-coverage`; use `gcov` + `lcov` to generate HTML.
* Upload coverage artefact in GitLab Pages for reviewer access.
