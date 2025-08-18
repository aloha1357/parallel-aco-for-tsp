@echo off
echo ========================================
echo   四個實例平行ACO實驗快速執行腳本
echo ========================================

REM 檢查是否編譯過
if not exist "four_instances_experiment_runner.exe" (
    echo 實驗程式未編譯，正在編譯...
    call build_experiment.bat
    if %errorlevel% neq 0 (
        echo 編譯失敗，退出
        pause
        exit /b 1
    )
)

REM 檢查數據檔案
echo 檢查實驗數據檔案...
set missing_files=0

if not exist "..\data\eil51.tsp" (
    echo 缺少檔案: ..\data\eil51.tsp
    set missing_files=1
)
if not exist "..\data\kroA100.tsp" (
    echo 缺少檔案: ..\data\kroA100.tsp  
    set missing_files=1
)
if not exist "..\data\kroA150.tsp" (
    echo 缺少檔案: ..\data\kroA150.tsp
    set missing_files=1
)
if not exist "..\data\gr202.tsp" (
    echo 缺少檔案: ..\data\gr202.tsp
    set missing_files=1
)

if %missing_files%==1 (
    echo 錯誤: 缺少必要的TSP數據檔案
    echo 請確保data目錄下有所有需要的.tsp檔案
    pause
    exit /b 1
)

echo 所有數據檔案檢查完成

REM 詢問執行模式
echo.
echo 選擇執行模式:
echo [1] 開發模式 (2 iterations, 5 runs) - 快速測試
echo [2] 正式模式 (100 iterations, 100 runs) - 完整實驗
echo [3] 退出
echo.
set /p choice="請輸入選擇 (1-3): "

if "%choice%"=="1" (
    echo 執行開發模式實驗...
    four_instances_experiment_runner.exe
) else if "%choice%"=="2" (
    echo 執行正式模式實驗...
    echo 警告: 正式模式可能需要較長時間執行
    set /p confirm="確定要繼續嗎? (y/N): "
    if /i "%confirm%"=="y" (
        four_instances_experiment_runner.exe --production
    ) else (
        echo 取消執行
        goto end
    )
) else if "%choice%"=="3" (
    echo 退出
    goto end
) else (
    echo 無效選擇
    goto end
)

REM 檢查是否有Python用於結果分析
echo.
echo 實驗執行完成，檢查是否可以進行結果分析...
python --version >nul 2>&1
if %errorlevel% equ 0 (
    echo 發現Python，是否執行結果分析?
    set /p analyze="是否執行結果分析? (y/N): "
    if /i "%analyze%"=="y" (
        echo 安裝所需的Python套件...
        pip install pandas matplotlib seaborn scipy numpy
        echo 執行結果分析...
        python analyze_results.py
        echo 分析完成! 請查看 results/analysis/ 目錄中的結果
    )
) else (
    echo Python未安裝，跳過自動分析
    echo 如需分析結果，請安裝Python後執行: python analyze_results.py
)

:end
echo.
echo 腳本執行完成
pause
