@echo off
echo ���VS������ʱ�ļ�����С���̴�С
for /r %%i in (
*.aps *.idb *.ncb *.obj *.pch *.sbr *.tmp *.pdb *.bsc *.ilk *.res
*.sdf *.dep *.ipch *.tlog *.exp *.hpj *.mdp *.plg *.clw *.user *.log
) do echo ɾ��"%%i" && del /f /q "%%i"
echo ������ɣ�
echo .
pause
