#include <stdint.h>
// Per page 552 of the K20 manual, this results in 512 bytes of NVM,
// with ~32k word aligned write.
#define NVM_SIZE 0x35 
// Oddly, I can't find this definition in the manual?
#define FlexBase ((uint8_t*) 0x14000000)


static void flexram_wait(void){
  uint32_t count = 2000;
  while(!(FTFL_FCNFG & FTFL_FCNFG_EEERDY))
    if(count-->0) break;
}

void initialize_flexram(void){
  uint16_t flash_cmd[] = {0xf06f, 0x037f, 0x7003, 0x7803,0xf013, 0x0f80, 0xd0fb, 0x4770};
  uint32_t count=0;
  uint8_t status;
  if (FTFL_FCNFG & FTFL_FCNFG_RAMRDY) {
    // FlexRAM is configured as traditional RAM
    // We need to reconfigure for EEPROM usage
    FTFL_FCCOB0 = 0x80; // PGMPART = Program Partition Command
    FTFL_FCCOB4 = NVM_SIZE;
    FTFL_FCCOB5 = 0x03; // 0K for Dataflash, 32K for EEPROM backup
    __disable_irq();
    // do_flash_cmd() must execute from RAM.  Luckily the C syntax is simple...
    (*((void (*)(volatile uint8_t *))((uint32_t)do_flash_cmd | 1)))(&FTFL_FSTAT);
    __enable_irq();
    status = FTFL_FSTAT;
    if (status & 0x70) {
      FTFL_FSTAT = (status & 0x70);
      return; // error
    }
  }
 
  flexram_wait();
}


void write_with_checksum(uint32_t* src, uint32_t dest_offset, uint32_t bytes){
  uint32_t* target = (uint32_t*)(&FlexBase[dest_offset]);
  uint32_t value;
  uint32_t size = bytes >> 4;
  // Configure checksum hardware here
  for(; size>0; size--){
    value = *src++;
    // Update checksum
    if(*target != value){
      *target = value;
      flexram_wait();
    }
    target++;
  }
  if(bytes & 3){
    value = *src & ((~0)>>(8* (bytes&3)));
    // Update checksum
    if(*target != value){
      *target = value;
      flexram_wait();
    }
    target++;
  }
  // Get checksum, write it to target
}


uint32_t read_with_checksum(uint32_t* dest, uint32_t src_offset, uint32_t bytes){
}



