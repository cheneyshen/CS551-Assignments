#define MAX_GROUPS 10
#define MAX_RECEIVERS 10
#define MAX_SENDERS 10
#define MAX_GROUP_NAME_LEN 100
#define MAX_MESG_LEN 100
#define MAX_GROUPS_SIZE 1200

typedef int MessagingErrorCodes;

#define	MESSAGING_SUCCESS 0
#define	INP_ERR_NULL_GROUP_NAME 1
#define	GROUP_NAME_LEN_OVERFLOW 2
#define	GROUP_NOT_FOUND 3
#define	DUPLICATE_SENDER 4
#define	DUPLICATE_RECEIVER 5
#define	MSG_LEN_OVERFLOW_ERROR 6
#define	BUFFER_OVERFLOW 7
#define	NOT_RECEIVER_OF_GROUP 8
#define	BUFFER_UNDERFLOW 9
#define	DUPLICATE_GROUP 10
#define	GROUPS_FULL 11
#define	GROUP_CLOSED 12


struct SenderList
{
	int senderList[MAX_SENDERS];
	int curr_num; /* current number of senders */
};

struct ReceiverList
{
	int receiverList[MAX_RECEIVERS]; 
	int curr_num; /* current number of receivers */
};

struct Item //Represents each message
{
	char Mesg[MAX_MESG_LEN];
	int ReceiverList[MAX_RECEIVERS]; //List of receivers who have already read the message
	int num_of_read_complete;
	int deletedFlag; // If 1 it means that the message has been deleted
};

struct ItemsList
{
	struct Item itemsList[50]; //Hardcoded to 50 
	int curr_num;
};

struct MsgListPerGroup //List of 50 messages per group
{
	char group_name[MAX_GROUP_NAME_LEN + 1];
	struct SenderList senders;
	struct ReceiverList receivers;
	struct ItemsList iList; //Hardcoded to 50
	int openFlag; //If 1 it means the group is open
};

struct MsgListPerGroup GroupsList[MAX_GROUPS];
int numGroups;

/* Utility functions */
void groups_init();
int getSenderId(int groupId, int senderId);
int getReceiverId(int groupId, int receiverId);
int findEmptyGroupSlot();
int findEmptyItemSlot(int groupId);
int get_next_mesg_from_group(int groupId);
void print_buffer_snapshot();
void print_group_snapshot(int groupId);
/* End of Utility functions */

MessagingErrorCodes opengroup(char group_name[]);
MessagingErrorCodes closegroup(char group_name[]);
MessagingErrorCodes recovergroup(char group_name[]);
MessagingErrorCodes add_sender_to_group(char group_name[], int senderId);
MessagingErrorCodes add_receiver_to_group(char group_name[], int receiverId);
MessagingErrorCodes sendmsg(char group_name[], char mesg[]);
MessagingErrorCodes receivemsg(char group_name[], char **mesg, int receiverId); 
MessagingErrorCodes lookup_groups(char *groups);
