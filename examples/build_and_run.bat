@echo off
chcp 65001 >nul
rem =============================================================================
rem build_and_run.bat — Part 1 수학 데모를 빌드하고 실행합니다.
rem   일반 명령 프롬프트에서 더블클릭/실행해도 되도록 내부에서 VS 환경을 잡습니다.
rem =============================================================================
setlocal
set "ROOT=%~dp0.."

rem --- Visual Studio 환경(vcvars64) 찾기 ---
set "VCVARS="
for %%E in (Community Professional Enterprise BuildTools) do (
    if exist "C:\Program Files\Microsoft Visual Studio\2022\%%E\VC\Auxiliary\Build\vcvars64.bat" set "VCVARS=C:\Program Files\Microsoft Visual Studio\2022\%%E\VC\Auxiliary\Build\vcvars64.bat"
)
if not defined VCVARS (
    echo [오류] Visual Studio 2022를 찾지 못했습니다. VS2022를 설치하거나 Developer Command Prompt에서 빌드하세요.
    exit /b 1
)
call "%VCVARS%" >nul 2>&1

rem --- 컴파일 ( /utf-8 필수: 한글 주석을 올바로 읽게 함 ) ---
echo [빌드 중] math_demo ...
if not exist "%ROOT%\obj" mkdir "%ROOT%\obj"
cl /nologo /utf-8 /EHsc /std:c++20 /I "%ROOT%\src" "%ROOT%\examples\math_demo.cpp" "%ROOT%\src\math\Vector2.cpp" "%ROOT%\src\math\Vector3.cpp" "%ROOT%\src\math\Vector4.cpp" "%ROOT%\src\math\Matrix.cpp" /Fe:"%ROOT%\math_demo.exe" /Fo:"%ROOT%\obj\\" >nul
if errorlevel 1 ( echo [빌드 실패] & exit /b 1 )

rem --- 실행 ---
echo [실행]
echo --------------------------------------------------
"%ROOT%\math_demo.exe"
echo --------------------------------------------------
endlocal