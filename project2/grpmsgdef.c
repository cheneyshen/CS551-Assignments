#include "grpmsgdef.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG_MODE 1

void groups_init()
{
    memset(GroupsList, 0, sizeof(GroupsList));

	for(int i=0; i<MAX_GROUPS; i++) {
		GroupsList[i].openFlag=0;
	}
	numGroups = 0;
}

MessagingErrorCodes lookup_groups(char group_names[])
{
    memset(group_names, 0, sizeof(group_names));

	for(int i = 0; i < MAX_GROUPS && GroupsList[i].openFlag == 1; i++)
	{
	    strcat(group_names, GroupsList[i].group_name);
        strcat(group_names, ";");
	}

    strcat(group_names, "\0");
	return MESSAGING_SUCCESS;  //0
}


MessagingErrorCodes opengroup(char group_name[])
{
	if(group_name == NULL || strlen(group_name)== 0)
		return INP_ERR_NULL_GROUP_NAME;

	if(strlen(group_name) > MAX_GROUP_NAME_LEN)
		return GROUP_NAME_LEN_OVERFLOW;

	/*Check if group_name already exists */
	for(int i = 0; i < MAX_GROUPS; i++)
	{
		if(strncmp(GroupsList[i].group_name, group_name, strlen(group_name)) == 0)
			return DUPLICATE_GROUP;
	}

	int emptySlot = findEmptyGroupSlot();
	if(emptySlot == -1)
		return GROUPS_FULL;

	strcpy(GroupsList[emptySlot].group_name, group_name);
	GroupsList[emptySlot].senders.curr_num = 0;
	GroupsList[emptySlot].receivers.curr_num = 0;
	GroupsList[emptySlot].iList.curr_num = 0;
	//Initialize all 50 items of all messages to deleted
	for(int i = 0; i < 50; i++)
		GroupsList[emptySlot].iList.itemsList[i].deletedFlag = 1;

	GroupsList[emptySlot].openFlag = 1; //running
#if DEBUG_MODE
	//printf("EmptySlot:%d %d\n", emptySlot, GroupsList[emptySlot].openFlag);
#endif
	numGroups++;
	return MESSAGING_SUCCESS;  //0
}

MessagingErrorCodes closegroup(char group_name[])
{
	if(group_name == NULL || strlen(group_name)== 0)
		return INP_ERR_NULL_GROUP_NAME;

	if(strlen(group_name) > MAX_GROUP_NAME_LEN)
		return GROUP_NAME_LEN_OVERFLOW;

	/*Check if group_name already exists */
	int i;
	for(i = 0; i < MAX_GROUPS; i++)
	{
		if(strncmp(GroupsList[i].group_name, group_name, strlen(group_name)) == 0)
			break;
	}

	if(MAX_GROUPS == i)
		return GROUP_NOT_FOUND;
	/*
	memset(GroupsList[i].group_name, 0, sizeof(GroupsList[i].group_name));
	GroupsList[i].senders.curr_num = 0;
	GroupsList[i].receivers.curr_num = 0;
	GroupsList[i].iList.curr_num = 0;
	//Initialize all 50 items of all messages to deleted
	for(int j = 0; j < 50; i++)
		GroupsList[i].iList.itemsList[j].deletedFlag = 1;
	*/
	GroupsList[i].openFlag = 0; //block
	numGroups--;
	return MESSAGING_SUCCESS;  //0
}

