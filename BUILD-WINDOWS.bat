@echo off
echo Building R387 AI Decorator with the Geode CLI...
where geode >nul 2>nul
if errorlevel 1 (
  echo ERROR: Geode CLI was not found in PATH.
  echo Install Geode SDK/CLI using https://docs.geode-sdk.org/getting-started/
  pause
  exit /b 1
)
geode build
if errorlevel 1 (
  echo.
  echo Build failed. Check the SDK version and C++ errors above.
  pause
  exit /b 1
)
echo.
echo Build completed. Check the build output for the .geode package.
pause
