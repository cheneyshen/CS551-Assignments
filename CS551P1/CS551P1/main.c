//
//  main.c
//  CS551P1
//
//  Created by spring  on 9/22/16.
//  Copyright © 2016 spring . All rights reserved.
//
#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAXARGS 10
#define MAXLEN 256
#define MAXLEN2 512
#define ALIASLEN 32
jmp_buf jbuffer;

struct alias{
    char key[ALIASLEN];
    char value[ALIASLEN];
}aliases[ALIASLEN];

// All commands have at least a type. Have looked at the type, the code
// typically casts the *cmd to some specific cmd type.
struct cmd {
    int type;          //  ' ' (exec), | (pipe), '<' or '>' for redirection
};

struct execcmd {
    int type;              // ' '
    char *argv[MAXARGS];   // arguments to the command to be exec-ed
};

struct redircmd {
    int type;          // < or >
    struct cmd *cmd;   // the command to be run (e.g., an execcmd)
    char *file;        // the input/output file
    int mode;          // the mode to open the file with
    int fd;            // the file descriptor number to use for the file
};

struct pipecmd {
    int type;          // & for CS551 P1
    struct cmd *left;  // left side of pipe
    struct cmd *right; // right side of pipe
};

struct listcmd {
    int type;          // ; for CS551 P1
    struct cmd *left;  // left side of pipe
    struct cmd *right; // right side of pipe
};

struct backcmd {
    int type;          // &
    struct cmd *cmd;   // the command to be run (e.g., an execcmd)
};

int fork1(void);  // Fork but exits on failure.
int wait();
struct cmd *parsecmd(char*);

// Execute cmd.  Never returns.
void runcmd(struct cmd *cmd)
{
    int p[2], r;
    struct backcmd *bcmd;
    struct execcmd *ecmd;
    struct pipecmd *pcmd;
    struct listcmd *lcmd;
    struct redircmd *rcmd;
    
    if(cmd == 0)
        exit(0);
    
    switch(cmd->type){
        case ' ':
            ecmd = (struct execcmd*)cmd;
            if(ecmd->argv[0] == 0)
                exit(0);
            
            execvp(ecmd->argv[0], ecmd->argv);
            //fprintf(stdout, "execv %s\n", ecmd->argv[0]);
            break;
        case '>':
        case '<':
            rcmd = (struct redircmd*)cmd;
            close(rcmd->fd);
            if(open(rcmd->file, rcmd->mode)<0) {
                fprintf(stderr, "open %s failed", rcmd->file);
                exit(0);
            }
            runcmd(rcmd->cmd);
            break;
        case ';':
            lcmd=(struct listcmd*)cmd;
            if(fork1() == 0)
              runcmd(lcmd->left);
            wait(&r);
            runcmd(lcmd->right);
            break;
        case '|':
            pcmd = (struct pipecmd*)cmd;
            if(pipe(p) < 0) {
                fprintf(stderr, "pipe\n");
                exit(0);
            }
            if(fork1() == 0){
                close(1);
                dup(p[1]);
                close(p[0]);
                close(p[1]);
                runcmd(pcmd->left);
            }
            if(fork1() == 0){
                close(0);
                dup(p[0]);
                close(p[0]);
                close(p[1]);
                runcmd(pcmd->right);
            }
            close(p[0]);
            close(p[1]);
            wait(&r);
            wait(&r);
            break;
        case '&':
            bcmd=(struct backcmd*)cmd;
            if(fork1()==0)
                runcmd(bcmd->cmd);
            break;
        default:
            fprintf(stderr, "unknown runcmd\n");
            exit(-1);
            
    }
    exit(0);
}

void print_prompt(){
	char * prompt=getenv("PROMPT");
	char buffer[256];
	printf("%s%s",getcwd(buffer,256),prompt);
}


int getcmd(char *buf, int nbuf)
{
    //getprofile();
    if (isatty(fileno(stdin))){
       
       print_prompt();
       //fprintf(stdout, "[CS551] ");
    }
    memset(buf, 0, nbuf);
    fgets(buf, nbuf, stdin);
    if(buf[0] == 0) // EOF
        return -1;
    return 0;
}

