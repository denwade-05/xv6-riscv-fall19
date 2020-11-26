#include <kernel/types.h>
#include <user/user.h>

int main(int argc, char const *argv[])
{
	int numbers[100];
	int fd[2];
	int index = 0;
	for (int i =2;i<=35;i++){
		numbers[index] = i;
		index++;
	}
	while(index>0){
		// printf("while\n");
		pipe(fd);
		if(fork()==0){
			int number = 0;
			index=0;
			int prime = 0;
			int i =0;
			close(fd[1]);
			while(read(fd[0],&number,sizeof(number))!=0){
				if (i==0){
					prime = number;
					printf("prime %d\n",prime);
					i = 1;
				}else{
					if(number % prime != 0){
						numbers[index] = number;
						index++;
					}
				}
			}
		}else{
			for(int i = 0;i<index;i++){
				// printf("%s\n", );
				write(fd[1],&numbers[i],sizeof(numbers[i]));
			}
			// printf("father:%d\n", getpid());
			close(fd[1]);
			wait();
			break;
		}
	}
	// printf("exitpid:%d\n", getpid());
	exit();
	
	return 0;
}