MessagingErrorCodes recovergroup(char group_name[])
{
	if(group_name == NULL || strlen(group_name)== 0)
		return INP_ERR_NULL_GROUP_NAME;

	if(strlen(group_name) > MAX_GROUP_NAME_LEN)
		return GROUP_NAME_LEN_OVERFLOW;

	/*Check if group_name already exists */
	int i;
	for(i = 0; i < MAX_GROUPS; i++)
	{
		if(strncmp(GroupsList[i].group_name, group_name, strlen(group_name)) == 0)
			break;
	}
	if(MAX_GROUPS == i)
		return GROUP_NOT_FOUND;

	GroupsList[i].iList.curr_num = 0;
	//Initialize all 50 items of all messages to deleted
	for(int j = 0; j < 50; j++) {
		GroupsList[i].iList.itemsList[j].deletedFlag = 1;
	}

	GroupsList[i].openFlag = 1;	//running
	numGroups++;
	return MESSAGING_SUCCESS;  //0
}

int getSenderId(int groupId, int senderId)
{
	int foundFlag = 0;
	int i;
	for(i = 0; i < GroupsList[groupId].senders.curr_num; i++)
	{
		if(GroupsList[groupId].senders.senderList[i] == senderId)
		{	
			foundFlag = 1;
			break;
		}

	}
	if(!foundFlag)
		return -1;
	return i;
}

int getReceiverId(int groupId, int receiverId)
{
	int foundFlag = 0;
	int i;
	for(i = 0; i < GroupsList[groupId].receivers.curr_num; i++)
	{
		if(GroupsList[groupId].receivers.receiverList[i] == receiverId)
		{	
			foundFlag = 1;
			break;
		}

	}
	if(!foundFlag)
		return -1;
	return i;
}

int findEmptyGroupSlot() {
	for(int i=0; i<MAX_GROUPS; i++) {
		if(GroupsList[i].openFlag == 0)
			return i;
	}
	return -1;
}

int findEmptyItemSlot(int groupId)
{
	int foundFlag = 0;
	int i;
	for(i = 0; i < 50; i++)
	{
		if(GroupsList[groupId].iList.itemsList[i].deletedFlag == 1)
		{
			foundFlag = 1;
			break;
		}
	}
	if(!foundFlag)
		return -1;
	return i;
}

MessagingErrorCodes add_sender_to_group(char group_name[], int senderId)
{
	int sender_ind;
	int foundFlag = 0;
	int i;
	if(group_name == NULL || strlen(group_name)== 0)
		return INP_ERR_NULL_GROUP_NAME;
	//printf("senderId :%d group_name:%s\n ", senderId, group_name);
	for(i = 0; i<numGroups; i++)
	{
		if(GroupsList[i].openFlag ==1 && strncmp(GroupsList[i].group_name, group_name, strlen(group_name)) == 0)
		{
			foundFlag = 1;
			break;
		}
	}
	if(!foundFlag)
	{
		return GROUP_NOT_FOUND;
	}
#if DEBUG_MODE
	//printf("Group Index:%d\n", i);
#endif

	/* Group name matched at index i. Now add sender */
	/* Check if duplicate before adding */
	sender_ind = getSenderId(i, senderId); 
	if(sender_ind != -1)
		return DUPLICATE_SENDER;

	//Adding sender
	GroupsList[i].senders.senderList[GroupsList[i].senders.curr_num] = senderId;
#if DEBUG_MODE
	//printf("Sender Index:%d\n", GroupsList[i].senders.curr_num);
#endif
	GroupsList[i].senders.curr_num++;
#if DEBUG_MODE
	//printf("Sender Index:%d\n", GroupsList[i].senders.curr_num);
#endif
	return MESSAGING_SUCCESS; //0
}

MessagingErrorCodes add_receiver_to_group(char group_name[], int receiverId)
{
	if(group_name == NULL)
		return INP_ERR_NULL_GROUP_NAME;
			
	int foundFlag = 0;
	int i;
	//printf("receiverId :%d group_name:%s\n ", receiverId, group_name);
	for(i = 0; i <numGroups; i++)
	{
		if(GroupsList[i].openFlag ==1 && strncmp(GroupsList[i].group_name, group_name, strlen(group_name)) == 0)
		{
			foundFlag = 1;
			break;
		}
	}
	if(!foundFlag)
	{
		return GROUP_NOT_FOUND;
	}

	/* Group name matched at index i. Now add publisher */
	/* Check if duplicate before adding */
	int receiver_ind = getReceiverId(i, receiverId); 
	if(receiver_ind != -1)
		return DUPLICATE_RECEIVER;

	//Adding receiver
	GroupsList[i].receivers.receiverList[GroupsList[i].receivers.curr_num] = receiverId;
	GroupsList[i].receivers.curr_num++;
	return MESSAGING_SUCCESS; //0
}

