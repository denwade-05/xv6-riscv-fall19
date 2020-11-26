#include <kernel/types.h>
#include <user/user.h>

int main(int argc, char const *argv[])
{
    int P1[2],P2[2];
    //father write , son read
    pipe(P1);
    //father read , son write
    pipe(P2);
    //mesaage
    int id = fork();
    char mesaage[100];
    if (id == 0)
    {
        write(P2[1],"pong",4);
        close(P2[1]);
        read(P1[0],mesaage,sizeof(mesaage));
        printf("%d: received %s\n", getpid(),mesaage);
    }else{
        write(P1[1],"ping",4);
        close(P1[1]);
        read(P2[0],mesaage,sizeof(mesaage));
    printf("%d: received %s\n", getpid(),mesaage);
    }
    exit();

    return 0;
}
