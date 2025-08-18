@echo off
echo 編譯四個實例實驗執行器...

REM 檢查編譯器
where g++ >nul 2>nul
if %errorlevel% neq 0 (
    echo 錯誤: 找不到 g++ 編譯器
    echo 請確保已安裝 MinGW 或其他 C++ 編譯器
    pause
    exit /b 1
)

REM 創建結果目錄
if not exist "results" mkdir results
if not exist "results\fixed_iterations" mkdir results\fixed_iterations
if not exist "results\time_budget" mkdir results\time_budget
if not exist "results\baseline_measurements" mkdir results\baseline_measurements
if not exist "results\analysis" mkdir results\analysis

REM 編譯實驗執行器
echo 編譯中...
g++ -std=c++17 -O3 -I../src -pthread four_instances_experiment_runner.cpp -o four_instances_experiment_runner.exe

if %errorlevel% equ 0 (
    echo 編譯成功!
    echo.
    echo 執行方式:
    echo   開發測試: four_instances_experiment_runner.exe
    echo   正式實驗: four_instances_experiment_runner.exe --production
    echo.
) else (
    echo 編譯失敗! 請檢查錯誤訊息
    pause
    exit /b 1
)

pause
