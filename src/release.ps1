# you must allow script execution by running
# 'Set-ExecutionPolicy RemoteSigned' in an admin powershell
# this requires vcvars to be already set (see vcvarsall17.ps1)
# 7zip is also required (choco install 7zip and add it to path)

$dir = Split-Path -Parent $MyInvocation.MyCommand.Definition

Push-Location "$dir"

function Write-Header($msg)
{
    Write-Host ""
    $hr = "#" * $Host.UI.RawUI.BufferSize.Width
    Write-Host $hr -Foreground Yellow -Background Black
    Write-Host "> $msg" -Foreground Yellow -Background Black
}

Write-Header "Compiling and stripping"
cmd /c "build.bat"

Write-Header "Packaging"
$folder = "weebp-" + $(.\wp.exe version) + "-windows-"
$clout = & cl 2>&1 | ForEach-Object{ "$_" }
"$clout" -match "(Microsoft.*for )([a-z0-9\-_]+)" | Out-Null
$folder = $folder + $Matches[2]

$files = "0bootstrap.bat","install.ps1","wp.exe","wp-headless.exe","weebp.dll",`
    "weebp.lib"
$package_files = ".gitignore","README.md","UNLICENSE"

mkdir $folder

foreach ($file in $files) {
    Copy-Item $file $folder
}

git -C .. archive HEAD -o src\$folder\src.zip
Set-Location $folder
&7z x src.zip
Set-Location ..

if (Test-Path "$folder.zip") {
    Remove-Item "$folder.zip"
}

$files += $package_files

&7z a "$folder.zip" $folder\src $($files | ForEach-Object { ,"$folder\$_" })

Write-Header "Result:"
&7z l "$folder.zip"

Remove-Item $folder -Force -Recurse
Pop-Location
