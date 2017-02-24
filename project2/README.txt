login as root. current directory /root.

unzip project2.zip
cd /root/project2/

cp callnr.h /usr/src/include/minix/
cp table.c /usr/src/servers/pm/
cp proto.h /usr/src/servers/pm/
cp misc.c /usr/src/servers/pm/
cp grpmsgdef.c /usr/src/servers/pm/
cp grpmsgdef.h /usr/src/servers/pm/
cp main.c /usr/src/servers/pm/

cd /usr/src/servers/pm/
make

Open makefile on the path /usr/src/servers/pm/Makefile
Insert “grpmsgdef.c” at the end of 5th line declaring SRCS.

cd /usr/src/servers/pm/

to compile the definitions, type: make

After compile :

cd /root/project2/

cp project2.h /usr/include/
cd /usr/src/releasetools
make install
sync


*NOTE : You can also execute prep.bash to run the above steps (Except manually inserting the SRC in pm Makefile)
reboot

After reboot is done :

==============================
Shell testcases
==============================

cd /root/project2/
cc project.c -o project
sh tests.sh
