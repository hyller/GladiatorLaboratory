#include <AutoItConstants.au3>

; 运行首页
Run("C:\Program Files (x86)\Google\Chrome\Application\chrome.exe https://vip.jd.com/home.html")
WinWaitActive("京东-欢迎登录 - Google Chrome")
Sleep ( 5000 )

; 全屏
Send("{F11}")
Sleep ( 2000 )

; 点击账户登录
MouseClick($MOUSE_CLICK_LEFT, 1040, 190) 
Sleep ( 2000 )

; 选择用户框
MouseClick($MOUSE_CLICK_LEFT, 1000, 270)
Sleep ( 2000 ) 

; 选择第一个用户
MouseClick($MOUSE_CLICK_LEFT, 1000, 300)
Sleep ( 2000 ) 

; 登录
MouseClick($MOUSE_CLICK_LEFT, 1000, 420)
Sleep ( 5000 ) 

; 签到
MouseClick($MOUSE_CLICK_LEFT, 1110, 200) 
Sleep ( 2000 )

; 关闭退出
WinClose("【京东会员】京豆兑换、专享礼包、领优惠券等会员特权，尽在京东会员俱乐部（vip.jd.com）！ - Google Chrome")
Sleep ( 1000 )