MessagingErrorCodes sendmsg(char group_name[], char mesg[])
{
	if(group_name == NULL || strlen(group_name)== 0)
		return INP_ERR_NULL_GROUP_NAME;

	if(strlen(mesg) > MAX_MESG_LEN)
		return MSG_LEN_OVERFLOW_ERROR;

	int foundFlag = 0;
	int i;
	for(i = 0; i<numGroups; i++)
	{
		if(GroupsList[i].openFlag ==1 && strncmp(GroupsList[i].group_name, group_name, strlen(group_name)) == 0)
		{
			foundFlag = 1;
			break;
		}
	}
	if(!foundFlag)
		return GROUP_NOT_FOUND;

	/* Group name matched at index i. Now add mesg */
#if DEBUG_MODE
	//printf("Group Index:%d\n", i);
#endif
	int emptySlot = findEmptyItemSlot(i);
#if DEBUG_MODE
	//printf("EmptySlot:%d\n", emptySlot);
#endif
	if(emptySlot == -1)
		return BUFFER_OVERFLOW;

	GroupsList[i].iList.itemsList[emptySlot].num_of_read_complete = 0;
	GroupsList[i].iList.itemsList[emptySlot].deletedFlag = 0;
	strcpy(GroupsList[i].iList.itemsList[emptySlot].Mesg, mesg);
    
	return MESSAGING_SUCCESS; //0
}

MessagingErrorCodes receivemsg(char group_name[], char **mesg, int receiverId)
{
    if(group_name == NULL || strlen(group_name)== 0)
		return INP_ERR_NULL_GROUP_NAME;

	int foundFlag = 0;
	int i;
	for(i = 0; i<numGroups; i++)
	{
		if(GroupsList[i].openFlag ==1 && strncmp(GroupsList[i].group_name, group_name, strlen(group_name)) == 0)
		{
			foundFlag = 1;
			break;
		}
	}
	if(!foundFlag)
	{
		return GROUP_NOT_FOUND;
	}
#if DEBUG_MODE
	//printf("Group Index:%d\n", i);
#endif
	int group_index = i;
	/* Group name matched at index i */
	/* check if the input receiver is a valid receiver */
	int found_flag = 0;
	for(int i = 0; i < GroupsList[group_index].receivers.curr_num; i++) 
	{
		if(GroupsList[group_index].receivers.receiverList[i] == receiverId)
		{
			found_flag = 1;
			break;
		}
	}

	if(!found_flag)
		return NOT_RECEIVER_OF_GROUP;

#if DEBUG_MODE
	//printf("Valid Receiver\n");
#endif
	/* Receiver is valid one */
	/* Parse through the item list and return the item which receiver has not yet read */
	int item_present_to_read=0;
	for(int i = 0; i < 50 ; i++)
	{
		if(!GroupsList[group_index].iList.itemsList[i].deletedFlag)
		{
			int isReadFlag = 0;
			for(int j = 0; j < GroupsList[group_index].iList.itemsList[i].num_of_read_complete; j++)
			{
				if(GroupsList[group_index].iList.itemsList[i].ReceiverList[j] == receiverId)
				{
					isReadFlag = 1;
					break; /* There is a match. Thus this mesg is already read by receiver. So move to next item */
				}
			}
#if DEBUG_MODE
	//printf("isReadFlag:%d i:%d \n", isReadFlag, i);
#endif
			if(!isReadFlag)
			{
				/* ith item is not read by receiver. So, read this item */
				char temp[MAX_MESG_LEN];
				item_present_to_read = 1;
				strcpy(temp, GroupsList[group_index].iList.itemsList[i].Mesg);
				strcpy(*mesg, temp);
				GroupsList[group_index].iList.itemsList[i].ReceiverList[GroupsList[group_index].iList.itemsList[i].num_of_read_complete++] = receiverId;
				/* Check if message is read by all receivers. If so delete the message */
				if(GroupsList[group_index].iList.itemsList[i].num_of_read_complete == GroupsList[group_index].receivers.curr_num)
					GroupsList[group_index].iList.itemsList[i].deletedFlag = 1;
				break;
			}
		}
	}
	if(!item_present_to_read) {
		return BUFFER_UNDERFLOW;
	}
	
	return MESSAGING_SUCCESS; //0
}

