//用不同的函数块实现不同的需求,为确保可以组合使用参数，考虑通过在list中增加其他函数是否执行的判定来完成组
//合，而不是分别在各个参数对应的函数块完整的实现对应的目标
//---------------------------------------------------------------
//-a: 显示隐藏文件 决定list在循环获取文件名输出时是否取消检测首字符的环节 已完成
//-l：显示详细信息 用stat获取详细信息，在输出文件名的循环前加入判定，若有则输出stat的信息
//-R：递归显示
//-t：按时间显示，最新最靠前
//-r：对目录反向排序（以目录的每个首字符所对应的ASCII值进行大到小排序）
//-i：输出i节点索引信息
//-s：在文件名后输出文件大小
#include<stdio.h>
#include<dirent.h>//提供readdir,opendir,closedir,包含dirent结构体
#include<sys/stat.h>//提供stat
#include<sys/types.h>//stat
#include <unistd.h>//stat,getcwd
#include<stdlib.h>//exit...?
#include<string.h>
#include<fcntl.h>

#define purple "\033[35m"//为后面分颜色输出用宏定义包装对应颜色的代码
#define blue "\033[34m"
#define red "\033[31m"
#define cyan "\033[36m"
#define reset "\033[0m"

int aflag,lflag,Rflag,tflag,rflag,iflag,sflag;



void information(char* filename,char* pathname){
    char wholepath[9999];
    struct stat stats;
    snprintf(wholepath,sizeof(wholepath),"%s/%s",pathname,filename);
    stat(wholepath,&stats);
    if(S_ISREG(stats.st_mode)!=0){//文件类型（未完成
        printf("-");
    }
    if((stats.st_mode&S_IRUSR)!=0)printf("r");else{printf("-");}//权限
    if((stats.st_mode&S_IWUSR)!=0)printf("w");else{printf("-");}
    if((stats.st_mode&S_IXUSR)!=0)printf("x");else{printf("-");}
    if((stats.st_mode&S_IRGRP)!=0)printf("r");else{printf("-");}
    if((stats.st_mode&S_IWGRP)!=0)printf("w");else{printf("-");}
    if((stats.st_mode&S_IXGRP)!=0)printf("x");else{printf("-");}
    if((stats.st_mode&S_IROTH)!=0)printf("r");else{printf("-");}
    if((stats.st_mode&S_IWOTH)!=0)printf("w");else{printf("-");}
    if((stats.st_mode&S_IXOTH)!=0)printf("x ");else{printf("- ");}
    printf("%d ",stats.st_nlink);//（未完成
    printf("%s %s ",stats.st_uid,stats.st_gid);
    printf("%lld",stats.st_size);
    printf("%s ",stats.st_mtime);


 

}

void list(int aflag,int lflag,int Rflag,int tflag,int rflag,int iflag,int sflag,char* path){
    char pathname[9999];
    struct dirent* content;
    if(path==NULL){//当前或指定
        getcwd(pathname,9999);
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
    while(1){//主循环
        content=readdir(cdspointer);
        if(content==NULL){
            break;
        }
        if(content->d_name[0]!='.'||aflag==1){//-a判定
            if(lflag==1){
                information(content->d_name,pathname);
            }
            printf("%s\n",content->d_name);
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
    if((extrastrcount!=0&&argc==2)||argc==1){//没输入路径就getcwd输出当前目录内容
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

