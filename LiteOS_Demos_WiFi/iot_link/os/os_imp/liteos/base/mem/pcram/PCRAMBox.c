#include "los_typedef.h"
#include "los_hwi.h"
#include "los_membox.ph"
#include "crc.h"
#include "pcrambox.h"
#include "los_task.h"
#if (LOSCFG_PLATFORM_EXC == YES)
#include "los_memcheck.ph"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * The address of the static memory pool must be aligned to the boundary of 4.
 */
#define OS_BOXMEM_BASE_ALIGN  4
#define IS_BOXMEM_ALIGNED(value, alignSize)  (0 == (((UINT32)(value)) & ((UINT32)(alignSize - 1))))

/**
 * Get the address of the next memory node in the static memory pool.
 */
#define OS_pcrambox_NODE_NEXT(addr, uwBlkSize)  (LOS_PCRAMBOX_NODE *)((UINT8 *)(addr) + (uwBlkSize))

/**
 * The magic word of the memory box.
 */
#ifdef LOS_pcrambox_MAGIC_CHECK
#define OS_pcrambox_MAGIC              0xFFAA
#define OS_pcrambox_SET_MAGIC(addr)    *((UINT16 *)(addr)) = OS_pcrambox_MAGIC
#define OS_pcrambox_CHECK_MAGIC(addr)  ((*((UINT16 *)(addr)) == OS_pcrambox_MAGIC) ? LOS_OK: LOS_NOK)
#else
#define OS_pcrambox_SET_MAGIC(addr)
#define OS_pcrambox_CHECK_MAGIC(addr)  LOS_OK
#endif

/**
 * Get the address of memory block according to the magic word information.
 */

#define OS_SCLIST_VIB_ADDR(addr)     ((VIB *)((UINT32 *)(addr) + SCLIST_TVCB_STRUCT_SIZE))
#define OS_pcrambox_USER_ADDR(addr)  ((VOID *)((UINT32 *)(addr) + LOS_pcrambox_MAGIC_SIZE))
#define OS_pcrambox_NODE_ADDR(addr)  ((LOS_pcrambox_NODE *)((UINT32 *)(addr) - LOS_pcrambox_MAGIC_SIZE))
#define TIDEqual(TI1, TI2)  (((TI1.TID != TI2.TID)||(TI1.Identifier != TI2.Identifier)) ? LOS_NOK : LOS_OK)
#define TVCBOverflow(addr)           ((g_TVBHead + g_TVCBSize * g_TVCBNum) < addr ? LOS_NOK : LOS_OK)
VOID *g_TVBHead = NULL;
UINT32 g_TVCBSize;                       //初始化里进行赋值操作
UINT32 g_VIBSize;                       //读写的时候转成8位拼起来
UINT16 g_TVCBNum;
/*****************************************************************************
 Function : osCheckBoxMem
 Description : Check whether the memory block is valid
 Input       : pstBoxInfo  --- Pointer to the memory pool
               pstNode     --- Pointer to the memory block that will be checked
 Output      : None
 Return      : LOS_OK - OK, LOS_NOK - Error
*****************************************************************************/
LITE_OS_SEC_TEXT static INLINE UINT32 osCheckBoxMem(const LOS_PCRAMBOX_INFO *pstBoxInfo, const VOID *pstNode)
{
    UINT32 uwOffSet;

    if (pstBoxInfo->uwBlkSize == 0)
    {
        return LOS_NOK;
    }

    uwOffSet = (UINT32)pstNode - (UINT32)(pstBoxInfo + 1);
    if ((uwOffSet % pstBoxInfo->uwBlkSize) != 0)
    {
        return LOS_NOK;
    }

    if ((uwOffSet / pstBoxInfo->uwBlkSize) >= pstBoxInfo->uwBlkNum)
    {
        return LOS_NOK;
    }

    return OS_pcrambox_CHECK_MAGIC(pstNode);
}


