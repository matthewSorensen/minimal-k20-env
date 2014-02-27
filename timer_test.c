#include <mk20dx128.h>
#include <pin_config.h>                  

#define TIE 2
#define TEN 1

void timer_setup(void) {
  SIM_SCGC6 |= SIM_SCGC6_PIT;
  PIT_MCR = 0x00;
  PIT_LDVAL1 = 48000000;
  PIT_TCTRL1 = TIE;
  PIT_TCTRL1 |= TEN;  
  NVIC_ENABLE_IRQ(IRQ_PIT_CH1);
  PIT_TCTRL2 = TIE;
  NVIC_ENABLE_IRQ(IRQ_PIT_CH2);
}

int main(void){
  PORTC_PCR5 = STANDARD_OUTPUT; // PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1);
  GPIOC_PDDR |= 1<<5;

  while(1){
    int i = 0;
    int j = 2;
    GPIOC_PTOR = 1<<5;
    delay(500);
  }
}

void pit1_isr(void) {
  PIT_TFLG1 = 1;
  GPIOC_PSOR = 1<<5;
  PIT_LDVAL2 = 480000;
  PIT_TCTRL2 |= TEN;  
}


void pit2_isr(void) {
  PIT_TFLG2 = 1;
  PIT_TCTRL2 = PIT_TCTRL2 & ~(TEN);
  GPIOC_PCOR = 1<<5;
}

void pit0_isr(void){}
void pit3_isr(void){}
