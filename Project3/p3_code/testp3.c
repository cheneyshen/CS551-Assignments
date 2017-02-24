/* ======================================INCLUDES===================================*/
#include <sys/cdefs.h>
#include <lib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <strings.h>

int get_fileinfo(int *pinode, int *finode)
{
	struct stat statbuf;
	int state;
	char path1[50];
	char file2[50];
	char fullpath[100];
	bzero(path1,50);
	bzero(file2,50);
	bzero(fullpath, 100);
	char *nicetime, *errormessage, *divider;

	printf("\nPlease input the path: ");
	scanf(" %[^\n]%*c",path1);
	printf("\nPlease input the file: ");
	scanf(" %[^\n]%*c",file2);

	strncpy(fullpath, path1, strlen(path1));
	if(path1[strlen(path1)-1]=='/')
		strcat(fullpath, file2);
	else {
		fullpath[strlen(path1)]='/';
		strcat(fullpath, file2);
	}
	
	state = stat(path1, &statbuf);
	if ( state == -1 ){
	    printf("stat path error!\n");
	    exit(1);
	}
	else{
        *pinode = statbuf.st_ino;
        //printf("\n the inode of the file %s is: %d", dirfilename, file_inode);
    }
    state = stat(fullpath, &statbuf);
	if ( state == -1 ){
	    printf("stat file error!\n");
	    exit(1);
	}
	else{
        *finode = statbuf.st_ino;
        //printf("\n the inode of the file %s is: %d", dirfilename, file_inode);
    }
    
	return 0;
}

int damageInode()
{
	char path1[50];
	char path2[100];
	char file[50];
	char fullpath[100];
	char mvstring[150];
	char nodes[10];
	bzero(path1,50);
	bzero(path2, 100);
	bzero(file, 50);
	bzero(fullpath, 100);
	bzero(mvstring, 150);
	bzero(nodes, 10);

	struct stat statbuf;
	int state;
	int srcinode;
	ino_t pinode, finode;
	printf("\nPlease input the path: ");
	scanf(" %[^\n]%*c",path1);
	printf("\nPlease input the file: ");
	scanf(" %[^\n]%*c",file);
	state = stat(path1, &statbuf);
	if ( state == -1 ){
	    printf("stat path error!\n");
	    exit(1);
	}
	else{
        pinode = statbuf.st_ino;
    }
	strncpy(fullpath, path1, strlen(path1));
	if(path1[strlen(path1)-1]=='/')
		strcat(fullpath, file);
	else {
		fullpath[strlen(path1)]='/';
		strcat(fullpath, file);
	}
    state = stat(fullpath, &statbuf);
	if ( state == -1 ){
	    printf("stat file error!\n");
	    exit(1);
	}
	else{
        finode = statbuf.st_ino;
    }
	strcpy(path2, "/tmp/");
	strcat(path2, file);
    strcat(path2, ".");
	sprintf(nodes, "%u", pinode);
    strcat(path2, nodes);
    strcat(path2, ".");
	sprintf(nodes, "%u", finode);
    strcat(path2, nodes);
    //printf("the newname of the file is: %s\n", path2);
    
	message m;
	memset(&m, 0, sizeof(m));
	m.m1_i1 = strlen(fullpath) + 1;
	m.m1_p1 = (char *) __UNCONST(fullpath);
	m.m1_i2 = pinode;
	m.m1_i3 = finode;
	(_syscall(VFS_PROC_NR, 110, &m));
	strcpy(mvstring, "mv ");
	strcat(mvstring, fullpath);
	strcat(mvstring, " ");
	strcat(mvstring, path2);
	//printf("mvstring:%s\n", mvstring);

	system(mvstring);
	return 0;
}

