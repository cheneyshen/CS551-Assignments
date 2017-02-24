#include "fs.h"
#include <sys/types.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <minix/ipc.h>
#include "const.h"
#include "inode.h"
#include "type.h"
#include "mfsdir.h"
#include "super.h"
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include <errno.h>
#include "buf.h"
#include <sys/stat.h>
#include <dirent.h>

#define SAME 1000

struct inode *rip;
struct super_block *sp;
unsigned int blk_size;
unsigned int nb_inodes;
static int cursor1=0;
static int cursor2=0;
static long rcdirs[100];
struct rcentry{
    long    pinode;  
    long    finode;
    char  	fname[100];
};
static struct rcentry rcnodes[100];
/*===========================================================================*
 *				fs_inodeviewer			     *
 *===========================================================================*/
int fs_inodeviewer()
{
	int inode_nr, i=0;
	block_t b;
	inode_nr = fs_m_in.m1_i1;
	printf("mfs:fs_inodeviewer:\n");
	rip=NULL;

	printf("Printing details of iNode %d\n", inode_nr);

	rip =  get_inode(fs_dev, inode_nr);
	b = (block_t)rip->i_count;
	if(b==0){
		printf("file already removed\n");
		//return 0;
	}
	printf("\n Mode = %d",rip->i_mode);
	printf("\t nLinks = %d", rip->i_nlinks);
	printf("\t iUid = %d", rip->i_uid);
	printf("\t iGid = %d",rip->i_gid);
	printf("\n Size = %d",rip->i_size);
	printf("\t aTime = %d",rip->i_atime);
	printf("\t mTime = %d",rip->i_mtime);
	printf("\t cTime = %d",rip->i_ctime);
	printf("\n ZonesBlocks are \n");
	for (i = 0; i < V2_NR_TZONES; i++){
	b = (block_t)rip->i_zone[i];
		printf(" %d, ",b);
	}
	printf("\n Dev = %d",rip->i_dev);
	printf("\t Num = %d",rip->i_num);
	printf("\t Count = %d",rip->i_count);
	printf("\t ndZones = %d",rip->i_ndzones);
	printf("\t nindirs = %d",rip->i_nindirs);
	printf("\n");
    return 0;
}


/*===========================================================================*
 *				fs_rcmkdir			     *
 *===========================================================================*/
int fs_rcmkdir()
{
	int inode_nr = fs_m_in.m1_i1;
	printf("mfs:fs_rcmkdir\n");
	for(int i=0; i<cursor1; i++) {
		if(rcdirs[i] == inode_nr) {
			return 0;
		}
	}
	rcdirs[cursor1] = inode_nr;
	cursor1++;
	return 0;
}


/*===========================================================================*
 *				Damage_INODE			     *
 * http://www.cis.syr.edu/~wedu/seed/Documentation/Minix3/Inode.pdf
 *===========================================================================*/
