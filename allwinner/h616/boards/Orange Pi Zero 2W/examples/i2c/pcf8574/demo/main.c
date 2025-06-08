// gcc lcd1602.c -lwiringPi -lwiringPiDev -lwiringPiLCD101rpi -o lcd1602

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <lcd1602pi.h>

int lcdHandle;
int lcdAddress = 0x20;
 
int usage(char **args) {
  fprintf(stderr,"usage: %s [ -a hex_address ]\n", args[0]);
  return 0;
}

int commandLineOptions(int argc, char **args) {
	int c;


	while ((c = getopt(argc, args, "a:")) != -1)
		switch (c) {
		case 'a':
      if (strncmp(optarg,"0x",2)==0) {
  			sscanf(&optarg[2], "%x", &lcdAddress);
      } else {
  			sscanf(optarg, "%x", &lcdAddress);
      }
			break;
		case '?':
			if (optopt == 'a')
				fprintf(stderr, "Option -%c requires an argument.\n", optopt);
			else if (isprint(optopt))
				fprintf(stderr, "Unknown option `-%c'.\n", optopt);
			else
				fprintf(stderr, "Unknown option character \\x%x.\n", optopt);

			return usage(args);

		default:
			abort();
		}


	//	for (index = optind; index < argc; index++)
	//		printf("Non-option argument %s\n", args[index]);
	return 1;
}

 
int main (int argc, char **args)
{
  int rs;
  printf ("Raspberry Pi LCD test program\n") ;

	if (!commandLineOptions(argc, args)) {
		return 1;
	}
  
 
  rs = wiringPiSetup ();
  if (rs != 0) {
    fprintf (stderr, ": Unable to initialise wiringPi: %s\n", strerror (errno));
    return 1;
  }
 
  lcdHandle=lcdSetup(lcdAddress);
  if (lcdHandle < 0) {  
    fprintf(stderr, "lcdInit failed\n");  
    return 2; 
  }
 
//Position cursor on the first line in the first column 
  lcdPosition(lcdHandle, 0, 0);
  lcdPuts(lcdHandle, "Hello World!!!");
  
  lcdPosition(lcdHandle, 0, 1);
  lcdPuts(lcdHandle, "line 2");
}
 
