@echo off
set p0=%1
echo current path:%p0%

::echo -F 打包一个单个文件  -D 打包多个文件  -icon=路径  -v FILE
::echo --hidden-import 应用需要的包，但是没有被打包进来
::echo --upx-dir C:\upx3.96\
::echo -w --noconsole 去除调试黑窗
::echo --upx-exclude 排除需要压缩的文件

pyinstaller -n xBin2Dfu -F %p0% --distpath ./ --clean --upx-exclude=msvcp140.dll --upx-exclude=msvcp140_1.dll --upx-exclude=vcruntime140.dll --upx-exclude=python36.dll --upx-exclude=vcruntime140_1.dll --upx-exclude=qwindowsvistastyle.dll --upx-exclude=qwindows.dll --upx-exclude=qwebgl.dll --upx-exclude=qoffscreen.dll --upx-exclude=qminimal.dll --upx-exclude=qico.dll

pause
exit