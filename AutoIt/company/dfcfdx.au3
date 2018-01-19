#include <AutoItConstants.au3>
#include <MsgBoxConstants.au3>

;==============================================================================
; 登录
;==============================================================================

; 运行
Run("C:\eastmoney\EmTrade\EmTrade.exe")
WinWaitActive("[CLASS:#32770]")

; 输入验证码！
Local $sCheckCode = InputBox( "验证码","输入验证码","","",200,150,400)

; 输入xxxx
ControlSetText ( "", "", "[ID:30044]", "195257" )

; 输入验证码！
ControlSetText ( "", "", "[ID:30099]", $sCheckCode )

; 登录
ControlClick("", "", "[ID:30046]")

; 等待主交易界面
Local $hWin = WinWaitActive("东方财富证券")
Local $hTreeView = ControlGetHandle($hWin, "", "[ID:2260]")
Sleep(1000)

;==============================================================================
; 批量新股申购
;==============================================================================
;ControlTreeView($hWin, "", $hTreeView, "Select",  "#6|#1")
;ControlClick($hWin, "", "批量申购") 
;
;send("{ENTER}");
;send("{ENTER}");
;send("{ENTER}");


;==============================================================================
; 批量新债申购
;==============================================================================
ControlTreeView($hWin, "", $hTreeView, "Select",  "#7|#1")
Sleep(5000)

ControlClick($hWin, "", "[ID:20074]")
Sleep(1000)

; 确认
Local $hWin_Sgxz = WinWaitActive("确认一键申购以下新债")
ControlClick($hWin_Sgxz, "", "确认申购")
Sleep(1000)

; 再次确认确认
WinWaitActive("提示")
send("{ENTER}");
Sleep(1000)

; 结束
WinWaitActive("","确认")
Send("{ENTER}");
Sleep(1000)

; 返回
Send("{ESC}");
Sleep(1000)

;==============================================================================
;退出
;==============================================================================
WinClose($hWin)
Sleep(1000)
Send("{ENTER}");