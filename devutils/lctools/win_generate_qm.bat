call win_env.bat

set PROJ_TRANS=%PROJ_BUILD%\translations

mkdir %PROJ_TRANS%

For /R %PROJ_HOME% %%G IN (*.ts) do (
	lrelease "%%G" -qm "%PROJ_TRANS%\%%~nG.qm"
)
