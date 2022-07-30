/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 *****************************************************************************
 * @file errorno.h
 * @author tanxt
 * @version v1.0.0
 ****************************************************************************/

#ifndef _ERRORNO_H_
#define _ERRORNO_H_

#define RETVAL(errno)           (- errno)

#define E_OK                    0   /* Success */
#define E_FAIL                  1   /* Failure */
#define E_NULL                  2   /* Invalid argument */
#define E_BUSY                  3   /* Device or resource busy */
#define E_STATE                 4   /* Unexpected  */
#define E_TIMEOUT               5   /* Timeout */
#define E_BUS                   6   /* Bus error */
#define E_SEND                  7   /* Send failure */
#define E_RECV                  8   /* Receive failure */
#define E_OPEN                  9   /* Open failure */
#define E_CLOSE                 10  /* Close failure */
#define E_GET                   11  /* Get failure */
#define E_SET                   12  /* Set failure */
#define E_RANGE                 13  /* Number out of range */
#define E_FULL                  14  /* Full */
#define E_EMPTY                 15  /* Empty */
#define E_ACCESS                16  /* Permission denied */
#define E_LOCKED                17  /* Device or resource is locked */
#define E_COMM                  18  /* Communication error */
#define E_PROTO                 19  /* Protocol error */
#define E_MSG                   20  /* Bad message */
#define E_CRC                   21  /* checksum error */
#define E_SYNC                  22  /* Synchronization failure */
#define E_ALLOC                 23  /* Allocation failure */
#define E_FRAME                 24  /* Error frame */
#define E_FORMAT                25  /* format error */
#define E_OVERLOAD              26  /* Overload */
#define E_NOT_SUPPORT           27  /* Not support */
#define E_NOT_EMPTY             28  /* Not empty */
#define E_NOT_CONN              29  /* Not connected */
#define E_INVAL_PARM            30  /* Invalid parameter */
#define E_INVAL_DATA            31  /* Invalid data */
#define E_INVAL_LEN             32  /* Invalid length */
#define E_INVAL_ADDR            33  /* Invalid address */
#define E_INVAL_CMD             34  /* Invalid command */
#define E_NO_DEV                35  /* No such device */
#define E_NO_SPACE              36  /* No Space */
#define E_NO_MEM                37  /* No memory */

#endif // _ERRORNO_H_
