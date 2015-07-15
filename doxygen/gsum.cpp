#include "stdio.h"
#include "string.h"

#define VERSION "0.0.2"

///
/// \brief gsum is a utility to caculate the sum of all input
/// \param [in] argc
/// \param [in] argv
/// \param [out] success of fail
///
int main ( int argc, char *argv[ ] )
{
	char str[3] = {0};
	unsigned char hex = 0;	
	unsigned char sum = 0;
	unsigned int in_len = 0;
    unsigned int i;
	
	printf("gsum version %s\n", VERSION);
	printf("Input: ");
	
	if(argc == 1)
	{
		printf("%02X ",hex);
		printf("\nSum  : %02X\n",sum);
		return 0;
	}
	
	in_len = strlen(argv[1]) / 2;	
	for( i = 0; i < in_len; i++)
	{
		str[0] =  argv[1][i*2];
		str[1] =  argv[1][i*2 + 1];
		sscanf(str,"%x",&hex);
		sum += hex;
		printf("%02X ",hex);
	}
	
    printf("\nSum  : %02X\n",sum);
}