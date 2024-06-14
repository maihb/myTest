#替换设备 id 脚本， 涉及到改配置的，需要增加到更新包的 sys-patchup 目录
old=$(grep devId /home/grg/inteGrationHALService/config.json | tail -1)
str='s/^.*"devId": "abc",/'$old'/g'
sed -i "$str" ../config.json