/*****************************************************************************
 Function : osFoundVIB
 Description : Found the target VIB
 Input       : TID, Identifier 
 Output      : None
 Return      : VIB  --- Pointer to the VIB
*****************************************************************************/
LITE_OS_SEC_TEXT VOID *osFoundVIB(UINT8 TID, UINT8 Identifier)
{
    TVCB *curTVCB = (TVCB *)g_TVBHead;
    VIB *curVIB = NULL;
    TIdentifier curVar;
    UINT8 TVCBCount = 0;
    curVar.TID = TID;
    curVar.Identifier = Identifier;
    while((curTVCB->varNum == 0) ||  (curTVCB->TID != TID))
    {
        curTVCB += g_TVCBSize;
        if(TVCBOverflow(curTVCB) == LOS_NOK)
        {
            return NULL;
        }
    }
    if(curTVCB->TID == TID)
    {
        curVIB = OS_SCLIST_VIB_ADDR(curTVCB);                        /*skip the length of the parameter varNum*/
        while(((TIDEqual(curVIB->varName, curVar)) == LOS_NOK) && (curVIB < curTVCB + g_TVCBSize))
        {
            curVIB += g_VIBSize;
        }
        if(TIDEqual(curVIB->varName, curVar) == LOS_OK)
        {
            return curVIB;
        }
    }
    return NULL;
}

/*****************************************************************************
 Function : osCreateVIB
 Description : Create the target VIB
 Input       : *pRet, Identifier, BlkNum, 
 Output      : None
 Return      : LOS_OK - OK, LOS_NOK - Error
*****************************************************************************/
LITE_OS_SEC_TEXT UINT32 osCreateVIB(VOID *pRet, UINT8 curTID, UINT8 Identifier, UINT16 BlkNum)
{
    TVCB *curTVCB = (struct TVCB *)g_TVBHead;
    VIB *curVIB = NULL;
    TIdentifier curVar;
    UINT8 TVCBCount = 0;
    VIB *curVIBBackup = NULL;
    curVar.TID = curTID;
    curVar.Identifier = Identifier;
    while(((curTVCB->TID != curTID) && (curTVCB->varNum != 0)) && (TVCBCount <= g_TVCBNum))
    {
        curTVCB = curVIB + g_TVCBSize;
        TVCBCount++;
    }
    if(TVCBCount > g_TVCBNum)
    {
        return LOS_NOK;
    }
    curVIB = OS_SCLIST_VIB_ADDR(curTVCB); 
    if(curTVCB->varNum == 0)
    {
        curTVCB->TID = curTID;
    }
    else
    {
        if(curTVCB->varNum * g_VIBSize > g_TVCBSize)
        {
            return LOS_NOK;
        }
        UINT8 varCount = 1;
        while((TIDEqual(curVIB->varName, curVar) == LOS_NOK) && (curVIB->addrStart != NULL))
        {
            curVIB += g_VIBSize;
            varCount++;
            if(varCount >= curTVCB->varNum)
            {
                break;
            }
        }
    }
    curVIB->addrStart = pRet;
    curVIB->varBlkNum = BlkNum;
    curVIB->varName = curVar;
    curVIB->varCRC = calc_crc16(0, &curVIB, 6);
    curTVCB->varNum++;
    curVIBBackup = curVIB + g_TVCBSize * g_TVCBNum;
    return LOS_OK;
}

/*****************************************************************************
 Function : osDeleteVIB
 Description : Delete the target VIB
 Input       : *pRet
 Output      : None
 Return      : LOS_OK - OK, LOS_NOK - Error
*****************************************************************************/
LITE_OS_SEC_TEXT UINT32 osDeleteVIB(VOID *pRet)
{
    UINT32 uwRet = LOS_NOK;
    VIB *curVIB = (VIB *) pRet;
    curVIB->addrStart = NULL;
    curVIB->varBlkNum = 0;
    curVIB->varCRC = 0;
   ((TVCB *)(curVIB->fromTVCB))->varNum --;
    return LOS_OK;
}

/*****************************************************************************
 Function : osCheckCRC
 Description : Check whether the CRC is valid
 Input       : VIB  --- Pointer to the VIB
 Output      : None
 Return      : LOS_OK - OK, LOS_NOK - Error
*****************************************************************************/
LITE_OS_SEC_TEXT INLINE UINT32 osCheckCRC(VIB VIBInfo)
{
    UINT16 curCRC;
    curCRC = calc_crc16(0,&VIBInfo,160);
    if (VIBInfo.varCRC == curCRC)
    {
        return LOS_OK;
    }
    else
    {
        return LOS_NOK;
    }
}

