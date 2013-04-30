MEMORY
{
	rom (rx) : ORIGIN = 0x080e0000, LENGTH = 128K
	ram (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
}
INCLUDE libopencm3_stm32f4.ld
