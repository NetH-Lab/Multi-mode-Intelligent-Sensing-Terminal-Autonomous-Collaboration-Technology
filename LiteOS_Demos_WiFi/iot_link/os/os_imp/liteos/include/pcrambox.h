#ifndef PCRAMBOX_H
#define PCRAMBOX_H

#include "los_config.h"
#include "los_membox.h"
#include "gd32vf103_pcram.h"
#if (LOSCFG_PLATFORM_EXC == YES)
#include "los_memcheck.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

// /**
//  * @ingroup los_membox
//  * Define whether to check the address validity
//  */
// #if (LOSCFG_PLATFORM_EXC == YES)
// #define LOS_MEMBOX_CHECK
// extern UINT8 g_aucMemMang[];
// #endif

/**
 * @ingroup los_pcrambox
 * Structure of a free node in a memory pool
 */
typedef struct PCRAMBOX_NODE
{
    struct PCRAMBOX_NODE *pstNext;            /* Free node's pointer to the next node in a memory pool */
} LOS_PCRAMBOX_NODE;

/**
 * @ingroup los_pcrambox
 * Memory pool information structure
 */
typedef struct
{
   UINT16           uwBlkSize;                  /* Block size */
   UINT16           uwBlkNum;                   /* Total number of blocks */
   UINT16           uwBlkCnt;                   /* The number of allocated blocks */
   LOS_PCRAMBOX_NODE  *curFreeNode;                 /* the location of the current free node for next allocation */
   UINT16           flag;                       /*whether pcram need to inisialize*/
} LOS_PCRAMBOX_INFO;

/**
 * @ingroup los_pcrambox
 * TID+Identifier information structure
 */
typedef struct 
{
    UINT8          TID;                        /*current task id  */
    UINT8         Identifier;                 /*variable name  */
}TIdentifier;

/**
 * @ingroup los_pcrambox
 * Variable Index Block information structure
 */
typedef struct 
{
    TIdentifier     varName;                     /* TID+Identifier */
    UINT16          varBlkNum;                   /*Number of allocated blk for variable*/
    UINT16          *addrStart;                     /*begin of addr for variable*/
    UINT16          varCRC;                      /*CRC of SCLIST*/
    VOID            *fromTVCB;                   /*which TVCB the VIB belongs to*/
}VIB;

/**
 * @ingroup los_pcrambox
 * Task Variable Content Block information structure
 */
typedef struct 
{
    UINT16          TID;                        /* which TASK each tvcb belongs to*/
    UINT16          varNum;                    /* number of variable in each task */
}TVCB;


/**
 * @ingroup los_pcrambox
 * Default enabled pcrambox's magic word detection function, this makes each block of pcrambox
 * need an extra 4 bytes of space. If it is not necessary, please do not change it.
 * If the magic word of pcrambox disabled, a bug will be generated, that is, when free a block
 * that has been freed, the pcrambox will be destroyed.
 */
#define LOS_pcrambox_MAGIC_CHECK
#ifdef LOS_pcrambox_MAGIC_CHECK
#define LOS_pcrambox_MAGIC_SIZE    4
#define SCLIST_TVCB_STRUCT_SIZE    sizeof(TVCB)
#define OS_PCRAM_START_ADDR         BANK_SRAM_ADDR
#else
#define LOS_pcrambox_MAGIC_SIZE    0
#endif

/**
 * @ingroup los_pcrambox
 * The memory box is aligned to 4 (memory pool addr or memory box node size)
 */
#define LOS_pcrambox_ALIGNED(align)           (((UINT32)(align) + 3) & 0xfffffffc)

/**
 * @ingroup los_pcrambox
 * Memory pool size
 * Users can use this macro to calculate the total size of pcrambox based on block size and block number
 */
#define LOS_pcrambox_SIZE(uwBlkSize, uwBlkNum)   (sizeof(LOS_pcrambox_INFO) + LOS_pcrambox_ALIGNED(uwBlkSize + LOS_pcrambox_MAGIC_SIZE) * (uwBlkNum))

/**
 *@ingroup los_pcrambox
 *@brief Initialize a memory pool.
 *
 *@par Description:
 *<ul>
 *<li>This API is used to initialize a memory pool.</li>
 *</ul>
 *@attention
 *<ul>
 *<li>The uwBoxSize parameter value should match the following two conditions : 1) Be less than or equal to the Memory pool size; 2) Be greater than the size of LOS_pcrambox_INFO.</li>
 *</ul>
 *
 *@param pBoxMem     [IN] Memory pool address.
 *@param uwBoxSize   [IN] Memory pool size.
 *@param uwBlkSize   [IN] Memory block size.
 *
 *@retval #LOS_NOK   The memory pool fails to be initialized.
 *@retval #LOS_OK    The memory pool is successfully initialized.
 *@par Dependency:
 *<ul>
 *<li>los_pcrambox.h: the header file that contains the API declaration.</li>
 *</ul>
 *@see None.
 *@since Huawei LiteOS V100R001C00
 */
