#include "fs.h"
#include "file.h"
#include "path.h"
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <minix/callnr.h>
#include <minix/safecopies.h>
#include <minix/endpoint.h>
#include <minix/com.h>
#include <minix/sysinfo.h>
#include <minix/u64.h>
#include <minix/vfsif.h>
#include <sys/ptrace.h>
#include <sys/svrctl.h>
#include <sys/resource.h>
#include <sys/dirent.h>
#include <sys/stat.h>
#include "file.h"
#include "vnode.h"
#include "vmnt.h"
#include "path.h"
#include "lock.h"


/*===========================================================================*
 *        do_inodeviewer           *
 *===========================================================================*/

int do_inodeviewer(){
  printf("do_inodeviewer\n");
  struct vnode *vp;
  struct vmnt *vmp;
  struct lookup resolve;
  char *devPath;
  devPath="/dev/c0d0p0s2";
  int inode_nr;
  inode_nr = job_m_in.m1_i2;
  
  lookup_init(&resolve, devPath, PATH_NOFLAGS, &vmp, &vp);
  resolve.l_vmnt_lock = VMNT_READ;
  resolve.l_vnode_lock = VNODE_READ;

  if ((vp = eat_path(&resolve, fp)) == NULL) {
    printf("unknown MFS API error\n");
    return -1;
  }
  unlock_vnode(vp);
  unlock_vmnt(vmp);
  put_vnode(vp);
  req_inodeviewer(vmp->m_fs_e, inode_nr);
  return 0;
}

/*===========================================================================*
 *        do_rcmkdir         *
 *===========================================================================*/
int do_rcmkdir()
{
  printf("do_rcmkdir\n");
  struct vnode *vp;
  struct vmnt *vmp;
  char *devPath;
  
  struct lookup resolve;
  devPath="/dev/c0d0p0s2";
  int inode_nr = job_m_in.m1_i1;
  printf("rcDir=%d\n", inode_nr);
  lookup_init(&resolve, devPath, PATH_NOFLAGS, &vmp, &vp);
  resolve.l_vmnt_lock = VMNT_READ;
  resolve.l_vnode_lock = VNODE_READ;
  if ((vp = eat_path(&resolve, fp)) == NULL) {
    printf("MFS error\n");
    return -1;
  }
  unlock_vnode(vp);
  unlock_vmnt(vmp);
  put_vnode(vp);
  req_rcmkdir(vmp->m_fs_e, inode_nr);
  return 0;
}

/*===========================================================================*
 *        do_damageinode         *
 *===========================================================================*/
int do_damageinode()
{
  printf("do_damageinode\n");
  char fullpath[PATH_MAX];

  size_t vname_length = job_m_in.m1_i1;
  vir_bytes vname = (vir_bytes)job_m_in.m1_p1;
  //printf("vfs: %u\n", vname_length);
  if(copy_name(vname_length, fullpath)!=OK) {
    if(fetch_name(vname, vname_length, fullpath)!=OK)
      return(err_code);
  }
  ino_t pinode = job_m_in.m1_i2;
  ino_t finode = job_m_in.m1_i3;
  struct vnode *vp;
  struct vmnt *vmp;
  char *devPath;
  struct lookup resolve;
  devPath="/dev/c0d0p0s2";
  lookup_init(&resolve, devPath, PATH_NOFLAGS, &vmp, &vp);
  resolve.l_vmnt_lock = VMNT_READ;
  resolve.l_vnode_lock = VNODE_READ;
  if ((vp = eat_path(&resolve, fp)) == NULL) {
    printf("MFS error\n");
    return -1;
  }
  req_damageinode(vmp->m_fs_e, pinode, finode, fullpath);
  unlock_vnode(vp);
  unlock_vmnt(vmp);
  put_vnode(vp);
  
  return 0;
}

/*===========================================================================*
 *        do_restoreinode         *
 *===========================================================================*/
int do_restoreinode()
{
  printf("do_restoreinode\n");
  char name[PATH_MAX];

  size_t vname_length = job_m_in.m1_i1;
  vir_bytes vname = (vir_bytes)job_m_in.m1_p1;
  printf("vfs: %u\n", vname_length);
  if(copy_name(vname_length, name)!=OK) {
    if(fetch_name(vname, vname_length, name)!=OK)
      return(err_code);
  }
  ino_t pinode = job_m_in.m1_i2;
  struct vnode *vp;
  struct vmnt *vmp;
  char *devPath;
  struct lookup resolve;
  devPath="/dev/c0d0p0s2";
  lookup_init(&resolve, devPath, PATH_NOFLAGS, &vmp, &vp);
  resolve.l_vmnt_lock = VMNT_READ;
  resolve.l_vnode_lock = VNODE_READ;
  if ((vp = eat_path(&resolve, fp)) == NULL) {
    printf("MFS error\n");
    return -1;
  }
  //printf("vfs: %u %s\n", pinode, name);
  int r = req_restoreinode(vmp->m_fs_e, pinode, name);
  //printf("vfs.do_restoreinode: %d\n", r);
  unlock_vnode(vp);
  unlock_vmnt(vmp);
  put_vnode(vp);
  return r;
 
}

