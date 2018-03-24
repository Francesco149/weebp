# https://stackoverflow.com/questions/2124753/how-can-i-use-powershell-with-the-visual-studio-command-prompt

param (
    [string]$arch = "amd64"
)

Push-Location "C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\Common7\Tools"
cmd /c "VsDevCmd.bat -arch=$arch&set" |
ForEach-Object {
    if ($_ -match "=") {
        $v = $_.split("="); set-item -force -path "ENV:\$($v[0])"  -value "$($v[1])"
    }
}
Pop-Location
Write-Host "`nVisual Studio 2017 Command Prompt variables set." -ForegroundColor Yellow