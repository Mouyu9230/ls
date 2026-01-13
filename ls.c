#include<stdio.h>
#include<dirent.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<pwd.h>
#include<grp.h>
#include<time.h>

#define purple "\033[35m"
#define blue "\033[34m"
#define green "\033[32m"
#define yellow "\033[33m"
#define red "\033[31m"
#define cyan "\033[36m"
#define reset "\033[0m"

int aflag,lflag,Rflag,tflag,rflag,iflag,sflag;

typedef struct uni{
    char name[256];
    struct stat stats;
}uni;


int dcheck(struct stat stats){
    if(S_ISDIR(stats.st_mode)!=0){
        return 1;
    }
    return 0;
}


void timesort(uni* unit,int count){
    uni temp;
    for(int i=0;i<count-1;i++){
        for(int j=0;j<count-1-i;j++){
            if(unit[j].stats.st_mtime<unit[j+1].stats.st_mtime){
                temp=unit[j];
                unit[j]=unit[j+1];
                unit[j+1]=temp;
            }
        }
    }
}


void swapsort(uni* unit,int count){
    uni temp;
    for(int i=0;i<count/2;i++){
        temp=unit[i];
        unit[i]=unit[count-1-i];
        unit[count-1-i]=temp;
    }
}


void showinformation(struct stat stats){

    if(S_ISREG(stats.st_mode)!=0){//文件类型
        printf("-");
    }else if(S_ISDIR(stats.st_mode)!=0){
    printf("d");
    }else if(S_ISLNK(stats.st_mode)!=0){
    printf("l");
    }else if(S_ISCHR(stats.st_mode)!=0){
    printf("c");
    }else if(S_ISBLK(stats.st_mode)!=0){
    printf("b");
    }else if(S_ISFIFO(stats.st_mode)!=0){
    printf("p");
    }else if(S_ISSOCK(stats.st_mode)!=0){
    printf("s");
    }else{
    printf("?");
   }

    if((stats.st_mode&S_IRUSR)!=0)printf("r");else printf("-");
    if((stats.st_mode&S_IWUSR)!=0)printf("w");else printf("-");
    if((stats.st_mode&S_IXUSR)!=0)printf("x");else printf("-");
    if((stats.st_mode&S_IRGRP)!=0)printf("r");else printf("-");
    if((stats.st_mode&S_IWGRP)!=0)printf("w");else printf("-");
    if((stats.st_mode&S_IXGRP)!=0)printf("x");else printf("-");
    if((stats.st_mode&S_IROTH)!=0)printf("r");else printf("-");
    if((stats.st_mode&S_IWOTH)!=0)printf("w");else printf("-");
    if((stats.st_mode&S_IXOTH)!=0)printf("x ");else printf("- ");

    printf("%d ",(int)stats.st_nlink);

    struct passwd *pw=getpwuid(stats.st_uid);
    struct group *gr=getgrgid(stats.st_gid);

    if(pw!=NULL)printf("%s ",pw->pw_name);else printf("%d ",stats.st_uid);
    if(gr!=NULL)printf("%s ",gr->gr_name);else printf("%d ",stats.st_gid);

    printf("%lld ",(long long int)stats.st_size);

    printf("%s",ctime(&stats.st_mtime));
}

