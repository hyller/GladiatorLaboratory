#include <AutoItConstants.au3>

; Run CCLeaner
ShellExecute("C:\Program Files\CCleaner\CCleaner64.exe");
WinWaitActive("Piriform CCleaner")
WinActivate("Piriform CCleaner")

;; Start analyze
;send("a")
;WinWaitActive("Piriform CCleaner", "ANALYSIS COMPLETE")
;WinActivate("Piriform CCleaner")
;
;; Run cleanner
;send("r")
;WinWaitActive("Piriform CCleaner", "CLEANING COMPLETE")
;WinActivate("Piriform CCleaner")

; Switch Registry tab
send("g")
WinWaitActive("Piriform CCleaner", "Scan for Issues")
WinActivate("Piriform CCleaner")

; Start scan for issue
send("s")
WinWaitActive("Piriform CCleaner", "&Fix selected issues...")
WinActivate("Piriform CCleaner")

; Start Fix selected issues
MouseClick ( "" ,1290,690,0 )
WinWaitActive("CCleaner", "Do you want to backup changes to the registry?")
WinActivate("CCleaner")
send("n")
MouseClick ( "" ,700,470,0 )
Sleep(5000)
;MouseClick ( "" ,700,470,0 )
