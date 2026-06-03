param(
    [string]$Kind = "upper",
    [string]$Version = "v0.0.3",
    [string]$ServerUser = "root",
    [string]$ServerHost = "38.76.214.157",
    [string]$ServerPort = "1564",
    [string]$RemoteStorage = "/var/www/update.hepi.ng/storage",
    [string]$SshKey = "$env:USERPROFILE\.ssh\hepi_deploy_ed25519"
)

$ReleaseDate = Get-Date -Format "yyyy-MM-dd"

python scripts/publish_firmware.py `
    --kind $Kind `
    --version $Version `
    --bin "build-gcc/STM32H743.bin" `
    --hex "build-gcc/STM32H743.hex" `
    --channel beta `
    --set-latest `
    --storage "./storage" `
    --notes `
        "本地发布 $Version" `
        "发布说明：界面与输入控制更新" `
        "适用设备：UPPER上位机" `
        "更新内容：输入框取消上浮动画，统一00.00格式，M_KEY切换设置项，数字颜色与选中位样式更新" `
        "发布日期：$ReleaseDate"

if ($LASTEXITCODE -ne 0) {
    Write-Host "publish_firmware.py 执行失败"
    exit 1
}

ssh -p $ServerPort -i $SshKey "${ServerUser}@${ServerHost}" "mkdir -p ${RemoteStorage}/firmware/${Kind}/versions"

if ($LASTEXITCODE -ne 0) {
    Write-Host "远程目录创建失败"
    exit 1
}

scp -P $ServerPort -i $SshKey -r `
    "./storage/firmware/$Kind/versions/$Version" `
    "${ServerUser}@${ServerHost}:${RemoteStorage}/firmware/${Kind}/versions/"

if ($LASTEXITCODE -ne 0) {
    Write-Host "版本目录上传失败"
    exit 1
}

scp -P $ServerPort -i $SshKey `
    "./storage/firmware/$Kind/latest.json" `
    "./storage/firmware/$Kind/index.json" `
    "${ServerUser}@${ServerHost}:${RemoteStorage}/firmware/${Kind}/"

if ($LASTEXITCODE -ne 0) {
    Write-Host "索引文件上传失败"
    exit 1
}

Write-Host "发布完成：$Kind $Version"
