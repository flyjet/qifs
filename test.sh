sudo rmmod qifs1
sudo insmod qifs1.ko
lsmod |grep qifs
sudo mount -t qifs /tmp /tmp/qifs
