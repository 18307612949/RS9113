#include "onebox_datatypes.h"


static uint32 checksum_addition(uint8 *buf, uint32 size, uint32 prev_sum)
{
	uint32 sum = prev_sum;
	uint32 cnt;
	uint32 cnt_limit;
	uint32 dword;

	if (size == 0)
	{
 		return sum;
	}

	cnt_limit = (size & (~0x3));
	/* Accumulate checksum */
	for (cnt = 0; cnt < cnt_limit; cnt += 4)
	{
		dword = *(uint32 *) &buf[cnt];
		sum += dword;
		if(sum < dword)
		{
			/* In addition operation, if result is lesser than any one of the operand
			 * it means carry is generated. 
			 * Incrementing the sum to get ones compliment addition */

			sum++;
		}
	}


	/* Handle non dword-sized case */
  if(size & 0x3) {
		dword = 0xffffffff;
		dword = ~(dword << (8 * (size & 0x3)));
		/* Keeping only valid bytes and making upper bytes zeroes. */
		dword = (*(uint32 *) &buf[cnt]) & dword;
		sum += dword;
		if(sum < dword)
		{
			sum++;
		}
	}

	return sum;
}

uint32 checksum_32bit(uint8 **scatter_addr, uint32 *scatter_len, uint32 no_of_scatters)
{
	uint32 sum = 0;
	uint32 cnt;
	uint8 *buf;
	uint32 size;

	for (cnt = 0; cnt < no_of_scatters; cnt++)
	{
		buf  = scatter_addr[cnt];
		size = scatter_len[cnt];
		sum = checksum_addition(buf, size, sum);
	}
	
	/* Invert to get the negative in ones-complement arithmetic */
	return ~sum;
}

