# you must allow script execution by running
# 'Set-ExecutionPolicy RemoteSigned' in an admin powershell
# this requires vcvars to be already set (see vcvarsall17.ps1)
# 7zip is also required (choco install 7zip and add it to path)

$dir = Split-Path -Parent $MyInvocation.MyCommand.Definition

Push-Location "$dir"

function Write-Header($msg)
{
    Write-Host ""
    Write-Host "########################" -Foreground Yellow -Background Black
    Write-Host "> $msg" -Foreground Yellow -Background Black
}

Write-Header "Compiling and stripping"
cmd /c "build.bat"

Write-Header "Packaging"
$folder = "weebp-" + $(.\wp.exe version) + "-windows-"
$clout = & cl 2>&1 | ForEach-Object{ "$_" }
"$clout" -match "(Microsoft.*for )([a-z0-9\-_]+)" | Out-Null
$folder = $folder + $Matches[2]
mkdir $folder
Copy-Item 0bootstrap.bat $folder
Copy-Item install.ps1 $folder
Copy-Item wp.exe $folder
Copy-Item wp-headless.exe $folder
Copy-Item weebp.dll $folder
Copy-Item weebp.lib $folder
git -C .. archive HEAD -o src\$folder\src.zip
Set-Location $folder
&7z x src.zip
Set-Location ..

if (Test-Path "$folder.zip") {
    Remove-Item "$folder.zip"
}

&7z a "$folder.zip" $folder\0bootstrap.bat `
    $folder\wp.exe $folder\wp-headless.exe `
    $folder\weebp.dll $folder\weebp.lib $folder\src `
    $folder\install.ps1 $folder\.gitignore $folder\README.md `
    $folder\UNLICENSE

Write-Header "Result:"
&7z l "$folder.zip"

Remove-Item $folder -Force -Recurse
Pop-Location
