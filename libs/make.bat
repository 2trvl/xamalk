mkdir build
cd build
cmake ..
cmake --build . --target INSTALL --config Release
cd ..
del /f /s /q build 1>nul
rmdir /s /q build
