ShellExecuteWait("g:\gaia_trunk\gaia_trunk.PR");
WinWait("Gaia_trunk Project") ;
WinMenuSelectItem("[si_frame]", "", "&Project", "Synchronize files...")
WinActive("Synchronize files") ;
send("{ENTER}");

