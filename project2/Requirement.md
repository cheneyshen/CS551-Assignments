CS551 Project 2: Message Ordered Multicast IPCs
Due: November 2, 2016
1.MINIX currently supports kernel IPCs that allows a sender to send a message to a receiver. In this project, we ask you to develop a set of IPC system calls that allow an application process to send messages to other application processes.
2.At the heart of this set of IPC’s is a pair of msend and mreceive system calls. A user process calls msend to send a message to a group of other user processes who call mreceive to retrieve the message. 
3.We require that messages from common senders will be delivered in the same order and in the same interleaving (receiver consistent). You need to describe how do you achieve this in your design document and have test cases to demonstrate it.
4.A receiver is blocked on the call to mreceive if no message has yet been sent to it or if there is, it may violate receiver consistency. You decide whether msend should be blocking. If it is, what is the blocking condition? You must defend your decision.
5.A process can belong to more than one group. (Think: what kind of protection makes sense?)
6.The other system calls allow the programmer to set up and remove a multicast group, and to recover from exception conditions. Call them openGroup, closeGroup and recoverGroup respectively.
7.Your system should be able to support several multicast groups.
8.You will be graded on the performance and easy of use (including under error conditions) of the system calls.
9.Do you need to detect deadlock or livelock? If not, why? If yes, how and what can the user processes do once deadlock or livelock is detected. Is there some way to prevent deadlock or livelock?
10.Also discuss the method that you use for resource management, if any.
Your design document should include the API’s of the system calls, and the API should specify the following:
	The names and parameters of the system calls
	The blocking conditions, if any, for the system calls
	The exception conditions of the system calls
11.You should submit the following:
	A bootable MINIX with your new IPC system calls
	Test programs that show your new IPC system calls meeting their requirements 
	A readme file that describes how to install your enhanced MINIX and how to execute your test programs
	An eight-page (min. 10 point fonts) design document that includes the following: 
	Manual pages for your new system calls
	The design of your system calls
	Possible exceptions and their handling methods
	The reasons behind your decision to make the msend system call non-blocking or blocking.
	Detection and recovery from deadlock if needed. Or why if not needed,
	Resource management, if any.
12.Each team member must submit a self evaluation

mount -t vbfs -o share=CS551 none /mnt

Project 2 Notes
	You need to implement at least 5 system calls:
	msend
	mreceive
	openGroup
	closeGroup
	recoverGroup
Additional system calls can be implemented at your choice
Hint: you need to modify Minix source code
	You also need to implement testcases to test the functionality of your system calls
For 5 and 10 in Project2’s slides:
You can choose either blocking send or non-blocking send. You need to defend your decision in the following steps:
If there can be deadlock or livelock, describe the deadlock/livelock condition in the document. And
(1) Your code should have mechanism to prevent deadlock/livelock from happening, such as Minix client-server design for IPC.
(2) Your code should be able to detect a deadlock/livelock and recover (recoverGroup can be used here) from it.