void print_buffer_snapshot()
{
	printf("------------------------------------------------------\n");
	printf("Group List:\n");
	for(int i = 0; i < numGroups; i++)
	{
		printf("Group %d:%s\n", i, GroupsList[i].group_name); 
		printf("Senders List:%d  ", GroupsList[i].senders.curr_num);
		for(int j = 0; j < GroupsList[i].senders.curr_num; j++)
		{
			printf("%d,", GroupsList[i].senders.senderList[j]);
		}
		printf("\n");
		printf("Receivers List:");
		for(int j = 0; j < GroupsList[i].receivers.curr_num; j++)
		{
			printf("%d,", GroupsList[i].receivers.receiverList[j]);
		}
		printf("\n");
		printf("Items List:\n");
		for(int j = 0; j < 50; j++)
		{
			if(!GroupsList[i].iList.itemsList[j].deletedFlag)
			{
				printf("Item %d:%s:\n", j,  GroupsList[i].iList.itemsList[j].Mesg);
				printf("Deleted Flag:%d   ", GroupsList[i].iList.itemsList[j].deletedFlag);
				printf("Num_read_receivers:%d   ", GroupsList[i].iList.itemsList[j].num_of_read_complete);
				for(int k = 0; k < GroupsList[i].iList.itemsList[j].num_of_read_complete; k++)
				{
					printf("%d,",GroupsList[i].iList.itemsList[j].ReceiverList[k]);
				}
			}
	        printf("\n");
		}
	    printf("------------\n");
	}
	printf("------------------------------------------------------\n");
}

void print_group_snapshot(int group_index)
{
	printf("------------------------------------------------------\n");
	int i=group_index;
	printf("GROUP %d:%s\n", i, GroupsList[i].group_name); 
	printf("Senders List:%d  ", GroupsList[i].senders.curr_num);
	for(int j = 0; j < GroupsList[i].senders.curr_num; j++)
	{
		printf("%d,", GroupsList[i].senders.senderList[j]);
	}
	printf("\n");
	printf("Receivers List:");
	for(int j = 0; j < GroupsList[i].receivers.curr_num; j++)
	{
		printf("%d,", GroupsList[i].receivers.receiverList[j]);
	}
	printf("\n");
	printf("Items List:\n");
	for(int j = 0; j < 50; j++)
	{
		if(!GroupsList[i].iList.itemsList[j].deletedFlag)
		{
			printf("Item %d:%s:\n", j,  GroupsList[i].iList.itemsList[j].Mesg);
			printf("Deleted Flag:%d   ", GroupsList[i].iList.itemsList[j].deletedFlag);
			printf("Num_read_receivers:%d   ", GroupsList[i].iList.itemsList[j].num_of_read_complete);
			for(int k = 0; k < GroupsList[i].iList.itemsList[j].num_of_read_complete; k++)
			{
				printf("%d,",GroupsList[i].iList.itemsList[j].ReceiverList[k]);
			}
		}
        printf("\n");
	}
    printf("------------\n");
}
