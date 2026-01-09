//---------------------------------------------------------------
//-a: 显示隐藏文件 
//-l：显示详细信息 
//-R：递归显示
//-t：按时间显示，最新最靠前
//-r：对目录反向排序（以目录的每个首字符所对应的ASCII值进行大到小排序）
//-i：输出i节点索引信息
//-s：在文件名后输出文件大小
#include<stdio.h>
#include<dirent.h>//提供readdir,opendir,closedir,包含dirent结构体
#include<sys/stat.h>//提供stat
#include<sys/types.h>//stat
#include<unistd.h>//stat,getcwd
#include<stdlib.h>//exit...?
#include<string.h>
#include<pwd.h>//getpwduid与返回结构体
#include<grp.h>//getgrgid与返回结构体
#include<time.h>//ctime函数

#define purple "\033[35m"//为后面分颜色输出用宏定义包装对应颜色的代码
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
    if(S_ISREG(stats.st_mode)!=0){
        return 0;
    }
    return 1;
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
    if(S_ISREG(stats.st_mode)!=0){//文件类型（未完成
        printf("-");
    }else{
        printf("d");
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
    char pathname[256];
    struct dirent* content;
    uni tlist[1024];//存所有unit以供排序
    int tcount=0;

    if(path==NULL){//当前或指定
        getcwd(pathname,256);
    }else{
        strcpy(pathname,path);
    }
    if(pathname==NULL){
        printf("error when trying to get current directory");
        exit(EXIT_FAILURE);
    }
    DIR* cdspointer=opendir(pathname);
    if(cdspointer==NULL){
        printf("error when trying to open directory");
        exit(EXIT_FAILURE);
    }
    printf("%s:\n",pathname);
    while(1){//read循环
        content=readdir(cdspointer);
        if(content==NULL){
            break;
        }
        char wholepath[256];
        struct stat stats;
        snprintf(wholepath,sizeof(wholepath),"%s/%s",pathname,content->d_name);//拼接
        if(stat(wholepath,&stats)!=0){
            continue;
    }
        stat(wholepath,&stats);
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

    for(int i=0;i<tcount;i++){//输出循环
        if(tlist[i].name[0]!='.'||aflag==1){//-a判定
            if(lflag==1){//-l判定
                showinformation(tlist[i].stats);
            }
            if(iflag==1){//根据不同情况分颜色输出
                printf(blue"%lld "reset,(long long)tlist[i].stats.st_ino);
            }
            if(sflag==1){
                printf(purple"%lld "reset,(long long)tlist[i].stats.st_blocks);//文件占用磁盘块数，系统默认1kb一块，stat里面512b算一块，除以二换算*
            }
            if(dcheck(tlist[i].stats)){
                printf(red"%s\n"reset,tlist[i].name);
            }else if(tlist[i].name[0]=='.'){
                printf(yellow"%s\n"reset,tlist[i].name);
            }else{
                printf(cyan"%s\n"reset,tlist[i].name);
            }
        }
    }
    closedir(cdspointer);
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
