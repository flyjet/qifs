it is team 7 file system, kernel module part.

This module can work independently, after mounted, it can perform some directory operations, like mkdir, rmdir and file operations like create, open, read, write, close, and delete.

Later, after connection module finished, all operations should go to user space to be implemented.

$ make                       //after that, should have qifs.ko

$ sudo insmod qifs.ko        //insert module

$ lsmod |grep qifs           // to check if module is there

$ mkdir  /tmp/qifs^M$ mount –t  qifs  /tmp  /tmp/qifs    //mount,

$ mount                      //check mount if successful

$ cd /tmp/qifs


$ umount /tmp/qifs           //unmount 
~                                                         