/*****************************************************************************
 Function : osIndexInit
 Description : Init index area
 Input       : None
 Output      : None
 Return      : LOS_OK - OK, LOS_NOK - Error
*****************************************************************************/
LITE_OS_SEC_TEXT UINT32 osIndexInit(VOID)
{
    TVCB *curTVCB = (TVCB *)g_TVBHead;
    VIB *curVIB = NULL;
    UINT32 TVBLength = g_TVCBNum * g_TVCBSize;
    while(curTVCB < g_TVBHead + TVBLength)
    {
        curTVCB->varNum = 0;
        curVIB = OS_SCLIST_VIB_ADDR(curTVCB);
        curTVCB += g_TVCBSize;
        do
        {
            curVIB->addrStart = NULL;
            curVIB->varBlkNum = 0;
            curVIB->varCRC = 0;
            curVIB->fromTVCB = (VOID *)curTVCB;
            curVIB += g_VIBSize;
        } while (curVIB < curTVCB);
    }
    return LOS_OK;
}

/*****************************************************************************
 Function : LOS_pcramboxInit
 Description : Initialize Static Memory pool
 Input       : pBoxMem    --- Pointer to the memory pool
               uwBoxSize  --- Size of the memory pool
               uwBlkSize  --- Size of the memory block
 Output      : None
 Return      : LOS_OK - OK, LOS_NOK - Error
*****************************************************************************/
LITE_OS_SEC_TEXT_INIT UINT32 LOS_pcramboxInit(VOID *pBoxMem, UINT32 uwBoxSize, UINT16 uwBlkSize)
{
    LOS_PCRAMBOX_INFO *pstBoxInfo = (LOS_PCRAMBOX_INFO *)pBoxMem;
    
    LOS_PCRAMBOX_NODE *pstNode = NULL;
    UINT16 i;
    UINTPTR uvIntSave;
    UINT32 uwRet = LOS_NOK;
    //if((UINT32 *)pBoxMem == 0xFFAAFFAA)
    g_TVBHead = (LOS_PCRAMBOX_NODE *)(pstBoxInfo + sizeof(LOS_PCRAMBOX_INFO));
    g_TVCBNum = 20;
    g_VIBSize = sizeof(VIB);
    g_TVCBSize = LOS_pcrambox_ALIGNED(g_VIBSize * 20);
    if(pstBoxInfo->flag == 0xFFAA)
    {
        printf("already init\n");
        printf("current free node addr begins from:%p\n",(UINT32)pstBoxInfo->curFreeNode);
        return LOS_OK;
    }
    if (pBoxMem == NULL || uwBlkSize == 0 || uwBoxSize < sizeof(LOS_PCRAMBOX_INFO))
    {
        return LOS_NOK;
    }

    if (!IS_BOXMEM_ALIGNED(pBoxMem, OS_BOXMEM_BASE_ALIGN))
    {
        return LOS_NOK;
    }

    uvIntSave = LOS_IntLock();
    /*
     * The node size is aligned to the next 4 boundary.
     * Memory that is not enough for one node size in the memory pool will be ignored.
     */
    uwRet = osIndexInit();
    if(uwRet != LOS_OK)
    {
        LOS_IntRestore(uvIntSave);
        return LOS_NOK;
    }
    pstBoxInfo->uwBlkSize = LOS_pcrambox_ALIGNED(uwBlkSize + LOS_pcrambox_MAGIC_SIZE);
    pstBoxInfo->uwBlkNum = (uwBoxSize - sizeof(LOS_PCRAMBOX_INFO) - g_TVCBNum * g_TVCBSize) / pstBoxInfo->uwBlkSize;
    pstBoxInfo->uwBlkCnt = 0;
    if (pstBoxInfo->uwBlkNum == 0)
    {
        LOS_IntRestore(uvIntSave);
        return LOS_NOK;
    }
    /*
     * calc the length of the index area
     * SKIP the index area
     * The PCRAM pool is divided into two parts, INDEX area as well as FREE MEMORY NODE area
     */
    pstNode = (LOS_PCRAMBOX_NODE *)(pstBoxInfo + sizeof(LOS_PCRAMBOX_INFO) + g_TVCBNum * g_TVCBSize);
    pstBoxInfo->curFreeNode = pstNode;
    for (i = 0; i < pstBoxInfo->uwBlkNum - 1; ++i)
    {
        pstNode->pstNext = OS_pcrambox_NODE_NEXT(pstNode, pstBoxInfo->uwBlkSize);
        pstNode = pstNode->pstNext;
    }
    pstNode->pstNext = (LOS_PCRAMBOX_NODE *)NULL;  /* The last node */
    //pBoxMem = (UINT32 *)0xFFAAFFAA;
#if ((LOSCFG_PLATFORM_EXC == YES) && (LOSCFG_SAVE_EXC_INFO == YES))
    osMemInfoUpdate(pBoxMem, uwBoxSize, MEM_MANG_pcrambox);
#endif
    pstBoxInfo->flag = 0xFFAA;
    (VOID)LOS_IntRestore(uvIntSave);
    printf("current free node addr begins from:%p\n",(UINT32)pstBoxInfo->curFreeNode);
    return LOS_OK;
}

