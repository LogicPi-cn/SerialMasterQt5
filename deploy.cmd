@echo off

REM win deploy 工具
set deploy=D:\Qt\5.12.7\msvc2017\bin\windeployqt.exe

REM 源文件
set source_exe_path=release\SerialMaster.exe

REM 目标文件
set target_path=ReleaseOutput
set target_exe=SerialMaster.exe

REM 清空目录
RD /S /Q %target_path%
mkdir %target_path%

REM 复制exe到data目录
copy %source_exe_path% %target_path%\%target_exe%

REM QT依赖库生成
%deploy% %target_path%\%target_exe% --release