#include <AutoItConstants.au3>
#include <MsgBoxConstants.au3>

; ������ҳ
Run("C:\Program Files (x86)\Google\Chrome\Application\chrome.exe https://jy.xzsec.com")
WinWaitActive("���߽��� �����Ƹ������߽��ף���ȫ����ݡ�ʡǮ - Google Chrome")
Sleep ( 5000 )

; ȫ��
Send("{F11}")

; ѡ�������
MouseClick($MOUSE_CLICK_LEFT, 950, 222)

; ��������
Send("195257")

; ѡ����֤���
MouseClick($MOUSE_CLICK_LEFT, 950, 272)

; ��ʾ������֤��
MsgBox($MB_OK, "Help", "��������֤��!") 

; ��¼
Send("{Enter}")
Sleep ( 5000 ) 

; ����¹��깺
MouseClick($MOUSE_CLICK_LEFT, 200, 580) 
Sleep ( 2000 )

; ��������¹��깺
MouseClick($MOUSE_CLICK_LEFT, 210, 635) 
Sleep ( 2000 )

; ���ȫ�������¹��깺
MouseClick($MOUSE_CLICK_LEFT, 335, 207) 
Sleep ( 2000 )

; ��������깺
MouseClick($MOUSE_CLICK_LEFT, 1020, 200) 
Sleep ( 2000 )

; ���ȷ��
MouseClick($MOUSE_CLICK_LEFT, 660, 400) 
Sleep ( 2000 )

; �ر��˳�
;WinClose("���߽��� �����Ƹ������߽��ף���ȫ����ݡ�ʡǮ - Google Chrome")
Sleep ( 1000 )