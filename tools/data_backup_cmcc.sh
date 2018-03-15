#!/sbin/busybox sh

BUSYBOX="/sbin/busybox"

$BUSYBOX echo "do data backup..."


#PPPOE
$BUSYBOX mkdir -p /cache/data/system
$BUSYBOX mkdir -p /cache/data/misc/wifi
$BUSYBOX mkdir -p /cache/data/property

$BUSYBOX cp -p /data/system/pap-secrets /data/system/chap-secrets  /cache/data/system/
$BUSYBOX cp -p /data/misc/wifi/wpa_supplicant.conf /cache/data/misc/wifi
$BUSYBOX cp -p /data/property/persist.sys.wifistate /cache/data/property/

$BUSYBOX echo "data backup finish"
exit 0