int restoreInode()
{
	char path1[50];
	char file[50];
	bzero(path1,50);
	bzero(file, 50);
	char fullpath[100];
	bzero(fullpath, 100);
	char mvstring[150];
	bzero(mvstring, 150);
	char nodes[10];
	bzero(nodes, 10);

	struct stat statbuf;
	int state;
	int srcinode;
	ino_t pinode, finode;
	printf("\nPlease input the path: ");
	scanf(" %[^\n]%*c",path1);
	printf("\nPlease input the file: ");
	scanf(" %[^\n]%*c",file);
	state = stat(path1, &statbuf);
	if ( state == -1 ){
	    printf("stat path error!\n");
	    exit(1);
	}
	else{
        pinode = statbuf.st_ino;
    }
	strncpy(fullpath, path1, strlen(path1));
	if(path1[strlen(path1)-1]=='/')
		strcat(fullpath, file);
	else {
		fullpath[strlen(path1)]='/';
		strcat(fullpath, file);
	}
	message m;
	memset(&m, 0, sizeof(m));
	m.m1_i1 = strlen(fullpath) + 1;
	m.m1_i2 = pinode;
	m.m1_p1 = (char *) __UNCONST(fullpath);
	printf("%d %d %s\n", m.m1_i1, m.m1_i2, fullpath);
	int r= (_syscall(VFS_PROC_NR, 106, &m));
	if(r>0) {
		//find file name, previous inode is clean and available
		strcpy(mvstring, "mv /tmp/");
		strcat(mvstring, file);
    	strcat(mvstring, ".");
		sprintf(nodes, "%u", pinode);
    	strcat(mvstring, nodes);
    	strcat(mvstring, ".");
		sprintf(nodes, "%u", r);
    	strcat(mvstring, nodes);
    	strcat(mvstring, " ");
    	strcat(mvstring, fullpath);
		printf("mvstring:%s\n", mvstring);
		system(mvstring);
	}
	else if(r==0){
		//already being used, just remove it
		strcpy(mvstring, "rm /tmp/");
		strcat(mvstring, file);
    	strcat(mvstring, ".");
		sprintf(nodes, "%u", pinode);
    	strcat(mvstring, nodes);
    	strcat(mvstring, "*");
		printf("mvstring:%s\n", mvstring);
		system(mvstring);
	}
	return 0;
}

int inodeViewer()
{
	message m;
	memset(&m, 0, sizeof(m));
	int pinode = 0;
	int finode = 0;
	get_fileinfo(&pinode, &finode);
	m.m1_i1 = pinode;
	m.m1_i2 = finode;
	(_syscall(VFS_PROC_NR, 108, &m));
	return 0;
}


int inodeViewer2()
{
	message m;
	memset(&m, 0, sizeof(m));
	int pinode = 0;
	int finode = 0;
	printf("Input node number:");
	scanf("%d", &finode);
	m.m1_i1 = pinode;
	m.m1_i2 = finode;
	(_syscall(VFS_PROC_NR, 108, &m));
	return 0;
}
void viewDirectory(const char *path)
{
	DIR *directory_name;
	struct dirent *start;

	//directory_name = opendir(path);
	if (!(directory_name = opendir(path)))
		return;
	//struct inode *rip;
	while ((start = readdir(directory_name))) {
		/*
			if(start->d_type == DT_DIR){
				char new[1000];
				int total_length = snprintf(new, sizeof(new)-1,"%s/%s",path,start->d_name);
				new[total_length] = 0;
				if(strcmp(start->d_name,".")== 0 || strcmp(start->d_name,"..") == 0)
					continue;
				printf("%s ----Inode number %u \n",start->d_name,start->d_ino);
				recur_func(new);
			}
			else
				{
				printf("	-%s----Inode number %u\n",start->d_name,start->d_ino);
				}
		*/
		if(strcmp(start->d_name,".")== 0 || strcmp(start->d_name,"..") == 0)
			continue;
		printf("%s ----Inode number %u \n",start->d_name,start->d_ino);
	}
	closedir(directory_name);
}

int directoryWalker()
{
	char path[50];
	bzero(path,50);
	printf("Enter the path you wish to open: \n");
	scanf(" %[^\n]%*c", path);
	viewDirectory(path);
	return 0;
}

int rcmkdir()
{
	char path[50];
	bzero(path,50);
	printf("Enter the path you wish to open \n");
	scanf(" %[^\n]%*c",path);

	if(mkdir(path, 0777)) {
		printf("Could not make directory %s. %s\n", path, strerror(errno));
		return 0;
	}
   	else {
		printf("Directory created.\n");
		struct stat statbuf;
		int state = stat(path, &statbuf);
		if(state == -1){
		    printf("stat error!\n");
		    return -1;
		}
		else{
			message m;
			memset(&m, 0, sizeof(m));
			m.m1_i1 = statbuf.st_ino;
			printf("%d \n", m.m1_i1);
			(_syscall(VFS_PROC_NR, 109, &m));
		}
   	}
	return 0;
}

int main(){
	int input=0;

    printf("\n*********************File system tools*********************\n");
	do{
		printf("\nPlease enter \n \
			1 view a file\n \
			2 view a directory\n \
			3 delete a file\n \
			4 undelete a file\n \
			5 rcmkdir \n \
			6 view an inode \n \
			0 exit\n");
		printf(">");
		scanf("%d",&input);
		switch(input){
			case 1:
			inodeViewer();
			break;
			case 2:
			directoryWalker();
			break;
			case 3:
			damageInode();
			break;
			case 4:
			restoreInode();
			break;
			case 5:
			rcmkdir();
			break;
			case 6:
			inodeViewer2();
			case 0:
			exit(0);
			break;
			default:
			break;
		}
	}while(input !=0);
}