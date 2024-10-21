Set-Location ".\external\vcpkg"
cmd.exe /c "bootstrap-vcpkg.bat" "-disableMetrics"
$env:VCPKG_ROOT = (Get-Location)
$env:PATH = "$env:VCPKG_ROOT;$env:PATH"
Set-Location "..\.."

vcpkg.exe install