void list(int aflag,int lflag,int Rflag,int tflag,int rflag,int iflag,int sflag,char* path){
    int dflag=0;
    char pathname[1024];

    struct dirent* content;

    int cap = 128; 
    int tcount = 0;
    uni *tlist = malloc(cap * sizeof(uni));
    if(tlist == NULL){
        perror("malloc");
        return;
    }

    if(path==NULL){//当前或指定
        getcwd(pathname,1024);
    }else{
        strcpy(pathname,path);
    }

    if(pathname==NULL){
        printf("error when trying to get current directory");
        exit(EXIT_FAILURE);
    }

    DIR* cdspointer=opendir(pathname);
    if(cdspointer==NULL){
        perror(pathname);
        return;
    }

    printf("%s:\n",pathname);

    while(1){//read循环
        content=readdir(cdspointer);

        if(content==NULL){
            break;
        }

        char wholepath[1024];

        struct stat stats;
        
        snprintf(wholepath,sizeof(wholepath),"%s/%s",pathname,content->d_name);//拼接

        if(lstat(wholepath, &stats) != 0){
            continue;
        }

        if(tcount >= cap){
            cap *= 2;
            uni *tmp = realloc(tlist, cap * sizeof(uni));
            if(tmp == NULL){
                perror("realloc");
                break;
            }
            tlist = tmp;
        }
        strcpy(tlist[tcount].name,content->d_name);
        tlist[tcount].stats=stats;
        tcount++;
    }

    if(tflag==1){//-t判定
        timesort(tlist,tcount);
    }

    if(rflag==1){
        swapsort(tlist,tcount);
    }

    for(int i=0;i<tcount;i++){
        if(tlist[i].name[0]!='.'||aflag==1){//-a判定
            if(lflag==1){//-l判定
                showinformation(tlist[i].stats);
            }

            if(iflag==1){
                printf(yellow"%lld "reset,(long long)tlist[i].stats.st_ino);
            }

            if(sflag==1){
                printf(purple"%lld "reset,(long long)tlist[i].stats.st_blocks);
            }

            if(dcheck(tlist[i].stats)){
                printf(purple"%s\n"reset,tlist[i].name);
            }else if(tlist[i].name[0]=='.'){
                printf(blue"%s\n"reset,tlist[i].name);
            }else{
                printf(cyan"%s\n"reset,tlist[i].name);
            }

        }
    }

    if(Rflag==1){//-R模块
        for(int i=0;i<tcount;i++){
            if(dcheck(tlist[i].stats)&&!S_ISLNK(tlist[i].stats.st_mode)&&tlist[i].name[0]!='.'){
            char newpath[1024];

            strcpy(newpath,pathname);
            strcat(newpath,"/");
            strcat(newpath,tlist[i].name);

            printf("\n");
            list(aflag,lflag,Rflag,tflag,rflag,iflag,sflag,newpath);
            }
        }
    }
    closedir(cdspointer);
    free(tlist);
}

int main(int argc,char* argv[]){

    int extraposition[10];
    int extrastrcount=0;

    int findflag=0;//遍历寻找参数串并标记以供后续switch解析参数
    int continueflag=0;//遍历输出命令行参数中各个路径对应结果时用来标记，跳过参数字符串

    for(int i=1;i<argc;i++){//标记参数串位置
        if(argv[i][0]=='-'){
            extraposition[extrastrcount]=i;
            extrastrcount++;
        }
    }

    if(extrastrcount!=0){
        for(int s=1;s<argc;s++){
            for(int j=0;j<extrastrcount;j++){
                if(s==extraposition[j]){
                    findflag=1;
                    break;
                }
            }

            if(findflag==1){
                for(int i=1;i<strlen(argv[s]);i++){
                    switch(argv[s][i]){
                        case 'a':aflag=1;break;
                        case 'l':lflag=1;break;
                        case 'R':Rflag=1;break;
                        case 't':tflag=1;break;
                        case 'r':rflag=1;break;
                        case 'i':iflag=1;break;
                        case 's':sflag=1;break;
                    }
                }
            }
            findflag=0;
        }
    }

    if(extrastrcount==argc-1||argc==1){//没输入路径就getcwd输出当前目录内容
        list(aflag,lflag,Rflag,tflag,rflag,iflag,sflag,NULL);

    }else{//有的话就遍历argv输出，跳过参数字符串

        for(int i=1;i<argc;i++){
            for(int j=0;j<extrastrcount;j++){
                if(i==extraposition[j]){
                    continueflag=1;
                    break;
                }
            }
            if(continueflag==1){
                continueflag=0;
                continue;
            }
            list(aflag,lflag,Rflag,tflag,rflag,iflag,sflag,argv[i]);
        }
    }
    return 0;
}