/*****************************************************************************
 Function : os_pcramboxMerge
 Description : Merge memory block when there are no empty blocks to allocate
 Input       : pBoxMem  --- Pointer to memory pool
 Output      : None
 Return      : LOS_OK - OK, LOS_NOK - Error
 发现一个问题，因为分配时寻址都是直接从上一个块加上块大小直接得到下一个块的地址，如果merge时将后面已使用的块移动到前面，会造成物理地址不连续
*****************************************************************************/
LITE_OS_SEC_TEXT UINT32 os_pcramboxMerge(VOID *pBoxMem)
{
    // LOS_PCRAMBOX_INFO *pstBoxInfo = (LOS_PCRAMBOX_INFO *)pBoxMem;
    // UINT32 uwRet = LOS_NOK;
    // UINTPTR uvIntSave;

    // if (pBoxMem == NULL)
    // {
    //     return LOS_NOK;
    // }

    // uvIntSave = LOS_IntLock();
    // LOS_PCRAMBOX_NODE *pstNode = (LOS_PCRAMBOX_NODE *)(pstBoxInfo + 1);
    // LOS_PCRAMBOX_NODE *pstNextNode = pstNode; 
    // do
    // {
    //     if (osCheckBoxMem(pstBoxInfo, pstNode) != LOS_OK)
    //     {
    //         break;
    //     }
    //     pstNextNode += pstBoxInfo->uwBlkSize;
    //     if(OS_pcrambox_CHECK_MAGIC(pstNextNode) == LOS_OK)
    //     {
    //         pstNode->pstNext = pstNextNode;
    //         pstBoxInfo->uwBlkCnt--;
    //         pstNode = pstNextNode;
    //     }
    // } while (pstNode->pstNext == NULL);
    // uwRet = LOS_OK;
    // (VOID)LOS_IntRestore(uvIntSave);

    // return uwRet;
    return LOS_NOK;
}

/*****************************************************************************
 Function : os_pcramboxAlloc
 Description : Allocate Memory block from PCRAM pool
 Input       : pBoxMem  --- Pointer to memory pool
               BlkNum  --- the number of block need to be allocated
 Output      : None
 Return      : Pointer to allocated memory block
*****************************************************************************/
LITE_OS_SEC_TEXT VOID *os_pcramboxAlloc(VOID *pBoxMem, UINT16 BlkNum)
{
    LOS_PCRAMBOX_INFO *pstBoxInfo = (LOS_PCRAMBOX_INFO *)pBoxMem;
    LOS_PCRAMBOX_NODE *pRet = NULL;
    UINTPTR uvIntSave;
    UINT32 uwRet = LOS_NOK;

    if (pBoxMem == NULL)
    {
        return NULL;
    }

    uvIntSave = LOS_IntLock();
    pRet = pstBoxInfo->curFreeNode;
    VOID *pStart = pRet;
    while((pRet != NULL) && (BlkNum > 0))
    {
        pRet = pRet->pstNext;
        BlkNum--;
    }
    if(BlkNum != 0) 
    {
        uwRet = os_pcramboxMerge(OS_PCRAM_START_ADDR);
        (VOID)LOS_IntRestore(uvIntSave);
        return NULL;
    }
    else
    {
        pstBoxInfo->curFreeNode = pRet;
        pstBoxInfo->uwBlkCnt += BlkNum;
        (VOID)LOS_IntRestore(uvIntSave);
        return pStart;
    }
}

