@echo off
echo 清除VS工程临时文件，减小工程大小
for /r %%i in (
*.aps *.idb *.ncb *.obj *.pch *.sbr *.tmp *.pdb *.bsc *.ilk *.res
*.sdf *.dep *.ipch *.tlog *.exp *.hpj *.mdp *.plg *.clw *.user *.log
) do echo 删除"%%i" && del /f /q "%%i"
echo 清理完成！
echo .
pause
