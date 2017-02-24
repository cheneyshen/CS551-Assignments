#include <lib.h>
#include <unistd.h>

#define MAX_GNAME_SIZE 255
#define MAX_MESSAGE_SIZE 1024
#define MAX_GLIST_SIZE 255


int sys_ginit(void)
{
    
    message m;
    return ( _syscall(PM_PROC_NR, INITGROUP, &m) );
    
}

int sys_glookup(char *groups)
{
    message m;
    
    m.m1_p1 = groups;
    
    return ( _syscall(PM_PROC_NR, LOOKUPGROUP, &m) );
}

int sys_gopen(char *group_name)
{
    message m;

    m.m1_p1 = group_name;
    
    return ( _syscall(PM_PROC_NR, OPENGROUP, &m) );
}

int sys_gclose(char *group_name)
{
    message m;

    m.m1_p1 = group_name;
    
    return ( _syscall(PM_PROC_NR, CLOSEGROUP, &m) );
}

int sys_gsender(int senderId, char *group_name)
{
    message m;
    
    m.m1_i1 = senderId;
    m.m1_p1 = group_name;

    return ( _syscall(PM_PROC_NR, GROUPSENDER, &m) );
}

int sys_greceiver(int receiverId, char *group_name)
{
    message m;
    
    m.m1_i1 = receiverId;
    m.m1_p1 = group_name;
    
    return ( _syscall(PM_PROC_NR, GROUPRECEIVER, &m) );
}

int sys_msend(int senderId, char *group_name, char *mesg)
{
    message m;
    
    m.m1_i1 = senderId;
    m.m1_p1 = group_name;
    m.m1_p2 = mesg;

    return ( _syscall(PM_PROC_NR, MSGSEND, &m) );
}

int sys_mreceive(char *group_name, char *mesg, int receiverId)
{
    message m;
    
    m.m1_i1 = receiverId;
    m.m1_p1 = group_name;
    m.m1_p2 = mesg;
    
    int sta = _syscall(PM_PROC_NR, MSGRECEIVE, &m);
    return sta;
}

int sys_grecover(char *group_name)
{
    message m;

    m.m1_p1 = group_name;
    
    return ( _syscall(PM_PROC_NR, RECOVERGROUP, &m) );

}