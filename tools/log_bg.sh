TIMESTAMP=`date +%Y%m%d%H%M%S`

declare -i size=0
for  i in `ls /sdcard/*.log`
do
    let "size+=1"
done

let "size = size/2"

logcat -v time > /sdcard/aw_${size}_system_boot.$TIMESTAMP.log &
cat /proc/kmsg > /sdcard/aw_${size}_kernel_boot.$TIMESTAMP.log &
wait

