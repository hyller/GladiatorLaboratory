#include <AutoItConstants.au3>

; 运行首页
Run("C:\Program Files (x86)\Google\Chrome\Application\chrome.exe https://taojinbi.taobao.com/index.htm")
WinWaitActive("淘金币 - Google Chrome")
Sleep ( 2000 )

; 全屏
Send("{F11}")
Sleep ( 2000 )

; 点击淘金币
MouseClick($MOUSE_CLICK_LEFT, 120, 330) 
Sleep ( 2000 )

; 点击用户对话框
MouseClick($MOUSE_CLICK_LEFT, 660, 290) 
Sleep ( 2000 )

; 选择第一个用户
MouseClick($MOUSE_CLICK_LEFT, 660, 310)
Sleep ( 2000 ) 

; 登录
MouseClick($MOUSE_CLICK_LEFT, 630, 380)
Sleep ( 5000 ) 

; 领淘金币
MouseClick($MOUSE_CLICK_LEFT, 120, 330) 
Sleep ( 2000 )

; 关闭退出
WinClose("淘金币 - Google Chrome")
Sleep ( 1000 )