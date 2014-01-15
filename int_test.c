#include <mk20dx128.h>

#define TIE 2
#define TEN 1

void timer_setup() {
  SIM_SCGC6 |= SIM_SCGC6_PIT;
  PIT_MCR = 0x00;

  PIT_TCTRL2 = TIE;
  NVIC_ENABLE_IRQ(IRQ_PIT_CH2);
}


int main(void){
  // Configure pin 13 for output
  // All of these flags are documented on page 213
  PORTC_PCR5 = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1);
  GPIOC_PDDR |= 1<<5;
  // Configure pin 12 to trigger an interrupt on a rising edge
  // and enable pull-downs.
  // Pin 12 is 7th bit of port c
  PORTC_PCR7 = PORT_PCR_MUX(1) | 2 | (1<<19) | (1<<16); // MUX(1) is gpio, 2 enables pull, low bit is direction, 19th an 16th bit are int on rising.
  GPIOC_PDDR = GPIOC_PDDR & ~(1<<7);
    
  timer_setup();
  NVIC_ENABLE_IRQ(IRQ_PORTC);
  while(1);
}

void portc_isr(void){
  uint32_t isfr = PORTC_ISFR;
  //  PORTC_ISFR ; // 1 clears, not 0. Fix this.
  if(isfr & (1<<7)){
    PORTC_PCR7 |= 1<<24; // Critical - reset interrupt flag!
    GPIOC_PTOR = 1<<5;
    PIT_LDVAL2 = 480000;
    PIT_TCTRL2 |= TEN;  
  }
}

void pit2_isr(void) {
  PIT_TFLG2 = 1;
  PIT_TCTRL2 = PIT_TCTRL2 & ~(TEN);
  GPIOC_PCOR = 1<<5;
}