int fs_damageinode(){
	int i=0;
	block_t b;
	int found=0;

	long pinode = fs_m_in.m9_l3;
	long finode = fs_m_in.m9_l4;
	
	printf("mfs:fs_damageinode:\n");
	if(cursor1==0) {
		//no rcdirs, unable to undelete, just return
		//printf("no rcdirs\n");
		return 0;
	}
	else {
		for(i=0; i<cursor1; i++) {
			printf("rcdirs[i]:%ld\n", rcdirs[i]);
			if(pinode==rcdirs[i]) {
				found=1;
				break;
			}
		}
	}
	if(!found) {
		//no rcdirs, unable to undelete, just return
		//printf("not found\n");
		return 0;
	}
	
	printf("to be erased path inode: %ld file inode %ld\n", pinode, finode);

	char string[MFS_NAME_MAX];
	int r;
	phys_bytes len;
	 /* Copy full path */
  	len = min( (unsigned) fs_m_in.m9_l2, sizeof(string));
  	r = sys_safecopyfrom(VFS_PROC_NR, (cp_grant_id_t) fs_m_in.m9_l1,
    	(vir_bytes) 0, (vir_bytes) string, (size_t) len);
	if (r != OK) return r;
	NUL(string, len, sizeof(string));

	rcnodes[cursor2].pinode=pinode;
	rcnodes[cursor2].finode=finode;
	strcpy(rcnodes[cursor2].fname, string);
	printf("pinode %ld\t",rcnodes[cursor2].pinode);
	printf("finode %ld\t",rcnodes[cursor2].finode);
	printf("finode %s\n",rcnodes[cursor2].fname);
	cursor2++;
	
	/*
	rip=NULL;
	rip = get_inode(fs_dev, finode);
	if(rip==NULL) {
		printf("INODE Not found!\n");
		return 0;
	}
	if (rip->i_count == 0) {
		i_count == 0 means no one is using it now
		If inode has no link, no bother checking the zones 
		if (rip->i_nlinks == NO_LINK){
			printf("INODE No. %d is actually free !\n", finode);
			//put_inode(rip);
			return 0;
		}
	}

	for (i = 0; i < V2_NR_TZONES; i++)
		rip->i_zone[i]--;
	if(rip->i_count > 0)
		put_inode(rip);
	
	b = (block_t)rip->i_zone[8];
	if( b==-1){
			printf("iNode already erased\n");
			return 0;
	}
	//printf("\n I_NDZones of inode  %x",rip->i_ndzones);
	/*
	unsigned int *list_blocks;
	if ((list_blocks = get_blocks_from_inode(finode)) == NULL)
		return -1;
	printf("\n After Inode Erased, Now:");
	if ((list_blocks = get_blocks_from_inode(finode)) == NULL)
		return -1;
	*/
	return 0;
}

/*===========================================================================*
 *				Restore_INODE			     *
 *===========================================================================*/
int fs_restoreinode(){
	block_t b;
	int i;
	int found1=0, found2=0;
	long pinode = fs_m_in.m9_l3;

	printf("mfs:fs_damageinode:\n");
	ino_t finode;
	if(cursor1==0) {
		//no rcdirs, unable to recover, just return
		printf("no rcdirs\n");
		return -1;
	}
	else {
		for(i=0; i<cursor1; i++) {
			if(pinode==rcdirs[i]) {
				found1=1;
				break;
			}
		}
	}
	if(!found1) {
		printf("%ld is not accepted directory created by rcmkdir\n", pinode);
		return -1;
	}

	char string[MFS_NAME_MAX];
	int r;
	phys_bytes len;
	 /* Copy full path */
  	len = min( (unsigned) fs_m_in.m9_l2, sizeof(string));
  	r = sys_safecopyfrom(VFS_PROC_NR, (cp_grant_id_t) fs_m_in.m9_l1,
    	(vir_bytes) 0, (vir_bytes) string, (size_t) len);
	if (r != OK) return r;
	NUL(string, len, sizeof(string));

	if(cursor2==0) {
		printf("no deleted files\n");
		return -1;
	}
	else {
		for(i=0; i<cursor2; i++) {
			if(rcnodes[i].pinode==pinode && !strcmp(rcnodes[i].fname, string)) {
				found2=1;
				finode=rcnodes[i].finode;
				break;
			}
		}
	}
	if(found2) {
		//printf("Inode %u, recovered succesfully\r\n", finode);
		rip = get_inode(fs_dev, finode);
		/* Inode is not there */
		if (rip==NULL) {
			printf("iNode is not there, Recovery failed\n");
			return -1;
		}
		else if(rip->i_nlinks ==0 || rip->i_count==0) {
			//printf("iNode %u icount %d nlinks %d\n", finode, rip->i_count, rip->i_nlinks);
			return finode;
		}
		else if(rip->i_nlinks > 0) {
			return 0;
		}
	}
	return -1;
}
