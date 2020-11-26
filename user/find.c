/*
    Usage： find path name
    实现在给定路径的文件夹树中找到全部匹配name的
*/
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
 
char *getLastElem(char *p){
    char *w=p;
    char *last=0;
    while(*w!='\0'){
        if(*w=='/'){
            last=w;
        }
        w++;
    }
    if(last==0){
        return p;
    }
    return last+1;
}
void find(char* path,char *name){
    char buf[512], *p=0;
    int fd;
    struct dirent de;
    struct stat st;
 
    if((fd = open(path, O_RDONLY)) < 0){
        fprintf(2, "ls: cannot open %s\n", path);
        return;
    }
 
    if(fstat(fd, &st) < 0){
        fprintf(2, "ls: cannot stat %s\n", path);
        close(fd);
        return;
    }
    switch(st.type){
    case T_FILE:
        p=getLastElem(path);
        if(strcmp(p,name)==0)
            printf("%s\n",path);
        break;
 
    case T_DIR:
    	p=getLastElem(path);
        if(strcmp(p,name)==0){
            printf("%s\n",path);
            break;
        }
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
        printf("ls: path too long\n");
        break;
        }
        strcpy(buf, path);
        p = buf+strlen(buf);
        *p++ = '/';
        while(read(fd, &de, sizeof(de)) == sizeof(de)){
            if(de.inum == 0)
                continue;
            int t=strlen(de.name)>DIRSIZ?DIRSIZ:strlen(de.name);
            memmove(p, de.name, t);
            p[t] = 0;
            if(stat(buf, &st) < 0){
                printf("ls: cannot stat %s\n", buf);
                continue;
            }
            if(strcmp(de.name,".")==0||strcmp(de.name,"..")==0){
                continue;
            }
            find(buf,name);
        }
        break;
    }
    close(fd);
 
}
int main(int argc,char* argv[]){
    if(argc<3){
        exit();
    }
    find(argv[1],argv[2]);
    exit();
 
}
