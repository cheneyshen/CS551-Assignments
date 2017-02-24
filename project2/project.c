#include <project2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_GROUPS 10
#define MAX_RECEIVERS 10
#define MAX_SENDERS 10
#define MAX_GROUP_NAME_LEN 100
#define MAX_MESG_LEN 100
#define MAX_GROUPS_SIZE 1200

typedef int MessagingErrorCodes;

char *return_codes[]={
    "MESSAGING_SUCCESS",
    "INP_ERR_NULL_GROUP_NAME",
    "GROUP_NAME_LEN_OVERFLOW",
    "GROUP_NOT_FOUND",
    "DUPLICATE_SENDER",
    "DUPLICATE_RECEIVER",
    "MSG_LEN_OVERFLOW_ERROR",
    "BUFFER_OVERFLOW",
    "NOT_RECEIVER_OF_GROUP",
    "BUFFER_UNDERFLOW",
    "DUPLICATE_GROUP",
    "GROUPS_FULL",
    "GROUP_CLOSED"
};

void printUsage(void)
{
    printf("Usage: \t\t\targ[1]=Type of command[\n\t\t\t\t1=opengroup, \n\t\t\t\t2=closegroup, \n\t\t\t\t3=gsender, \n\t\t\t\t4=greceiver, \n\t\t\t\t5=msend, \n\t\t\t\t6=mreceive, \n\t\t\t\t7=recoergroup]\n\t\t\targ[2]=Group name\n\t\t\targ[3]=Process ID\n\t\t\targ[4]=Message content\n");
    printf("Retrieving groups\n");
    int status;
    char all_groups[MAX_GROUPS_SIZE];
    status = sys_glookup(all_groups);
    
    printf("Retrieved groups : \n");
    char *token = strtok(all_groups, ";");
    while(token) {
        printf("%s\n", token);
        token = strtok(NULL, ";");
    }
 
    if(status){
        printf("Failed\n");
    }
}

int main(int argc, char *argv[], char *envp[])
{
    int proctype = 1;
    int procid = 1;
    int status;
    int choice;
    char msg1[50*MAX_MESG_LEN];
    char grpnam[MAX_GROUP_NAME_LEN];
    if (argc>=3)
    {
        if ((atoi(argv[1])>0))
        {
            proctype = atoi(argv[1]);
            memset(grpnam, 0, sizeof(grpnam));
            strncpy(grpnam, argv[2], MAX_GROUP_NAME_LEN);
            if (argc>=4)
                procid = atoi(argv[3]);
            if (argc>=5) {
                memset(msg1, 0, sizeof(msg1));
                strncpy(msg1, argv[4], MAX_MESG_LEN);
            }
        }
        else
        {
            printUsage();
            return -1;
        }
    }
    else
    {
        printUsage();
        return -1;
    }
    
    if((proctype<1)||(proctype>7))
    {
        printf("\nEnter choice[\n\t\t\t\t1=opengroup, \n\t\t\t\t2=closegroup, \n\t\t\t\t3=gsender, \n\t\t\t\t4=greceiver, \n\t\t\t\t5=msend, \n\t\t\t\t6=mreceive, \n\t\t\t\t7=recoergroup]");
        scanf("%d",&choice);
    }
    else
    {
        choice = proctype;
    }
    if(choice==1)
    {
        //open group
        printf("Asking to open group : %s\n", grpnam);
        status=sys_gopen(grpnam);
        printf("Status : %s\n", return_codes[status]);
    }
    else if(choice==2)
    {
        //close group
        printf("Asking to close group : %s\n", grpnam);
        status=sys_gclose(grpnam);/Users/spring/Google 云端硬盘/CS551/project22/project2.h
        printf("Status : %s\n", return_codes[status]);
    }
    else if(choice==3)
    {
        //as sender
        char* pch;
        printf("Asking to become a sender of groups : %s\n", grpnam);
        pch = strtok(grpnam, ",");
        while(pch != NULL)
        {
            printf("grpnam:%s\n", pch);
            status = sys_gsender(procid, pch);
            printf("Status : %s\n", return_codes[status]);
            if(3==status) {
                break; //not found
            }
            pch = strtok(NULL, ",");
        }
    }
    else if(choice==4)
    {
        //as receiver
        char* pch;
        printf("Asking to become a receiver of groups : %s\n", grpnam);
        pch = strtok(grpnam, ",");
        while(pch != NULL)
        {
            status = sys_greceiver(procid, grpnam);
            printf("Status : %s\n", return_codes[status]);
            if(3==status) {
                break; //not found
            }
            pch = strtok(NULL, ",");
        }
    }
    else if(choice==5)
    {
        //send msg
        printf("Publishing a message : %s to group : %s\n", msg1, grpnam);
        status = sys_msend(getpid(), grpnam, msg1);
        printf("Status : %s\n", return_codes[status]);
        if(3==status) {
            return 0;//not found
        }
    }
    else if(choice==6)
    {
        //receive msg
        printf("Retrieving the message from group : %s\n", grpnam);
        status = sys_mreceive(grpnam, msg1, procid);
        printf("Status : %s\n", return_codes[status]);
       
        if(status == 0){
            printf("Message received : %s \n", msg1);
            
            while(!status) {
                status = sys_mreceive(grpnam, msg1, procid);
                if(status == 0){
                    printf("Message received : %s \n", msg1);
                }
            }
        }
        else{
            while(status) {
                sleep(1);
                //printf("Blocking Retrieving the message from group : %s\n", grpnam);
                status = sys_mreceive(grpnam, msg1, procid);
                printf("Status : %s %d\n", return_codes[status], status);
                if(0==status) {
                    printf("Message received : %s \n", msg1);
                }
                else if(3==status) {
                    break;//not found
                }
                while(!status) {
                    status = sys_mreceive(grpnam, msg1, procid);
                    if(0==status) {
                        printf("Message received : %s \n", msg1);
                    }
                }

            }
        }
    }
    else if(choice==7)
    {
        //recover group
        printf("Asking to recover a group : %s\n", grpnam);
        status = sys_grecover(grpnam);
        printf("Status : %s\n", return_codes[status]);
    }
    return 0;
}
