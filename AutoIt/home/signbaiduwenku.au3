#include <AutoItConstants.au3>

Run("C:\Program Files (x86)\Google\Chrome\Application\chrome.exe https://wenku.baidu.com/task/browse/daily")
WinWaitActive("��������_--�ٶ��Ŀ� - Google Chrome")
Sleep ( 5000 )

Send("{F11}")
Sleep ( 1000 )

Send("{END}")
Sleep ( 1000 )

MouseClick($MOUSE_CLICK_LEFT, 1000, 340) 
Sleep ( 5000 )

WinClose("��������_--�ٶ��Ŀ� - Google Chrome")
Sleep ( 1000 )