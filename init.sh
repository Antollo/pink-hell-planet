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

cp -r ./src/shaders ./bin;

echo 'Done!!!';
