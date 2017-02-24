To implentment, please follow the commands below:

mount -t vbfs -o share=Your_Share_Folder none /mnt

cd /mnt/p3_code

cp include/minix/vfsif.h /usr/src/include/minix/vfsif.h
cp include/minix/vfsif.h /usr/include/minix/vfsif.h

cp servers/mfs/Makefile /usr/src/servers/mfs/Makefile
cp servers/mfs/table.c /usr/src/servers/mfs/table.c
cp servers/mfs/proto.h /usr/src/servers/mfs/proto.h
cp servers/mfs/new.c /usr/src/servers/mfs/new.c

cp servers/vfs/Makefile /usr/src/servers/vfs/Makefile
cp servers/vfs/table.c /usr/src/servers/vfs/table.c
cp servers/vfs/proto.h /usr/src/servers/vfs/proto.h
cp servers/vfs/new.c /usr/src/servers/vfs/new.c
cp servers/vfs/request.c /usr/src/servers/vfs/request.c

cd /usr/src/servers/vfs
make
cd /usr/src/servers/mfs
make
cd /usr/src/releasetools
make install
sync
reboot
