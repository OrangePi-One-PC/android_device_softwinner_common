#!/sbin/busybox sh

BUSYBOX="/sbin/busybox"

$BUSYBOX echo "do data resume..."
#$BUSYBOX mv /cache/data /data/
#$BUSYBOX cp -a /cache/property /data/
$BUSYBOX cp -a /cache/data/system /data/
$BUSYBOX cp -a /cache/data/misc /data/
$BUSYBOX cp -a /cache/data/property /data/

$BUSYBOX chown 1000:1000 /data/system /data/system/pap-secrets /data/system/chap-secrets
$BUSYBOX chmod 0775 /data/system
$BUSYBOX chmod 0600 /data/system/pap-secrets /data/system/chap-secrets
$BUSYBOX chown 1000:9998 /data/misc
$BUSYBOX chmod 01771 /data/misc
$BUSYBOX chown 1010:1010 /data/misc/wifi
$BUSYBOX chmod 0770 /data/misc/wifi
$BUSYBOX chown 1000:1010 /data/misc/wifi/wpa_supplicant.conf
$BUSYBOX chmod 0660 /data/misc/wifi/wpa_supplicant.conf
#$BUSYBOX chown 0:0 /data/property
$BUSYBOX chmod 0700 /data/property
#$BUSYBOX chown 0:0 /data/property/persist.sys.wifistate
$BUSYBOX chmod 0600 /data/property/persist.sys.wifistate

$BUSYBOX rm -rf /cache/data

$BUSYBOX echo "data resume finish"
exit 0
