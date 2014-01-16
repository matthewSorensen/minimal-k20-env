
#include <usb_serial.h>
#include <mk20dx128.h>
 


char message[] = "Hello, World";

int main(){

  PORTC_PCR5 = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1);
  GPIOC_PDDR |= 1<<5;
   
  int i, j;
  while(1){
    usb_serial_write(message,12);
    for(i = 0; i < 48000000; i++){
      for(j = 0; j <5; j++){
	GPIOC_PSOR = 1<<5;
      }
    }
  }
}
