#include <AutoItConstants.au3>

; ������ҳ
Run("C:\Program Files (x86)\Google\Chrome\Application\chrome.exe https://vip.jd.com/home.html")
WinWaitActive("����-��ӭ��¼ - Google Chrome")
Sleep ( 5000 )

; ȫ��
Send("{F11}")
Sleep ( 2000 )

; ����˻���¼
MouseClick($MOUSE_CLICK_LEFT, 1040, 190) 
Sleep ( 2000 )

; ѡ�������
MouseClick($MOUSE_CLICK_LEFT, 1000, 330)
Sleep ( 2000 ) 
Send("Bohai@1985")
Sleep ( 2000 ) 

; ��¼
MouseClick($MOUSE_CLICK_LEFT, 1000, 420)
Sleep ( 5000 ) 

; ǩ��
MouseClick($MOUSE_CLICK_LEFT, 1155, 195) 
Sleep ( 2000 )

; �ر��˳�
WinClose("��������Ա�������һ���ר����������Ż�ȯ�Ȼ�Ա��Ȩ�����ھ�����Ա���ֲ���vip.jd.com���� - Google Chrome")
Sleep ( 1000 )