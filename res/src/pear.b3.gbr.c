#pragma bank 3
unsigned char bank_pear = 3;
const unsigned char pear_tiles[] = {
	//Frame 0
	0x01,0x01,0x03,0x03,0x07,0x07,0x07,0x04,
	0x0f,0x0c,0x0f,0x08,0x1f,0x18,0x1b,0x14,
	0x37,0x38,0x6b,0x74,0x57,0x68,0x4b,0x74,
	0x57,0x68,0x6a,0x75,0x35,0x3a,0x1f,0x1f,
	0xe0,0xe0,0x70,0xb0,0x38,0xd8,0x98,0xe8,
	0xcc,0xfc,0xfc,0x3c,0xf8,0x18,0xfc,0x0c,
	0xfc,0x04,0xfe,0x06,0xfe,0x02,0xfe,0x02,
	0xfe,0x06,0xfc,0x04,0x7c,0x9c,0xf0,0xf0,
	
	//Frame 1
	0x07,0x07,0x0d,0x0e,0x19,0x1e,0x13,0x1d,
	0x37,0x3b,0x3f,0x3c,0x1b,0x1c,0x37,0x38,
	0x2b,0x34,0x77,0x68,0x6b,0x54,0x57,0x68,
	0x6b,0x74,0x35,0x2a,0x3a,0x3d,0x0f,0x0f,
	0x80,0x80,0xc0,0xc0,0xe0,0xe0,0xe0,0x20,
	0xf0,0x30,0xf0,0x10,0xf8,0x18,0xf8,0x08,
	0xfc,0x0c,0xfe,0x06,0xfe,0x02,0xfe,0x02,
	0xfe,0x02,0xfe,0x06,0xfc,0x0c,0xf8,0xf8,
	
	//Frame 2
	0x0f,0x0f,0x1b,0x1d,0x37,0x3f,0x27,0x3c,
	0x6f,0x7c,0x7f,0x78,0x1f,0x18,0x1b,0x14,
	0x37,0x38,0x6b,0x74,0x57,0x68,0x4b,0x74,
	0x57,0x68,0x6a,0x75,0x35,0x3a,0x1f,0x1f,
	0x00,0x00,0x80,0x80,0xc0,0xc0,0xe0,0x60,
	0xf0,0x10,0xf8,0x18,0xf8,0x18,0xfc,0x0c,
	0xfc,0x04,0xfe,0x06,0xfe,0x02,0xfe,0x02,
	0xfe,0x06,0xfc,0x04,0x7c,0x9c,0xf0,0xf0,
	
	//Frame 3
	0x01,0x01,0x01,0x01,0x03,0x03,0x07,0x06,
	0x0f,0x0c,0x0f,0x0c,0x1b,0x1c,0x37,0x38,
	0x2b,0x34,0x77,0x68,0x6b,0x54,0x57,0x68,
	0x6b,0x74,0x35,0x2a,0x3a,0x3d,0x0f,0x0f,
	0xe0,0xe0,0x70,0xb0,0xf8,0xf8,0xf8,0x28,
	0xfc,0x3c,0xfc,0x1c,0xf8,0x18,0xf8,0x08,
	0xfc,0x0c,0xfe,0x06,0xfe,0x02,0xfe,0x02,
	0xfe,0x02,0xfe,0x06,0xfc,0x0c,0xf8,0xf8
};

#include "TilesInfo.h"
const struct TilesInfo pear = {
	16, //width
	16, //height
	4, //num_tiles
	pear_tiles, //tiles
	0, //CGB palette
};