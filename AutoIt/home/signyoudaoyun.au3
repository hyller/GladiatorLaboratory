#include <AutoItConstants.au3>

; ����
Run("C:\Program Files (x86)\Youdao\YoudaoNote\RunYNote.exe")
WinWaitActive("�е��Ʊʼ�")
Sleep ( 1000 )

; ���ͷ��
MouseClick($MOUSE_CLICK_LEFT, 40, 50) 
Sleep ( 1000 )

; ���ǩ��
MouseClick($MOUSE_CLICK_LEFT, 860, 180) 
Sleep ( 1000 )

; �˳�ǩ������
MouseClick($MOUSE_CLICK_LEFT, 1070, 140) 
Sleep ( 1000 )

; �رճ���
WinClose("�е��Ʊʼ�")
Sleep ( 1000 )