/*****************************************************************************
 Function : SC_MALLOC
 Description : Allocate sc block from pcram Memory pool
 Input       : pBoxMem  --- Pointer to memory pool
 Output      : None
 Return      : Pointer to allocated memory block
*****************************************************************************/
LITE_OS_SEC_TEXT VOID *SC_MALLOC(UINT8 Identifier, UINT16 Length, UINT8 *Flag)
{
    UINT8 TID;
    UINT32 uwRet = LOS_NOK;
    UINT16 BlkNum, uwBlkSize;
    UINTPTR uvIntSave;
    VOID *pRet;
    uvIntSave = LOS_IntLock();
    TID = (UINT8)LOS_CurTaskIDGet();
    if(TID == LOS_ERRNO_TSK_ID_INVALID)
    {
        LOS_IntRestore(uvIntSave);
        return NULL;
    }
    pRet = osFoundVIB(TID, Identifier);
    if(pRet != NULL)
    {
        if(*Flag == 0)
        {
            LOS_pcramboxClr(OS_PCRAM_START_ADDR , ((VIB *)pRet)->addrStart, ((VIB *)pRet)->varBlkNum);
            *Flag = 1;
            LOS_IntRestore(uvIntSave);
            return ((VIB *)pRet)->addrStart;
        }
        else
        {
            LOS_IntRestore(uvIntSave);
            return ((VIB *)pRet)->addrStart;
        }
        
    }
    else
    {
        uwBlkSize = ((LOS_PCRAMBOX_INFO *)OS_PCRAM_START_ADDR)->uwBlkSize;
        BlkNum = (Length % uwBlkSize) == 0 ? (Length / uwBlkSize) : (Length / uwBlkSize + 1); 
        pRet = os_pcramboxAlloc(OS_PCRAM_START_ADDR  , BlkNum);
        if(pRet != NULL)
        {
            uwRet = osCreateVIB(pRet, TID, Identifier, BlkNum);
            LOS_pcramboxClr(OS_PCRAM_START_ADDR , pRet, BlkNum);
            *Flag = 0;
            LOS_IntRestore(uvIntSave);
            printf("the allocated boxes numbers are %d\n", BlkNum);
            return pRet;
        } 
        else
        {
            LOS_IntRestore(uvIntSave);
            return NULL;
        }
        
    }
}

/*****************************************************************************
 Function : os_pcramboxFree
 Description : Free Memory block and return it to PCRAM pool
 Input       : pBoxMem  --- Pointer to memory pool
               pBox     --- Pointer to memory block to free
               BlkNum   --- the number of blocks to free
 Output      : None
 Return      : LOS_OK - OK, LOS_NOK - Error
*****************************************************************************/
LITE_OS_SEC_TEXT UINT32 os_pcramboxFree(VOID *pBoxMem, VOID *pBox, UINT16 BlkNum)
{
    LOS_PCRAMBOX_INFO *pstBoxInfo = (LOS_PCRAMBOX_INFO *)pBoxMem;
    UINT32 uwRet = LOS_NOK;
    UINTPTR uvIntSave;

    if (pBoxMem == NULL || pBox == NULL || BlkNum <= 0)
    {
        return LOS_NOK;
    }
    uvIntSave = LOS_IntLock();
    do
    {
        // LOS_PCRAMBOX_NODE *pstNode = OS_PCRAMBOX_NODE_ADDR(pBox);
        VOID *pRet = pBox; 

        // if (osCheckBoxMem(pstBoxInfo, pstNode) != LOS_OK)
        // {
        //     break;
        // }
        while(BlkNum > 0)
        {
            OS_pcrambox_SET_MAGIC(pRet);
            pRet += pstBoxInfo->uwBlkSize;
            BlkNum--;
            //pstBoxInfo->uwBlkCnt--;
        }
        uwRet = LOS_OK;
        break;
    } while (0);
    (VOID)LOS_IntRestore(uvIntSave);
    return uwRet;
}

