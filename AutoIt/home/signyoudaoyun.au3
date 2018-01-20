#include <AutoItConstants.au3>

; 运行
Run("C:\Program Files (x86)\Youdao\YoudaoNote\RunYNote.exe")
WinWaitActive("有道云笔记")
Sleep ( 1000 )

; 点击头像
MouseClick($MOUSE_CLICK_LEFT, 40, 50) 
Sleep ( 1000 )

; 点击签到
MouseClick($MOUSE_CLICK_LEFT, 860, 180) 
Sleep ( 1000 )

; 退出签到窗口
MouseClick($MOUSE_CLICK_LEFT, 1070, 140) 
Sleep ( 1000 )

; 关闭程序
WinClose("有道云笔记")
Sleep ( 1000 )