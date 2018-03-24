function Get-ScriptDirectory
{
    $Invocation = (Get-Variable MyInvocation -Scope 1).Value
    Split-Path $Invocation.MyCommand.Path
}

$scriptdir = $(Get-ScriptDirectory)

$oldpath = [Environment]::GetEnvironmentVariable("PATH", "User")
$newpath = "$scriptdir\;$oldpath".Split(";")
$newpath = $newpath | Select-Object -uniq
$newpath = $newpath -join ";"
[Environment]::SetEnvironmentVariable("PATH", "$newpath", "User")

function Save-Shortcut($exe, $name, $params)
{
    $s = (New-Object -COM WScript.Shell).CreateShortcut(
        "$env:USERPROFILE\Desktop\$name.lnk")
    $s.TargetPath = "$scriptdir\$exe"
    $s.WorkingDirectory = "$scriptdir"
    $s.Arguments = $params
    $s.Save()
}

Save-Shortcut "wp.exe" "wp add (fullscreen)" "add -f"
Save-Shortcut "wp.exe" "wp add" "add"
Save-Shortcut "wp-headless.exe" "mpv next" "mpv playlist-next"
