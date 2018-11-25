//******************************************************************************************
//!
//! \file   FIFO.c
//! \brief  Genernal FIFO Model Interface.
//!         You can use uniform FIFO Model to manager Any type of data element.
//! \author cedar
//! \date   2013-12-16
//! \email  xuesong5825718@gmail.com
//!
//! \license
//!
//! Copyright (c) 2013 Cedar MIT License
//!
//! Permission is hereby granted, free of charge, to any person obtaining a copy
//! of this software and associated documentation files (the "Software"), to deal
//! in the Software without restriction, including without limitation the rights to
//! use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
//! the Software, and to permit persons to whom the Software is furnished to do so,
//! subject to the following conditions:
//!
//! The above copyright notice and this permission notice shall be included in all
//! copies or substantial portions of the Software.
//!
//! THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//! IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//! FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//! AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//! LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//! OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
//! IN THE SOFTWARE.
///
//******************************************************************************************
#include "includes.h"
#include "FIFO.h"

//******************************************************************************************
//!                     ASSERT MACRO
//******************************************************************************************
#ifndef ASSERT

#ifdef  NDEBUG
#define ASSERT(x)
#else
#define ASSERT(x) do {while(!(x));} while(0)
#endif

#endif  // ASSERT

#ifdef USE_DYNAMIC_MEMORY
//******************************************************************************************
//
//! \brief  Create An New FIFO Instance.
//! This function allocate enought room for N blocks fifo elements, then return the pointer
//! of FIFO.
//!
//! \param  [in] UnitSize is fifo element size.
//! \param  [in] UnitCnt is count of fifo elements.
//! \retval The Pointer of FIFO instance, return NULL is failure to allocate memory.
//!
//! \note   -# You must enable USE_MEMORY_ALLOC macro and ensure your system have <stdlib.h>
//!            Header file before use this function.
//! \note   -# Functions FIFO_Create and FIFO_Destory must be used in pairs.
//!
//******************************************************************************************
FIFO_t* FIFO_Create(uint8_t UnitSize, uint32_t UnitCnt)
{
    //! Check input parameters.
    ASSERT(0 != UnitSize);
    ASSERT(0 != UnitCnt);

    FIFO_t*   pFIFO     = NULL;         //!< FIFO Pointer
    uint8_t*  pBaseAddr = NULL;         //!< Memory Base Address

    //! Allocate Memory for pointer of new FIFO Control Block.
    pFIFO = (FIFO_t*) malloc(sizeof(FIFO_t));
    if(NULL == pFIFO)
    {
        //! Allocate Failure, exit now.
        return (NULL);
    }

    //! Allocate memory for FIFO.
    pBaseAddr = malloc(UnitSize*UnitCnt);
    if(NULL == pBaseAddr)
    {
        //! Allocate Failure, exit now.
        return (NULL);
    }

    //! Initialize General FIFO Module.
    FIFO_Init(pFIFO, pBaseAddr, UnitSize, UnitCnt);

    return (pFIFO);
}

//******************************************************************************************
//
//! \brief  Destory FIFO
//!  This function first release memory section, then reinit the fifo pointer parameter.
//!
//! \param  [in] pFIFO is the pointer of valid fifo.
//! \retval None.
//!
//! \note   -# You must enable USE_MEMORY_ALLOC macro and ensure your system have <stdlib.h>
//!            Header file before use this function.
//
//******************************************************************************************
void FIFO_Destory(FIFO_t* pFIFO)
{
    //! Check input parameters.
    ASSERT(NULL != pFIFO);
    ASSERT(NULL != pFIFO->pStartAddr);

    //! Free FIFO memory
    free(pFIFO->pStartAddr);

#if 0
    //! Reset FIFO parameters
    pFIFO->pStartAddr  = NULL;
    pFIFO->pEndAddr    = NULL;
    pFIFO->pReadIndex  = NULL;
    pFIFO->pWriteIndex = NULL;
    pFIFO->UnitSize    = 0;
    pFIFO->Count       = 0;
#endif

    //! Free FIFO Control Block memory.
    free(pFIFO);

    return;     //!< Success
}

//******************************************************************************************
//
//! \brief  Create An New FIFO Instance(in Single Mode).
//! This function allocate enought room for N blocks fifo elements, then return the pointer
//! of FIFO.
//!
//! \param  [in] UnitCnt is count of fifo elements.
//! \retval The Pointer of FIFO instance, return NULL is failure to allocate memory.
//!
//! \note   -# You must enable USE_MEMORY_ALLOC macro and ensure your system have <stdlib.h>
//!            Header file before use this function.
//! \note   -# Functions FIFO_Create and FIFO_Destory must be used in pairs.
//!
//******************************************************************************************
FIFO_S_t* FIFO_S_Create(uint32_t UnitCnt)
{
    FIFO_S_t* pFIFO     = NULL;         //!< FIFO Pointer
    uint8_t*  pBaseAddr = NULL;         //!< Memory Base Address

    //! Check input parameters.
    ASSERT(0 != UnitCnt);

    //! Allocate Memory for pointer of new FIFO Control Block.
    pFIFO = (FIFO_S_t*) malloc(sizeof(FIFO_S_t));
    if(NULL == pFIFO)
    {
        //! Allocate Failure, exit now.
        return (NULL);
    }

    //! Allocate memory for FIFO.
    pBaseAddr = malloc(UnitCnt);
    if(NULL == pBaseAddr)
    {
        //! Allocate Failure, exit now.
        return (NULL);
    }

    //! Initialize General FIFO Module.
    FIFO_S_Init(pFIFO, pBaseAddr, UnitCnt);

    return (pFIFO);
}

