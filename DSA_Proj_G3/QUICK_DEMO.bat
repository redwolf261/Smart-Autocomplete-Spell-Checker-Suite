@echo off
echo ============================================================
echo   QUICK DEMO - Just Watch!
echo ============================================================
echo.

cd c:\Users\HP\DSA_Proj_G3\DSA_Proj_G3

echo TEST 1: Searching for words starting with "prog"
echo.
"1`nprog`n6" | .\autocomplete.exe
echo.
echo Press any key for next test...
pause >nul

cls
echo TEST 2: Searching for words starting with "comp"
echo.
"1`ncomp`n6" | .\autocomplete.exe
echo.
echo Press any key for next test...
pause >nul

cls
echo TEST 3: Spell checking "program" (correct word)
echo.
"2`nprogram`n6" | .\autocomplete.exe
echo.
echo Press any key for next test...
pause >nul

cls
echo TEST 4: Spell checking "algoritm" (misspelled)
echo.
"2`nalgoritm`n6" | .\autocomplete.exe
echo.
echo Press any key to finish...
pause >nul

cls
echo ============================================================
echo   Demo Complete!
echo ============================================================
echo.
echo Try it yourself now:
echo   1. Run: .\autocomplete.exe
echo   2. Choose option 1 (Autocomplete) or 2 (Spell Check)
echo   3. Type any word or prefix
echo.
echo Or use the web version:
echo   1. Run: python webserver.py
echo   2. Open: http://localhost:8080/index.html
echo.
pause
