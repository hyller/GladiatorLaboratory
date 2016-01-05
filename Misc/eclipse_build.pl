
$eclipseBuild = "D:/System/PortableApps/eclipse-SDK-3.7-win32/eclipse/eclipsec.exe";
$eclipseOptions = "-nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild --launcher.suppressErrors";
$tempWorkspace = "tempWorkspace";
$projectDir = "./GSEB_Slave_Unitary";
$projectName = "GSEB_Slave_Unitary";
$config = "Debug";

$commandLine = "$eclipseBuild $eclipseOptions -data \"$tempWorkspace\" -import \"$projectDir\" -cleanBuild \"$projectName/$config\"";
print $commandLine;
system ("$commandLine");