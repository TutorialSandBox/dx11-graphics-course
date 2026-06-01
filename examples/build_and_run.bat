@echo off
:: =============================================================================
:: build_and_run.bat — Part 1 수학 데모를 빌드하고 실행합니다.
::   일반 명령 프롬프트에서 더블클릭/실행해도 되도록, 내부에서 VS 환경을 잡습니다.
:: =============================================================================
setlocal

:: 저장소 루트 = 이 배치 파일의 부모 폴더
set ROOT=%~dp0..

:: --- Visual Studio 환경(vcvars64) 찾기 ---
set "VCVARS="
for %%E in (Community Professional Enterprise BuildTools) do (
    if exist "C:\Program Files\Microsoft Visual Studio\2022\%%E\VC\Auxiliary\Build\vcvars64.bat" (
        set "VCVARS=C:\Program Files\Microsoft Visual Studio\2022\%%E\VC\Auxiliary\Build\vcvars64.bat"
    )
)
if not defined VCVARS (
    echo [오류] Visual Studio 2022를 찾지 못했습니다.
    echo        "Developer Command Prompt for VS 2022"에서 직접 빌드하거나, VS2022를 설치하세요.
    exit /b 1
)
call "%VCVARS%" >nul

:: --- 컴파일 ---
echo [빌드 중] math_demo ...
cl /nologo /EHsc /std:c++20 /I "%ROOT%\src" ^
   "%ROOT%\examples\math_demo.cpp" "%ROOT%\src\math\Vector2.cpp" "%ROOT%\src\math\Vector3.cpp" "%ROOT%\src\math\Vector4.cpp" ^
   /Fe:"%ROOT%\math_demo.exe" /Fo:"%ROOT%\\" >nul
if errorlevel 1 (
    echo [빌드 실패]
    exit /b 1
)

:: --- 실행 ---
echo [실행]
echo --------------------------------------------------
"%ROOT%\math_demo.exe"
echo --------------------------------------------------
endlocal