int fork1(void)
{
    int pid;
    
    pid = fork();
    if(pid == -1)
        perror("fork");
    //fprintf(stdout, "pid: %d\n", pid);
    return pid;
}

//command to be executed constructor
struct cmd* execcmd(void)
{
    struct execcmd *cmd;
    
    cmd = malloc(sizeof(*cmd));
    memset(cmd, 0, sizeof(*cmd));
    cmd->type = ' ';
    return (struct cmd*)cmd;
}

//command redirected constructor
struct cmd* redircmd(struct cmd *subcmd, char *file, int type)
{
    struct redircmd *cmd;
    
    cmd = malloc(sizeof(*cmd));
    memset(cmd, 0, sizeof(*cmd));
    cmd->type = type;
    cmd->cmd = subcmd;
    cmd->file = file;
    cmd->mode = (type == '<') ?  O_RDONLY : O_WRONLY|O_CREAT|O_TRUNC;
    cmd->fd = (type == '<') ? 0 : 1;
    return (struct cmd*)cmd;
}

//pipe command constructor
struct cmd* pipecmd(struct cmd *left, struct cmd *right)
{
    struct pipecmd *cmd;
    
    cmd = malloc(sizeof(*cmd));
    memset(cmd, 0, sizeof(*cmd));
    cmd->type = '|';
    cmd->left = left;
    cmd->right = right;
    return (struct cmd*)cmd;
}

//list command constructor
struct cmd* listcmd(struct cmd *left, struct cmd *right)
{
  struct listcmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = ';';
  cmd->left = left;
  cmd->right = right;
  return (struct cmd*)cmd;
}

//backend command constructor
struct cmd* backcmd(struct cmd *subcmd)
{
    struct backcmd *cmd;
    
    cmd = malloc(sizeof(*cmd));
    memset(cmd, 0, sizeof(*cmd));
    cmd->type = '&';
    cmd->cmd = subcmd;
    return (struct cmd*)cmd;
}

// Parsing
char whitespace[] = " \t\r\n\v";
char symbols[] = "<>&|;";

