/*
 * Copyright (c) 2012 Sander van der Burg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef __IFF_IFFTYPES_H
#define __IFF_IFFTYPES_H

#define IFF_ID_SIZE 4

/** A signed byte */
typedef char IFF_Byte;

/** An unsigned byte */
typedef unsigned char IFF_UByte;

/** A 16-bit signed type */
typedef short IFF_Word;

/** A 16-bit unsigned type */
typedef unsigned short IFF_UWord;

/** A 32-bit signed type */
typedef int IFF_Long;

/** A 32-bit unsigned type */
typedef unsigned int IFF_ULong;

/** A 4 byte ID type */
typedef char IFF_ID[IFF_ID_SIZE];

#define TRUE 1
#define FALSE 0

//#define IFF_BIG_ENDIAN @IFF_BIG_ENDIAN@

#endif