extern UINT32 LOS_pcramboxInit(VOID *pBoxMem, UINT32 uwBoxSize, UINT16 uwBlkSize);

/**
 *@ingroup los_pcrambox
 *@brief Request a memory block.
 *
 *@par Description:
 *<ul>
 *<li>This API is used to request a memory block.</li>
 *</ul>
 *@attention
 *<ul>
 *<li>The input pPool parameter must be initialized via func LOS_pcramboxInit.</li>
 *</ul>
 *
 *@param pBoxMem     [IN] Memory pool address.
 *
 *@retval #VOID*      The request is accepted, and return a memory block address.
 *@retval #NULL       The request fails.
 *@par Dependency:
 *<ul>
 *<li>los_pcrambox.h: the header file that contains the API declaration.</li>
 *</ul>
 *@see LOS_pcramboxFree
 *@since Huawei LiteOS V100R001C00
 */
//extern VOID *LOS_pcramboxAlloc(VOID *pBoxMem);
extern VOID *SC_MALLOC(UINT8 Identifier, UINT16 Length, UINT8 *Flag);
/**
 *@ingroup los_pcrambox
 *@brief Free a memory block.
 *
 *@par Description:
 *<ul>
 *<li>This API is used to free a memory block.</li>
 *</ul>
 *@attention
 *<ul>
 *<li>The input pPool parameter must be initialized via func LOS_pcramboxInit.</li>
 *<li>The input pBox parameter must be allocated by LOS_pcramboxAlloc.</li>
 *</ul>
 *
 *@param pBoxMem     [IN] Memory pool address.
 *@param pBox        [IN] Memory block address.
 *
 *@retval #LOS_NOK   This memory block fails to be freed.
 *@retval #LOS_OK    This memory block is successfully freed.
 *@par Dependency:
 *<ul>
 *<li>los_pcrambox.h: the header file that contains the API declaration.</li>
 *</ul>
 *@see LOS_pcramboxAlloc
 *@since Huawei LiteOS V100R001C00
 */
//extern UINT32 LOS_pcramboxFree(VOID *pBoxMem, VOID *pBox);
extern UINT32 SC_FREE(UINT8 Identifier);

/**
 *@ingroup los_pcrambox
 *@brief Clear a memory block.
 *
 *@par Description:
 *<ul>
 *<li>This API is used to set the memory block value to be 0.</li>
 *</ul>
 *@attention
 *<ul>
 *<li>The input pPool parameter must be initialized via func LOS_pcramboxInit.</li>
 *<li>The input pBox parameter must be allocated by LOS_pcramboxAlloc.</li>
 *</ul>
 *
 *@param pBoxMem     [IN] Memory pool address.
 *@param pBox        [IN] Memory block address.
 *
 *@retval VOID
 *@par Dependency:
 *<ul>
 *<li>los_pcrambox.h: the header file that contains the API declaration.</li>
 *</ul>
 *@see None.
 *@since Huawei LiteOS V100R001C00
 */
extern VOID LOS_pcramboxClr(VOID *pBoxMem, VOID *pBox, UINT16 BlkNum);


/**
 *@ingroup los_pcrambox
 *@brief calculate pcrambox information.
 *
 *@par Description:
 *<ul>
 *<li>This API is used to calculate pcrambox information.</li>
 *</ul>
 *@attention
 *<ul>
 *<li>One parameter of this interface is a pointer, it should be a correct value, otherwise, the system may be abnormal.</li>
 *</ul>
 *
 *@param  pBoxMem        [IN]  Type  #VOID*   Pointer to the calculate pcrambox.
 *@param  uwMaxBlk       [OUT] Type  #UINT32* Record pcrambox max block.
 *@param  uwBlkCnt       [OUT] Type  #UINT32* Record pcrambox block count alreay allocated.
 *@param  uwBlkSize      [OUT] Type  #UINT32* Record pcrambox block size.
 *
 *@retval #LOS_OK        The heap status calculate success.
 *@retval #LOS_NOK       The pcrambox  status calculate with some error.
 *@par Dependency:
 *<ul><li>los_memory.h: the header file that contains the API declaration.</li></ul>
 *@see LOS_MemAlloc | LOS_MemRealloc | LOS_MemFree
 *@since Huawei LiteOS V100R001C00
 */
extern UINT32 LOS_pcramboxStatisticsGet(VOID *pBoxMem, UINT32 *puwMaxBlk, UINT32 *puwBlkCnt, UINT32 *puwBlkSize);

//extern UINT32 os_PCRAM_MERGE(VOID *pBoxMem);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
