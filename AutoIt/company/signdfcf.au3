#include <AutoItConstants.au3>
#include <MsgBoxConstants.au3>

; 运行首页
Run("C:\Program Files (x86)\Google\Chrome\Application\chrome.exe https://jy.xzsec.com")
WinWaitActive("在线交易 东方财富网在线交易，安全、便捷、省钱 - Google Chrome")
Sleep ( 5000 )

; 全屏
Send("{F11}")

; 选择密码框
MouseClick($MOUSE_CLICK_LEFT, 950, 222)

; 输入密码
Send("195257")

; 选择验证码框
MouseClick($MOUSE_CLICK_LEFT, 950, 272)

; 提示输入验证码
MsgBox($MB_OK, "Help", "请输入验证码!") 

; 登录
Send("{Enter}")
Sleep ( 5000 ) 

; 点击新股申购
MouseClick($MOUSE_CLICK_LEFT, 200, 580) 
Sleep ( 2000 )

; 点击批量新股申购
MouseClick($MOUSE_CLICK_LEFT, 210, 635) 
Sleep ( 2000 )

; 点击全部批量新股申购
MouseClick($MOUSE_CLICK_LEFT, 335, 207) 
Sleep ( 2000 )

; 点击批量申购
MouseClick($MOUSE_CLICK_LEFT, 1020, 200) 
Sleep ( 2000 )

; 点击确认
MouseClick($MOUSE_CLICK_LEFT, 660, 400) 
Sleep ( 2000 )

; 关闭退出
;WinClose("在线交易 东方财富网在线交易，安全、便捷、省钱 - Google Chrome")
Sleep ( 1000 )