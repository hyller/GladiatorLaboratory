#include <AutoItConstants.au3>
#include <MsgBoxConstants.au3>

;==============================================================================
; ��¼
;==============================================================================

; ����
Run("C:\eastmoney\EmTrade\EmTrade.exe")
WinWaitActive("[CLASS:#32770]")

; ������֤�룡
Local $sCheckCode = InputBox( "��֤��","������֤��","","",200,150,400)

; ����xxxx
ControlSetText ( "", "", "[ID:30044]", "195257" )

; ������֤�룡
ControlSetText ( "", "", "[ID:30099]", $sCheckCode )

; ��¼
ControlClick("", "", "[ID:30046]")

; �ȴ������׽���
Local $hWin = WinWaitActive("�����Ƹ�֤ȯ")
Local $hTreeView = ControlGetHandle($hWin, "", "[ID:2260]")
Sleep(1000)

;==============================================================================
; �����¹��깺
;==============================================================================
;ControlTreeView($hWin, "", $hTreeView, "Select",  "#6|#1")
;ControlClick($hWin, "", "�����깺") 
;
;send("{ENTER}");
;send("{ENTER}");
;send("{ENTER}");


;==============================================================================
; ������ծ�깺
;==============================================================================
ControlTreeView($hWin, "", $hTreeView, "Select",  "#7|#1")
Sleep(5000)

ControlClick($hWin, "", "[ID:20074]")
Sleep(1000)

; ȷ��
Local $hWin_Sgxz = WinWaitActive("ȷ��һ���깺������ծ")
ControlClick($hWin_Sgxz, "", "ȷ���깺")
Sleep(1000)

; �ٴ�ȷ��ȷ��
WinWaitActive("��ʾ")
send("{ENTER}");
Sleep(1000)

; ����
WinWaitActive("","ȷ��")
Send("{ENTER}");
Sleep(1000)

; ����
Send("{ESC}");
Sleep(1000)

;==============================================================================
;�˳�
;==============================================================================
WinClose($hWin)
Sleep(1000)
Send("{ENTER}");