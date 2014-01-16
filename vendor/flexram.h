#ifndef flexram_h
#define flexram_h

void initialize_flexram(void);
void write_with_checksum(uint32_t*, uint32_t, uint32_t);
uint32_t read_with_checksum(uint32_t*,uint32_t,uint32_t);

#endif