int gettoken(char **ps, char *es, char **q, char **eq)
{
    char *s;
    int ret;
    
    s = *ps;
    while(s < es && strchr(whitespace, *s))
        s++;
    if(q)
        *q = s;
    ret = *s;
    switch(*s){
        case 0:
            break;
        case '<':
        case '>':
            s++;
            break;
        case ';':
        case '|':
        case '&':
            s++;
            break;
        default:
            ret = 'a';
            while(s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
                s++;
            break;
    }
    if(eq)
        *eq = s;
    
    while(s < es && strchr(whitespace, *s))
        s++;
    *ps = s;
    return ret;
}

/*
 * skip the whitespaces till the next char, 
 * which is tested to see whether it occurs in toks char stream
 */
int peek(char **ps, char *es, char *toks)
{
    char *s;
    
    s = *ps;
    //fprintf(stdout, "s:%s\n", s);
    while(s < es && strchr(whitespace, *s))
        s++;
    *ps = s;
    return *s && strchr(toks, *s);
}

/*
 * make a copy of the characters in the input buffer, starting from s through es.
 * null-terminate the copy to make it a string.
 */
char* mkcopy(char *s, char *es)
{
    int n = (int)(es - s);
    char *c = malloc(n+1);
    assert(c);
    strncpy(c, s, n);
    c[n] = 0;
    return c;
}


struct cmd* parseredirs(struct cmd *cmd, char **ps, char *es)
{
    int tok;
    char *q, *eq;
    
    while(peek(ps, es, "<>")){
        tok = gettoken(ps, es, 0, 0);
        if(gettoken(ps, es, &q, &eq) != 'a') {
            fprintf(stderr, "missing file for redirection\n");
            exit(-1);
        }
        switch(tok){
            case '<':
                cmd = redircmd(cmd, mkcopy(q, eq), '<');
                break;
            case '>':
                cmd = redircmd(cmd, mkcopy(q, eq), '>');
                break;
        }
    }
    return cmd;
}

//exec command genertor, supports redirection
struct cmd* parseexec(char **ps, char *es)
{
    char *q, *eq;
    int tok, argc;
    struct execcmd *cmd;
    struct cmd *ret;
    
    ret = execcmd();
    cmd = (struct execcmd*)ret;
    
    argc = 0;
    ret = parseredirs(ret, ps, es);
    // update tokens, need ;| and & for pipe and backend
    while(!peek(ps, es, ";|&")){
        if((tok=gettoken(ps, es, &q, &eq)) == 0)
            break;
        if(tok != 'a') {
            fprintf(stderr, "syntax error\n");
            exit(-1);
        }
        cmd->argv[argc] = mkcopy(q, eq);
        argc++;
        if(argc >= MAXARGS) {
            fprintf(stderr, "too many args\n");
            exit(-1);
        }
        ret = parseredirs(ret, ps, es);
    }
    cmd->argv[argc] = 0;
    return ret;
}

//parallelized commands generator
struct cmd* parsepipe(char **ps, char *es)
{
    struct cmd *cmd;
    
    cmd = parseexec(ps, es);
    if(peek(ps, es, "|")){
        gettoken(ps, es, 0, 0);
        cmd = pipecmd(cmd, parsepipe(ps, es));

    }
    return cmd;
}

//general commands generator
struct cmd* parseline(char **ps, char *es)
{
    struct cmd *cmd;
    cmd = parsepipe(ps, es);
    while(peek(ps, es, "&")){
        gettoken(ps, es, 0, 0);
        cmd = backcmd(cmd);
    }
    if(peek(ps, es, ";")){
        gettoken(ps, es, 0, 0);
        cmd = listcmd(cmd, parseline(ps, es));
    }
    return cmd;
}

//commands generator from input buffer
struct cmd* parsecmd(char *s)
{
    char *es;
    struct cmd *cmd;
    
    es = s + strlen(s); // tail of string s
    cmd = parseline(&s, es);
    peek(&s, es, "");
    if(s != es){
        fprintf(stderr, "leftovers: %s\n", s);
        exit(-1);
    }
    return cmd;
}

// Helpers
void ctrl_c(int sig){
    char c[16];
    printf("\nAre you sure to EXIT ? [y/n]: ");
    scanf("%s",c);

    if(strcmp(c,"y")==0) exit(0);
    else longjmp(jbuffer,0);
}

/*
 * generate 5stars
 */
void star(int (*p)[62],int i,int j,int n)
{
    int k,l,a,b;
    for(k=0;k<18;k++,i++,j++)
    {
        *(*(p+i)+j)=n;
        if(k>7&&k<11)
            *(*(p+i)+j)=0;
    }
    i--,j--;
    for(k=0;k<11;k++,i--,j-=4)
    {
        *(*(p+i)+j)=n;
        if(k>4&&k<7)
            *(*(p+i)+j)=0;
    }
    i++,j+=4;
    for(k=0;k<47;k++,j++)
    {
        *(*(p+i)+j)=n;
    }
    j--;
    for(k=0;k<11;k++,i++,j-=4)
    {
        *(*(p+i)+j)=n;
        if(k>3&&k<6)
            *(*(p+i)+j)=0;
    }
    i--,j+=4;
    for(k=0;k<18;k++,i--,j++)
    {
        *(*(p+i)+j)=n;
        if(k>6&&k<10)
            *(*(p+i)+j)=0;
    }
    i++,j--;
    a=i,b=j;
    i++,j++;
    for(k=1;k<17;k++,i++,j++)
        for(l=j-1;*(*(p+i)+l)!=n;l--)
            *(*(p+i)+l)=n+1;
    i=a,j=b;
    i++,j--;
    for(k=1;k<17;k++,i++,j--)
        for(l=j+1;*(*(p+i)+l)!=n;l++)
            *(*(p+i)+l)=n+1;
}

/*
 * print 5stars
 */
void print(int(*p)[62])
{
    int i,i2=0,j,j2=49;
    for(i=0;i<20;i++)
    {
        fprintf(stdout," ");
        for(j=0;j<62;j++)
        {
            if(i==i2||i==i2+1)
                if(j>j2&&j<j2+13)
                    continue;
            if(*(*(p+i)+j)==1||*(*(p+i)+j)==2)
                fprintf(stdout,"*");
            else if(*(*(p+i)+j)==3||*(*(p+i)+j)==4)
                fprintf(stdout,"*");
            else
                fprintf(stdout," ");
        }
        fprintf(stdout," \n");
    }
}

/*
 * check if buffer constatins parenthesis, 
 * if has, must has matched "(" and ")"
 * refer to leetcode
 */
int validateparenthes(const char* s)
{
    char paren[MAXLEN];
    bzero(paren, MAXLEN);
    int ct1=0,ct2=0; // ( counter and ) counter
    for(int i=0;i<strlen(s);i++) {
        switch(s[i]) {
            case '(':
                paren[ct1]='(';ct1++;
                break;
            case ')':
                if(paren[ct1-1]=='(')
                    ct1--;
                else ct2++;
                break;
            default:;
        }
    }
    if(ct1==0&&ct2==0)
        return 1;
    else
        return 0;
}

/*
 * convert buffer to linear commands queue
 * the precedence of the commands are from left to right
 * but parenthesized commands supersedes that order
 */
void changesequence(char* ps) {
    //printf("from:%s\n",ps);
    char dest[MAXLEN];
    bzero(dest, MAXLEN);
    int leng=(int)strlen(ps);
    strncpy(dest, ps, leng);
    bzero(ps, MAXLEN);
    int left=0; //first '('
    int right=leng-1; //first ')'
    int times=0; //'(' time of occurrence
    int paren=0;
    int leftoffset=0;
    int rightoffset=leng-1;
    for(int i=0;i<leng;i++) {
        if (dest[i]=='(') {
            if(times==0)
                left=i; //found it
            times++;
            paren=1;
        }
        else if(dest[i]==')') {
            times--;
            if(times==0) {
                right=i; // found matched one
                break;
            }
        }
    }
    if(paren) //have parentheses
        strncpy(ps, dest+left+1, right-left-1);
    else {
        char* pdest=strchr(dest,',');
        while (pdest!=NULL)
        {
            dest[pdest-dest]='|';
            pdest=strchr(pdest+1,',');
            //printf("to0:%s\n",dest);
        }
        strncpy(ps, dest, leng);
    }
    //printf("left:%d\tright:%d\tleng:%d\tdest[leng-1]=%c\n",left, right, leng, dest[leng-1]);
    if(left>0) {
        strncat(ps, ";", 1);
        while(isalnum(dest[left])==0) {
            left--; //cut unuseful letter
        }
        while(dest[leftoffset]!='$' && isalnum(dest[leftoffset])==0) {
            leftoffset++; //cut unuseful letter, alias $ must be header
        }
        strncat(ps, dest+leftoffset, left-leftoffset+1);
        //printf("to1:%s\n",ps);
    }
    if(right<leng-2) {
        char *semi=strchr(dest+right, ';');
        if(semi==NULL) {
            if(left>0) {
                strncat(ps, "|", 1);
            }
            else
                strncat(ps, ";", 1);
            //printf("to2:%s\n",ps);
            while(dest[right]!='$' && isalnum(dest[right])==0) {
                right++; //cut unuseful letter
            }
        }
        while(isalnum(dest[rightoffset])==0) {
            rightoffset--;  //cut unuseful letter
        }
        if(semi==NULL)
            strncat(ps, dest+right, rightoffset-right+1);
        else
            //right is ), should +1, otherwise output has )
            strncat(ps, dest+right+1, rightoffset-right+1);
        //printf("to3:%s\n",ps);
    }
    if(strchr(ps, '(')!=0 || strchr(ps, ',')!=0) {
        changesequence(ps);
    }
    return;
}

void convertalias(char* ps) {
    char *pch;
    char *penv;
    char dest[MAXLEN];
    bzero(dest, MAXLEN);
    char *buf=ps;
    //printf("Splitting string \"%s\" into tokens:\n",ps);
    char *outer_ptr=NULL;
    char *inner_ptr=NULL;
    /*
    char *str1, *str2, *token, *subtoken;
    char *saveptr1, *saveptr2;
    for (str1 = ps; ; str1 = NULL) {
        token = strtok_r(str1, "$", &saveptr1);
        if (token == NULL)
            break;
        printf("%s\n", token);

       for (str2 = token; ; str2 = NULL) {
            subtoken = strtok_r(str2, " ", &saveptr2);
            if (subtoken == NULL)
                break;
            }
        }
    }
    */
    while ((pch = strtok_r(buf,"$",&outer_ptr))!=NULL)
    {
        buf=pch;
        while ((pch = strtok_r(buf," ",&inner_ptr))!=NULL) {
            int repeated=0; // prevent from infinite recursive in aliases
            int slash=0; // tailer signal is ,
            //printf("pch=%s\n",pch);
            //fetch cursor to valid char
            while(strchr(" \t\r\n\v", pch[strlen(pch)-1]))
                pch[strlen(pch)-1]='\0';
            if(pch[strlen(pch)-1]==',') {
                pch[strlen(pch)-1]='\0';    // delete the ,
                slash=1;
            }
            //strncpy(tmp, pch, strlen(pch)-1);
            while((penv=getenv(pch))!=NULL) {
                printf("penv:%s\n",penv);
                if(repeated>2) {
                    fprintf(stderr, "nested alias definition!");
                    exit(0);
                }
                pch = penv;
                repeated++;
            }
            strncat(dest, pch, strlen(pch));
            strcat(dest, " ");
            if(slash) // deleted ",", now add it
                strcat(dest, ",");
            buf=NULL;
        }
        buf=NULL;
    }
    bzero(ps, MAXLEN);
    strncpy(ps, dest, strlen(dest));
    //printf("ps=%s\n",ps);
}

void getprofile(){
    FILE * file=fopen("PROFILE","r");
    if(file==NULL){
        printf("PROFILE file not found, default home directory\n");
        setenv("HOME","/root",1);
        chdir("/root/Group15");
        return;
    }
    char tmp1[256];
    char tmp2[256];
    int k=0; //aliases counter in PROFILE file
    while (fgets(tmp1,sizeof(tmp1),file)){
        int j=0;
        int i=0;
        while(tmp1[i]!='='){
            tmp2[i]=tmp1[i];
            i++;
        }
        tmp2[i]='\0';
        //printf("tmp1=%s\n", tmp1);
        //printf("tmp2=%s\n", tmp2);
        if(strcmp(tmp2,"PATH")==0){
            i++;
            while(tmp1[i]!='\0'&&tmp1[i]!='\n'){
                tmp2[j]=tmp1[i];
                j++;
                i++;
            }
            tmp2[j]='\0';
            setenv("PATH",tmp2,1);
        }
        else if(strcmp(tmp2,"PROMPT")==0){
            i++;
            while(tmp1[i]!='\0'&&tmp1[i]!='\n'){
                tmp2[j]=tmp1[i];
                j++;
                i++;
            }
            tmp2[j]='\0';
            setenv("PROMPT",tmp2,1);
        }
        else if(strcmp(tmp2,"HOME")==0){
            i++;
            while(tmp1[i]!='\0'&&tmp1[i]!='\n'){
                tmp2[j]=tmp1[i];
                j++;
                i++;
            }
            tmp2[j]='\0';
            setenv("HOME",tmp2,1);
            chdir(tmp2);
        }
        else if(strstr(tmp2,"alias")!=0) {
            //sample: alias listcontent="ls"
            strcpy(tmp2, tmp1);
            //printf("tmp2=%s\n", tmp2);
            char * pch;
            pch = strtok(tmp2," \"=");
            //printf ("%s\n",pch);
            pch = strtok(NULL," \"=");
            strncpy(aliases[k].key, pch, strlen(pch));
            //printf ("%s\n",pch);
            pch = strtok(NULL, "\"=");
            strncpy(aliases[k].value, pch, strlen(pch));
            //printf("%s\n",pch);
            setenv(aliases[k].key, aliases[k].value, 1);
            //printf("key=%d, value=%s\n", strlen(aliases[k].key), getenv(aliases[k].key));
            k++;
        }
    }
    fclose(file);
}

int main(void){
    int a[20][62]={0};  // for printing 5stars
    star(a,2,25,3);
    print(a);
    fprintf(stdout,"================--^-^----WELCOME----^-^--================\n");
    fprintf(stdout,"                @  +  @            @ +  @                \n");
    fprintf(stdout,"                   _                 _                   \n");
    fprintf(stdout,"Group name: No.15\n");
    fprintf(stdout, "Group member:Fei Shen A20354692\t");
    fprintf(stdout, "Guanxin Wang A20356484\n");
    static char buf[MAXLEN];
    int fd, r;

    getprofile();

    // Assumes three file descriptors open.
    while((fd = open("console", O_RDWR)) >= 0){
        if(fd >= 3){
            close(fd);
            break;
        }
    }
    /*
    char line1[MAXLEN] ="command1, command2, (command3)";
    char line2[MAXLEN] ="command1, command2, (command3, (command4))";
    char line3[MAXLEN] ="(command1, command2)";
    char line4[MAXLEN] ="(command1),command2";
    char line5[MAXLEN] ="((command3))";
    char line6[MAXLEN] ="((((command3))))";
    char line7[MAXLEN] ="((((command3)))), (((((command4)))))";
    char line8[MAXLEN] ="123;;;";
    
    while(strchr(line1, '(')!=0) {
        convertline(line1);
    }
    while(strchr(line2, '(')!=0) {
        convertline(line2);
    }
    while(strchr(line6, '(')!=0) {
        convertline(line6);
    }
    while(strchr(line7, '(')!=0) {
        convertline(line7);
    }
    
    while(strchr(line8, '(')!=0) {
        convertline(line8);
    }
    char* s = "   (ala ma kota";
    char* es = s + strlen(s);
    printf("peek(&s, es, \"(\") = %s\n", peek(&s, es, "(") ? "true" : "false");
    printf("peek(&s, es, \"<(\") = %s\n", peek(&s, es, "<(") ? "true" : "false");
    printf("peek(&s, es, \"<\") = %s\n", peek(&s, es, "<") ? "true" : "false");
    return 0;
    */
    signal(SIGINT,ctrl_c);  // set exit event handler
    setjmp(jbuffer);    // used for performing nonlocal gotos 

    //convertalias("ls -a, $listcontent -l, $cao");

    // Read and run input commands.
    while(getcmd(buf, sizeof(buf)) >= 0){
        if(buf[0] == 'c' && buf[1] == 'd' && buf[2] == ' '){
            // Clumsy but will have to do for now.
            // Chdir has no effect on the parent if run in the child.
            buf[strlen(buf)-1] = 0;  // chop \n
            if(chdir(buf+3) < 0)
                fprintf(stderr, "cannot cd %s\n", buf+3);
            continue;
        }
        // type exit to log off
        if(strstr(buf, "exit")!=0){
            printf("System Log Off\n");
            exit(0);
        }
        // child process
        if(fork1() == 0) {
            // check if matched parentheses
            int valid=validateparenthes(buf);
            //printf("%d\n", valid);
            if (valid) {
                convertalias(buf);
                changesequence(buf);
                //fprintf(stdout, "buf1:%s\n", buf);
                //fprintf(stdout, "buf2:%s\n", buf);
                //fprintf(stdout, "buf: %s\n", buf);
                runcmd(parsecmd(buf));
           }
        }
        //parent process
        wait(&r);
        //printf("execvp done\n\n");
    }
    exit(0);
}
