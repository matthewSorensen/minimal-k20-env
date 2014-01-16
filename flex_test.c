#include <mk20dx128.h>
#include <flexram.h>
#include <stdint.h>

int main(void){
  uint32_t array[10];
  int i;

  PORTC_PCR5 = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1);
  GPIOC_PDDR |= 1<<5;

  for(i = 0; i < 10; i++){
    array[i] = i;
  }

  write_with_checksum(&array,0,10);
  if(read_with_checksum(&array,0,10)){
    GPIOC_PSOR = 1<<5;
  }
  while(1);
}
