#!/bin/bash

set -e;

mkdir -p ./thirdparty/glad/glad;
mkdir -p ./bin;

function download_zip {
    wget -qO- $1 | busybox unzip -od $2 -;
}

download_zip 'https://onedrive.live.com/download?cid=F7B2E5ED214ED032&resid=F7B2E5ED214ED032%214910&authkey=ANa6TuYeKnGai4A' ./thirdparty/glad/glad
download_zip 'https://onedrive.live.com/download?cid=F7B2E5ED214ED032&resid=F7B2E5ED214ED032%214909&authkey=ADt8ZYAVrLtcSyY' ./bin
download_zip 'https://onedrive.live.com/download?cid=F7B2E5ED214ED032&resid=F7B2E5ED214ED032%214929&authkey=AMzTuQnU0EVMqOo' ./bin
download_zip 'https://onedrive.live.com/download?cid=F36537D7A4297BE4&resid=F36537D7A4297BE4%21835&authkey=AIFCo9mbj_zDv6E' ./bin 

cp -r ./src/shaders ./bin;
cp ./src/config/config.default.json ./bin/config.json

echo 'Done!!!';
