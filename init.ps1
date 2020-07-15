Remove-Item "$env:TEMP\glad.zip" -ErrorAction SilentlyContinue -Force -Confirm:$false
Invoke-WebRequest -Uri 'https://onedrive.live.com/download?cid=F7B2E5ED214ED032&resid=F7B2E5ED214ED032%214910&authkey=ANa6TuYeKnGai4A' -OutFile "$env:TEMP\glad.zip"
Expand-Archive "$env:TEMP\glad.zip" -DestinationPath ".\thirdparty\glad\glad" -Force
Remove-Item "$env:TEMP\glad.zip" -ErrorAction SilentlyContinue -Force -Confirm:$false

New-Item -ItemType directory -Path ".\bin" -ErrorAction SilentlyContinue | Out-Null
New-Item -ItemType directory -Path ".\bin\Release" -ErrorAction SilentlyContinue | Out-Null

Remove-Item "$env:TEMP\penguin.zip" -ErrorAction SilentlyContinue -Force -Confirm:$false
Invoke-WebRequest -Uri "https://onedrive.live.com/download?cid=F7B2E5ED214ED032&resid=F7B2E5ED214ED032%214909&authkey=ADt8ZYAVrLtcSyY" -OutFile "$env:TEMP\penguin.zip"
Expand-Archive "$env:TEMP\penguin.zip" -DestinationPath ".\bin\Release" -Force
Remove-Item "$env:TEMP\penguin.zip" -ErrorAction SilentlyContinue -Force -Confirm:$false

Remove-Item "$env:TEMP\slime.zip" -ErrorAction SilentlyContinue -Force -Confirm:$false
Invoke-WebRequest -Uri "https://onedrive.live.com/download?cid=F7B2E5ED214ED032&resid=F7B2E5ED214ED032%214929&authkey=AMzTuQnU0EVMqOo" -OutFile "$env:TEMP\slime.zip"
Expand-Archive "$env:TEMP\slime.zip" -DestinationPath ".\bin\Release" -Force
Remove-Item "$env:TEMP\slime.zip" -ErrorAction SilentlyContinue -Force -Confirm:$false

Copy-Item -Path ".\src\shaders" -Destination ".\bin\Release" -Force -Recurse
Write-Host "Done"