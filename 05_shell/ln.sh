path=~/work/createw/tmpCode
ls | grep -v '.sh$' | xargs -I {} rm {}
arr=(
    "../winGrationHAL/libIdcardModule_CN"
    "../winGrationHAL/common"
)
for v in ${arr[@]}; do
    ln -s $v .
done
echo done!
