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

; �������Ի���
MouseClick($MOUSE_CLICK_LEFT, 620, 365) 
Sleep ( 2000 )

; ѡ���һ���û�
MouseClick($MOUSE_CLICK_LEFT, 620, 410)
Sleep ( 2000 ) 

; ��¼
MouseClick($MOUSE_CLICK_LEFT, 630, 400)
Sleep ( 5000 ) 

; ���Խ��
MouseClick($MOUSE_CLICK_LEFT, 120, 330) 
Sleep ( 2000 )

; �ر��˳�
WinClose("�Խ�� - Google Chrome")
Sleep ( 1000 )