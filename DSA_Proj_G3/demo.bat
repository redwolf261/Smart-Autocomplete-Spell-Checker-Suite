@echo off
echo ============================================================
echo     SMART AUTOCOMPLETE - COMPREHENSIVE DEMO
echo ============================================================
echo.

cd c:\Users\HP\DSA_Proj_G3\DSA_Proj_G3

echo TEST 1: Autocomplete for "prog"
echo ----------------------------------------
(echo 1 & echo prog & echo 6) | autocomplete.exe
echo.
echo.

echo TEST 2: Autocomplete for "comp"
echo ----------------------------------------
(echo 1 & echo comp & echo 6) | autocomplete.exe
echo.
echo.

echo TEST 3: Spell Check - Correct Word "algorithm"
echo ----------------------------------------
(echo 2 & echo algorithm & echo 6) | autocomplete.exe
echo.
echo.

echo TEST 4: Spell Check - Misspelled Word "algoritm"
echo ----------------------------------------
(echo 2 & echo algoritm & echo 6) | autocomplete.exe
echo.
echo.

echo TEST 5: View Statistics
echo ----------------------------------------
(echo 4 & echo 6) | autocomplete.exe
echo.
echo.

echo ============================================================
echo     ALL TESTS COMPLETED!
echo ============================================================
pause
