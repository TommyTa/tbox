/*
 *	crc16.h - CRC-16 routine
 *
 * Implements the standard CRC-16:
 *   Width 16
 *   Poly  0x8005 (x^16 + x^15 + x^2 + 1)
 *   Init  0
 *
 * Copyright (c) 2005 Ben Gardner <bgardner@wabtec.com>
 *
 * This source code is licensed under the GNU General Public License,
 * Version 2. See the file COPYING for more details.
 */

#ifndef __CRC16_H
#define __CRC16_H

extern unsigned short const crc16_table[256];

#ifdef __cplusplus
extern "C" {
#endif

unsigned short crc16(unsigned short crc, const unsigned char *buffer, unsigned int len);

#ifdef __cplusplus
}
#endif

static inline unsigned short crc16_byte(unsigned short crc, const unsigned char data)
{
	return (crc >> 8) ^ crc16_table[(crc ^ data) & 0xff];
}

#endif /* __CRC16_H */
