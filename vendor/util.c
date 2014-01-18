#include "util.h"
void delay(uint32_t ms)
{
	uint32_t start = micros();

	if (ms > 0) {
		while (1) {
			if ((micros() - start) >= 1000) {
				ms--;
				if (ms == 0) return;
				start += 1000;
			}
			yield();
		}
	}
}

