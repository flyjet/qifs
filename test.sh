sudo rmmod qifs
sudo insmod qifs.ko
lsmod |grep qifs
sudo mount -t qifs /tmp /tmp/qifs
