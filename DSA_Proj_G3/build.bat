@echo off
REM Build and Test Script for Windows
REM Smart Autocomplete & Spell-Checker Suite

echo ============================================
echo  Smart Autocomplete - Build and Test
echo ============================================
echo.

REM Check for g++ compiler
where g++ >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: g++ compiler not found!
    echo Please install MinGW or add g++ to PATH
    pause
    exit /b 1
)

echo [1/5] Checking directory structure...
if not exist "src\" mkdir src
if not exist "include\" mkdir include
if not exist "data\" mkdir data
if not exist "build\" mkdir build
echo    - Directories OK

echo.
echo [2/5] Compiling source files...
g++ -std=c++17 -Wall -Wextra -O2 -Iinclude ^
    src/trie.cpp ^
    src/bloomfilter.cpp ^
    src/hashtable.cpp ^
    src/lrucache.cpp ^
    src/priorityqueue.cpp ^
    src/bktree.cpp ^
    src/editdistance.cpp ^
    src/sort.cpp ^
    src/spellchecker.cpp ^
    src/filehandler.cpp ^
    src/utils.cpp ^
    src/main.cpp ^
    -o autocomplete.exe

if %ERRORLEVEL% NEQ 0 (
    echo    - Compilation FAILED!
    pause
    exit /b 1
)
echo    - Compilation successful!

echo.
echo [3/5] Verifying executable...
if exist "autocomplete.exe" (
    echo    - Executable created successfully
) else (
    echo    - ERROR: Executable not found!
    pause
    exit /b 1
)

echo.
echo [4/5] Checking data files...
if not exist "data\dictionary.txt" (
    echo    - Creating sample dictionary...
    echo algorithm 250 > data\dictionary.txt
    echo autocomplete 300 >> data\dictionary.txt
    echo computer 350 >> data\dictionary.txt
    echo programming 430 >> data\dictionary.txt
    echo python 420 >> data\dictionary.txt
    echo java 410 >> data\dictionary.txt
    echo code 400 >> data\dictionary.txt
    echo data 500 >> data\dictionary.txt
    echo structure 440 >> data\dictionary.txt
    echo search 390 >> data\dictionary.txt
)
echo    - Dictionary ready

echo.
echo [5/5] Build Summary:
echo    - Compiler: g++ (C++17)
echo    - Optimization: O2
echo    - Output: autocomplete.exe
echo    - Status: SUCCESS

echo.
echo ============================================
echo  Build Complete!
echo ============================================
echo.
echo To run the application:
echo   autocomplete.exe
echo.
echo To run tests (if compiled):
echo   cd tests
echo   [run test executables]
echo.
echo To open web interface:
echo   Open frontend\index.html in your browser
echo.

pause