//******************************************************************************************
//
//! \brief  Destory FIFO Instance(in Single Mode).
//!  This function release memory, then reinit the FIFO struct.
//!
//! \param  [in] pFIFO is the pointer of FIFO instance
//! \retval None.
//!
//! \note   -# You must enable USE_MEMORY_ALLOC macro and ensure your system have <stdlib.h>
//!            Header file before use this function.
//
//******************************************************************************************
void FIFO_S_Destory(FIFO_S_t* pFIFO)
{
    //! Check input parameters.
    ASSERT(NULL != pFIFO);
    ASSERT(NULL != pFIFO->pStartAddr);

    //! Free FIFO memory
    free(pFIFO->pStartAddr);

#if 0
    //! Reset FIFO parameters
    pFIFO->pStartAddr  = NULL;
    pFIFO->pEndAddr    = NULL;
    pFIFO->pReadIndex  = NULL;
    pFIFO->pWriteIndex = NULL;
    pFIFO->UnitSize    = 0;
    pFIFO->Count       = 0;
#endif

    //! Free FIFO Control Block memory.
    free(pFIFO);

    return;     //!< Success
}

#endif // USE_DYNAMIC_MEMORY


//******************************************************************************************
//
//! \brief  Initialize an static FIFO struct(in single mode).
//!
//! \param  [in] pFIFO is the pointer of valid FIFO instance.
//! \param  [in] pBaseAddr is the base address of pre-allocate memory, such as array.
//! \param  [in] UnitCnt is count of fifo elements.
//! \retval 0 if initialize successfully, otherwise return -1.
//
//******************************************************************************************
int FIFO_S_Init(FIFO_S_t* pFIFO, void* pBaseAddr, uint32_t UnitCnt)
{
    //! Check input parameters.
    memset(pFIFO, 0, sizeof(FIFO_S_t));
    pFIFO->pStartAddr = pBaseAddr;
    pFIFO->buffSize = UnitCnt;
	
    return CL_OK;
}


//******************************************************************************************
//
//! \brief  Put an element into FIFO(in single mode).
//!
//! \param  [in]  pFIFO is the pointer of valid FIFO.
//! \param  [in]  Element is the data element you want to put
//!
//! \retval 0 if operate successfully, otherwise return -1.
//
//******************************************************************************************
int FIFO_S_Put(FIFO_S_t* pFIFO, uint8_t Element)
{
    uint32_t datalen = 0;

    if (pFIFO->writeIndex >= pFIFO->readIndex) 
	{
        datalen = pFIFO->writeIndex - pFIFO->readIndex + 1;
    }
	else
	{
        datalen = pFIFO->writeIndex + pFIFO->buffSize - pFIFO->readIndex + 1;
    }

    if (pFIFO->buffSize > datalen) 
	{
        pFIFO->pStartAddr[pFIFO->writeIndex++] = Element;
        pFIFO->usedLen++;
        if (pFIFO->buffSize <= pFIFO->writeIndex) 
		{
            pFIFO->writeIndex = 0;
        }
        return 0;
    }
	
    return CL_FAIL;
}


//******************************************************************************************
//
//! \brief  Get an element from FIFO(in single mode).
//!
//! \param  [in]  pFIFO is the pointer of valid FIFO.
//!
//!
//
//******************************************************************************************
int FIFO_S_Get(FIFO_S_t* pFIFO, uint8_t *pData)
{
//	printf("writeIndex= [%d], readIndex= [%d] \r\n", pFIFO->writeIndex, pFIFO->readIndex);
	
    if (pFIFO->writeIndex != pFIFO->readIndex) 
    {
        *pData = pFIFO->pStartAddr[pFIFO->readIndex++];
        if (pFIFO->buffSize <= pFIFO->readIndex) 
        {
            pFIFO->readIndex = 0;
        }
        if (pFIFO->usedLen) 
        {
            pFIFO->usedLen--;
        }
        return CL_OK;
    }
    return CL_FAIL;
}


//******************************************************************************************
//
//! \brief  FIFO is empty (in single mode)?
//!
//! \param  [in] pFIFO is the pointer of valid FIFO.
//!
//! \retval - None-zero(true) if empty.
//!         - Zero(false) if not empty.
//
//******************************************************************************************
int FIFO_S_IsEmpty(FIFO_S_t* pFIFO)
{
    //! Check input parameter.
    ASSERT(NULL != pFIFO);
    return (pFIFO->writeIndex == pFIFO->readIndex) ? CL_TRUE : CL_FALSE;
}


//******************************************************************************************
//
//! \brief  Get FIFO the number of elements(in single mode)?
//!
//! \param  [in] pFIFO is the pointer of valid FIFO.
//!
//! \retval The number of elements in FIFO.
//
//******************************************************************************************
int FIFO_S_CountUsed(FIFO_S_t* pFIFO)
{
    //! Check input parameter.
    ASSERT(NULL != pFIFO);
    return (pFIFO->usedLen);
}


//******************************************************************************************
//
//! \brief  Flush the content of FIFO.
//!
//! \param  [in] pFIFO is the pointer of valid FIFO.
//!
//! \retval 0 if success, -1 if failure.
//
//******************************************************************************************
int FIFO_S_Flush(FIFO_S_t* pFIFO)
{
    //! Check input parameters.
    ASSERT(NULL != pFIFO);
    //! Initialize FIFO Control Block.
    pFIFO->usedLen = 0;
    pFIFO->readIndex = 0;
    pFIFO->writeIndex = 0;
	
    return (0);
}




