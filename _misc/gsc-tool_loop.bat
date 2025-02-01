@echo off
setlocal

set appPath="K:\gsc-tool\gsc-tool.exe"

set targDir="E:\Call of Duty - Modern Warfare 3\DumpedFiles"


for /f "delims=|" %%f in (%targDir%) do (
	%appPath% -m decomp -g iw5 -s pc "%%f"
)

endlocal