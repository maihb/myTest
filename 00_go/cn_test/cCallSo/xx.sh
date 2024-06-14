#!/usr/bin/zsh
curl -v --location 'http://127.0.0.1:7777/inteGration/hardwareControl/api/fprCapture' \
--header 'Content-Type: application/json; charset=UTF-8' \
--data '{
    "useType": 1,
    "leftRight":1,
    "position":14,
    "timeout":6,
    "historyFeature":"dfsdfa"
}
'
