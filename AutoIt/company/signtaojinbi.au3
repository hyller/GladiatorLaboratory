#include <AutoItConstants.au3>

; ������ҳ
Run("C:\Program Files (x86)\Google\Chrome\Application\chrome.exe https://taojinbi.taobao.com/index.htm")
WinWaitActive("�Խ�� - Google Chrome")
Sleep ( 2000 )

; ȫ��
Send("{F11}")
Sleep ( 2000 )

; ����Խ��
MouseClick($MOUSE_CLICK_LEFT, 120, 330) 
Sleep ( 2000 )

; ����û��Ի���
MouseClick($MOUSE_CLICK_LEFT, 660, 290) 
Sleep ( 2000 )

; ѡ���һ���û�
MouseClick($MOUSE_CLICK_LEFT, 660, 310)
Sleep ( 2000 ) 

; ��¼
MouseClick($MOUSE_CLICK_LEFT, 630, 380)
Sleep ( 5000 ) 

; ���Խ��
MouseClick($MOUSE_CLICK_LEFT, 120, 330) 
Sleep ( 2000 )

; �ر��˳�
WinClose("�Խ�� - Google Chrome")
Sleep ( 1000 )