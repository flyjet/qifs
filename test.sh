mount |grep qifs >/dev/null
if [ $? -eq 0 ]; then
    echo "[i] unmount /tmp/qifs ..."
    sudo umount /tmp/qifs
else
    echo "[i] dir already umount"
fi

lsmod |grep qifs >/dev/null
if [ $? -eq 0 ]; then
    echo "[!] module already exist, remove it ..."
    sudo rmmod qifs
else
    echo "[!] module not exist ..."
fi
echo "[!] insert moduile ... qifs"
sudo insmod qifs.ko
if [ ! -d /tmp/qifs ]; then
    echo "[!] directory /tmp/qifs not exist, create it ... "
    sudo mkdir /tmp/qifs
fi
echo "[!] mount qifs to /tmp/qifs ..."
sudo mount -t qifs /tmp /tmp/qifs
