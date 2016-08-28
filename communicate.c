#ifdef RaspberryPi
#include <stdio.h> //for printf
#include <stdint.h> //uint8_t definitions
#include <stdlib.h> //for exit(int);
#include <string.h> //for errno
#include <errno.h> //error output
#include <wiringPi.h>
#include <wiringSerial.h>

int fd;
char device[]= "/dev/ttyACM0";
unsigned long baud = 9600;
unsigned long time=0;


int main(void);
;void loop(void);
void setup(void);

void setup(){

      printf("%s \n", "Raspberry Startup!");
      fflush(stdout);

 if ((fd = serialOpen (device, baud)) < 0){
 fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
 exit(1);
 }

 if (wiringPiSetup () == -1){
 fprintf (stdout, "Unable to start wiringPi: %s\n", strerror (errno)) ;
 exit(1);
 }
}

void loop(){

        if(serialDataAvail (fd)){
                char newChar = serialGetchar(fd);
                    printf("%c", newChar);
                    
                          }
}

int main(){
      setup();
      while(1) loop();
          return 0;
}
#endif
