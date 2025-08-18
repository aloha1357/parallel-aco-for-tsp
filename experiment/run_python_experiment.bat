@echo off
echo =============================================
echo   Four Instances Parallel ACO Experiment
echo =============================================

REM Check Python installation
python --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: Python is not installed or not in PATH
    echo Please install Python 3.7+ and try again
    pause
    exit /b 1
)

REM Install required packages
echo Installing required Python packages...
pip install pandas numpy matplotlib seaborn scipy
if %errorlevel% neq 0 (
    echo WARNING: Some packages may not have been installed correctly
)

REM Check data files
echo Checking TSP instance files...
set missing_files=0

if not exist "..\data\eil51.tsp" (
    echo Missing file: ..\data\eil51.tsp
    set missing_files=1
)
if not exist "..\data\kroA100.tsp" (
    echo Missing file: ..\data\kroA100.tsp  
    set missing_files=1
)
if not exist "..\data\kroA150.tsp" (
    echo Missing file: ..\data\kroA150.tsp
    set missing_files=1
)
if not exist "..\data\gr202.tsp" (
    echo Missing file: ..\data\gr202.tsp
    set missing_files=1
)

if %missing_files%==1 (
    echo ERROR: Missing required TSP instance files
    echo Please ensure all .tsp files are in the data/ directory
    pause
    exit /b 1
)

echo All data files found

REM Select execution mode
echo.
echo Select execution mode:
echo [1] Development mode (2 iterations, 5 runs) - Quick test
echo [2] Production mode (100 iterations, 100 runs) - Full experiment
echo [3] Analysis only (analyze existing results)
echo [4] Exit
echo.
set /p choice="Enter choice (1-4): "

if "%choice%"=="1" (
    echo Running development mode experiment...
    python parallel_aco_experiment.py
) else if "%choice%"=="2" (
    echo Running production mode experiment...
    echo WARNING: Production mode may take several hours
    set /p confirm="Are you sure you want to continue? (y/N): "
    if /i "%confirm%"=="y" (
        python parallel_aco_experiment.py --production
    ) else (
        echo Cancelled
        goto end
    )
) else if "%choice%"=="3" (
    echo Available results directories:
    for /d %%i in (results\experiment_*) do echo   %%i
    echo.
    set /p results_dir="Enter results directory name: "
    if exist "results\%results_dir%" (
        python parallel_aco_experiment.py --analysis-only "results\%results_dir%"
    ) else (
        echo ERROR: Directory not found
    )
) else if "%choice%"=="4" (
    echo Exiting
    goto end
) else (
    echo Invalid choice
    goto end
)

:end
echo.
echo Script completed
pause
