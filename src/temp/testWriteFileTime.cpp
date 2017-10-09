#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "utils/TimeUtil.h"


int main(){
	int value =18000;

	unsigned long timein = TimeUtil::convert_us(TimeUtil::getTime());


	for (int i = 0; i < 1; ++i)
	{
		int fd = open("/home/long/trunk/test/test3", O_WRONLY);
	if (fd == -1)
		return 0;


	char buf[10];

	snprintf(buf, 50, "%d", value);


	ssize_t numwrite = write(fd, buf, strlen(buf));
	if (numwrite < 1) {
		close(fd);
		return 0;
	}

	close(fd);
	}

	int fd = open("/home/long/trunk/test/test3", O_RDONLY);
	if (fd == -1)
		return 0;


	char buf2[10];

	// snprintf(buf2, 50, "%d", value);


	ssize_t numwrite = read(fd, buf2, 20);
	if (numwrite < 1) {
		close(fd);
		return 0;
	}

	close(fd);

	int v2 = 0;

	for (int i = 0; i < 10; ++i)
	{
		printf("%c", buf2[i]);
	}
	sscanf(buf2, "%d", &v2);

	printf("\n v2: %d\n", v2);


	
	unsigned long timeout = TimeUtil::convert_us(TimeUtil::getTime());

	printf("total time1: %lu \n", timeout - timein);



	// timein = TimeUtil::convert_us(TimeUtil::getTime());


	// for (int i = 0; i < 100000; ++i)
	// {

	// 	FILE* file = fopen("/home/long/trunk/test/test3", "w");
	// 	if (file == NULL){
	// 		// std::cout << "Failed open file: " << freqInterface[i] << std::endl;
	// 		printf("Failed open file\n");
	// 		exit(1);
	// 	}
	// 	if (fprintf(file, "%d", value) < 0) { 
	// 	// std::cout << "failed to write file" << std::endl;
	// 	} 

	// 	fclose(file);



	// }
	
	// timeout = TimeUtil::convert_us(TimeUtil::getTime());

	// printf("total time2: %lu \n", timeout - timein);



}



