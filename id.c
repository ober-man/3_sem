#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<sys/types.h>
#include<unistd.h>
#include<pwd.h>
#include<grp.h>

const int MAXSIZE = 100;

int main(int argc, char* argv[])
{
    uid_t uid;
    gid_t gid;
    struct passwd* user;
    struct group* grp;
    gid_t groups[100];
    char* arg;
    int check = 0;
    
    switch(argc)
    {
    case 1: // id (no arguments)
        check = getgroups(MAXSIZE, groups);
        if(check <= 0)
        {
            printf("Error in groups\n");
            exit(-1);
        }
    
        uid = getuid();
        user = getpwuid(uid);
    
        gid = getgid();
        grp = getgrgid(gid);
    
        printf("uid=%d(%s) gid=%d(%s) groups=", uid, user->pw_name, gid, grp->gr_name);
    
        for(int i = check-1; i >=0; --i)
        {
            grp = getgrgid(groups[i]);
            if(i==0)
                printf("%d(%s)", groups[i], grp->gr_name);
            else
                printf("%d(%s),", groups[i], grp->gr_name);
        }
        printf("\n");
        break;

    case 2: // id arg
        arg = argv[1];
        if(isdigit(arg[0])) // id uid
        {
            uid = atoi(arg);
            user = getpwuid(uid);
            grp = getgrgid(uid);
            if(user == NULL)
            {
                printf("id: '%d': no such user", uid);
                exit(-1);
            }
        }
        else // id name
        {
            user = getpwnam(arg);
            grp = getgrnam(arg);
            if(user == NULL)
            {
                printf("id: '%s': no such user", arg);
                exit(-1);
            }
        }
        
        if(user == NULL)
        {
            printf("id: '%s': no such user", arg);
            exit(-1);
        }
    
        
        if(grp == NULL)
        {
            printf("Error in group identification");
            exit(-1);
        }
    
        printf("uid=%d(%s) gid=%d(%s) groups=", user->pw_uid, user->pw_name, grp->gr_gid, grp->gr_name);
    
        int number = MAXSIZE;
        check = getgrouplist(user->pw_name, grp->gr_gid, groups, &number);
        if(check == -1)
        {
            printf("Too much groups\n");
            exit(-1);
        }
    
        for(int i = number-1; i >= 0; --i)
        {
            grp = getgrgid(groups[i]);
            if(i==0)
                printf("%d(%s)", groups[i], grp->gr_name);
            else
                printf("%d(%s),", groups[i], grp->gr_name);
        }
        printf("\n");
        break;
    default:
        printf("Wrong arguments\n");
        exit(-1);
    }
    return 0;
}
