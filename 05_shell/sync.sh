#!/bin/bash
path=~/work/createw/tmpCode
find $path -d 1 -type d | xargs -I {} rm -r {}
arr=(
    "../winGrationHAL/libIdcardModule_CN"
    "../winGrationHAL/common"
)
for v in ${arr[@]}; do
    echo cp -r $v .
    cp -r $v .
done
echo done!
