<#
.SYNOPSIS
    Builds e4dccommlib and the Python wrapper for one Python version.

.DESCRIPTION
    Used by .github/workflows/release.yml, and runnable by hand on any Windows
    machine with CMake, a C++ toolchain and uv:

        ./ci/build.ps1 -PythonVersion 3.11 -E4dcCommlibSource ..\..\ccpp\e4dccommlib

    Output: <OutDir>\e4dccommlib_python_wrapper.cp311-win_amd64.pyd

    The e4dccommlib build directory is shared between Python versions, so the
    second invocation only rebuilds the wrapper.
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory)] [ValidatePattern('^3\.\d+$')] [string] $PythonVersion,
    [Parameter(Mandatory)] [string] $E4dcCommlibSource,
    [string] $OutDir = 'dist',
    [string] $Pybind11Version = '3.1.0',
    [ValidateSet('Release', 'RelWithDebInfo', 'Debug')] [string] $Config = 'Release'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

# Windows PowerShell does not fail on native command errors: make it explicit.
function Invoke-Native {
    param([Parameter(Mandatory)] [string] $Exe, [string[]] $Arguments = @())
    Write-Host "> $Exe $($Arguments -join ' ')" -ForegroundColor Cyan
    & $Exe @Arguments
    if ($LASTEXITCODE -ne 0) { throw "'$Exe' failed with exit code $LASTEXITCODE" }
}

function Invoke-CMakeProject {
    param([string] $Source, [string] $Build, [string] $Prefix, [string[]] $Defines = @())
    Invoke-Native cmake (@('-S', $Source, '-B', $Build, "-DCMAKE_BUILD_TYPE=$Config", "-DCMAKE_CONFIGURATION_TYPES=$Config") + $Defines) | Out-Host
    Invoke-Native cmake @('--build', $Build, '--config', $Config, '--parallel') | Out-Host
    Invoke-Native cmake @('--install', $Build, '--config', $Config, '--prefix', $Prefix) | Out-Host
}

$root      = Split-Path -Parent $PSScriptRoot
$buildRoot = Join-Path $root 'build'
$libPrefix = Join-Path $buildRoot 'e4dccommlib-install'
$tag       = 'py' + ($PythonVersion -replace '\.', '')
$OutDir    = [IO.Path]::GetFullPath((Join-Path $root $OutDir))

# 1. Toolchain: the exact Python to build against + pybind11's CMake package.
Invoke-Native uv @('python', 'install', $PythonVersion) | Out-Host
$python      = (Invoke-Native uv @('python', 'find', $PythonVersion) | Select-Object -Last 1).Trim()
$pybind11Dir = (Invoke-Native uvx @('--from', "pybind11==$Pybind11Version", 'pybind11-config', '--cmakedir') | Select-Object -Last 1).Trim().Trim('"')
Write-Host "Python   : $python"
Write-Host "pybind11 : $pybind11Dir"

# 2. e4dccommlib (static, Python independent).
Invoke-CMakeProject -Source (Resolve-Path $E4dcCommlibSource) `
                    -Build (Join-Path $buildRoot 'e4dccommlib') `
                    -Prefix $libPrefix

# 3. The wrapper for this Python version.
Invoke-CMakeProject -Source $root `
                    -Build (Join-Path $buildRoot $tag) `
                    -Prefix $OutDir `
                    -Defines @("-DPython_EXECUTABLE=$python", "-Dpybind11_DIR=$pybind11Dir", "-DCMAKE_PREFIX_PATH=$libPrefix")

# 4. Smoke test: the target interpreter must import the module it was built for.
#    okFrontPanel.dll is not shipped: load it from the FrontPanel install
#    (Python does not search the PATH for DLLs).
$frontPanelBin = Join-Path $env:FRONT_PANEL_PATH 'bin\x64'
Invoke-Native $python @('-c', "import os, sys; os.add_dll_directory(r'$frontPanelBin'); sys.path.insert(0, r'$OutDir'); import e4dccommlib_python_wrapper as m; print('import OK:', m.__file__)") | Out-Host
