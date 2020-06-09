param(
    [string]$bochs = $(throw "Parameter missing: -bochs BochsPath")
)

$osName = "oranges"

    
Set-Location -Path ../

#---------------------测试参数合法性---------------------
$bochs = "$bochs".TrimEnd("\")

if (-not(Test-Path -Path "$bochs" -PathType Container)) {
    Write-Error "BochsPath $bochs not exists!"
    Exit 1 
}

#---------------------读取bochs.src 生成正确的配置文件---------------------
Write-Output "Reading bochs.src......"
$bochsSrcFile = Get-Content -Path "./.prebuilt/bochs.src"
if (-not $bochsSrcFile) { 
    Write-Error "Get BochsSourceFile (bochs.src) fail."
    Exit 1
}
Write-Output "Generating $osName.src ......"
Out-File -InputObject $bochsSrcFile.Replace("{BochsImage}", "$osName.img").Replace("{BochsPath}", $bochs) -FilePath "./.out/$osName.bxrc" -Encoding "ASCII"



#---------------------创建软盘镜像文件---------------------
make
if ($LASTEXITCODE -ne 0) {
    Write-Error "Make fail."
    Exit $LASTEXITCODE
}

Write-Output "All complete."