/*****************************************************************************
 Function : SC_FREE
 Description : Allocate sc block from pcram Memory pool
 Input       : pBoxMem  --- Pointer to memory pool
 Output      : None
 Return      : Pointer to allocated memory block
*****************************************************************************/
LITE_OS_SEC_TEXT UINT32 SC_FREE(UINT8 Identifier)
{
    UINT8 TID;
    UINT32 uwRet = LOS_NOK;
    UINT16 BlkNum, uwBlkSize;
    UINTPTR uvIntSave;
    VOID *pRet;
    uvIntSave = LOS_IntLock();
    TID = LOS_CurTaskIDGet();
    if(TID == LOS_ERRNO_TSK_ID_INVALID)
    {
        (VOID)LOS_IntRestore(uvIntSave);
        return LOS_NOK;
    }
    pRet = osFoundVIB(TID, Identifier);
    if(pRet == NULL)
    {
        (VOID)LOS_IntRestore(uvIntSave);
        return LOS_NOK;
    }
    uwRet = os_pcramboxFree(OS_PCRAM_START_ADDR , ((VIB *)pRet)->addrStart, ((VIB *)pRet)->varBlkNum);
    if(uwRet == LOS_OK)
    {
        uwRet = osDeleteVIB(pRet);
    }
    else
    {
        (VOID)LOS_IntRestore(uvIntSave);
        return LOS_NOK;
    }
    (VOID)LOS_IntRestore(uvIntSave);
    return uwRet;
}

/*****************************************************************************
 Function : LOS_pcramboxClr
 Description : Clear the memory block
 Input       : pBoxMem  --- Pointer to memory pool
               pBox     --- Pointer to memory block to clear
               BlkNum   --- the number of the block need to clear
 Output      : None
 Return      : None
*****************************************************************************/
LITE_OS_SEC_TEXT_MINOR VOID LOS_pcramboxClr(VOID *pBoxMem, VOID *pBox, UINT16 BlkNum)
{
    LOS_PCRAMBOX_INFO *pstBoxInfo = (LOS_PCRAMBOX_INFO *)pBoxMem;

    if (pBoxMem == NULL || pBox == NULL)
    {
        return;
    }

    memset(pBox, 0, pstBoxInfo->uwBlkSize * BlkNum);
}

/*****************************************************************************
 Function : LOS_pcramboxStatisticsGet
 Description : Get information about pcrambox
 Input       : pBoxMem     --- Pointer to the calculate pcrambox
 Output      : puwMaxBlk   --- Record the total number of pcrambox
               puwBlkCnt   --- Record the number of the allocated blocks of pcrambox
               puwBlkSize  --- Record the block size of pcrambox
 Return      : LOS_OK - OK, LOS_NOK - Error
*****************************************************************************/
LITE_OS_SEC_TEXT_MINOR UINT32 LOS_pcramboxStatisticsGet(VOID *pBoxMem, UINT32 *puwMaxBlk, UINT32 *puwBlkCnt, UINT32 *puwBlkSize)
{
    if ((NULL == pBoxMem) || (NULL == puwMaxBlk) || (NULL == puwBlkCnt) || (NULL == puwBlkSize))
    {
        return LOS_NOK;
    }

    *puwMaxBlk = ((LOS_PCRAMBOX_INFO *)pBoxMem)->uwBlkNum;    /* Total number of blocks */
    *puwBlkCnt = ((LOS_PCRAMBOX_INFO *)pBoxMem)->uwBlkCnt;    /* The number of allocated blocks */
    *puwBlkSize = ((LOS_PCRAMBOX_INFO *)pBoxMem)->uwBlkSize;  /* Block size */

    return LOS_OK;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
