/* -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil -*- */
#ifndef _BLT_TCLPROCS_H
#define _BLT_TCLPROCS_H

#include "bltAlloc.h"
#include "bltChain.h"
#include "bltHash.h"
#include "bltTags.h"
#include "bltList.h"
#include "bltPool.h"
#include "bltTree.h"
#include "bltDataTable.h"
#include "bltVector.h"

/* !BEGIN!: Do not edit below this line. */

/*
 * Exported function declarations:
 */

/* Slot 0 is reserved */
#ifndef Blt_AllocInit_DECLARED
#define Blt_AllocInit_DECLARED
/* 1 */
BLT_EXTERN void		Blt_AllocInit(Blt_MallocProc *mallocProc,
				Blt_ReallocProc *reallocProc,
				Blt_FreeProc *freeProc);
#endif
#ifndef Blt_Malloc_DECLARED
#define Blt_Malloc_DECLARED
/* 2 */
BLT_EXTERN void *	Blt_Malloc(size_t size);
#endif
#ifndef Blt_Free_DECLARED
#define Blt_Free_DECLARED
/* 3 */
BLT_EXTERN void		Blt_Free(const void *mem);
#endif
#ifndef Blt_Realloc_DECLARED
#define Blt_Realloc_DECLARED
/* 4 */
BLT_EXTERN void *	Blt_Realloc(void *ptr, size_t size);
#endif
#ifndef Blt_Calloc_DECLARED
#define Blt_Calloc_DECLARED
/* 5 */
BLT_EXTERN void *	Blt_Calloc(size_t numElem, size_t elemSize);
#endif
#ifndef Blt_MallocAbortOnError_DECLARED
#define Blt_MallocAbortOnError_DECLARED
/* 6 */
BLT_EXTERN void *	Blt_MallocAbortOnError(size_t size,
				const char *fileName, int lineNum);
#endif
#ifndef Blt_CallocAbortOnError_DECLARED
#define Blt_CallocAbortOnError_DECLARED
/* 7 */
BLT_EXTERN void *	Blt_CallocAbortOnError(size_t numElem,
				size_t elemSize, const char *fileName,
				int lineNum);
#endif
#ifndef Blt_ReallocAbortOnError_DECLARED
#define Blt_ReallocAbortOnError_DECLARED
/* 8 */
BLT_EXTERN void *	Blt_ReallocAbortOnError(void *ptr, size_t size,
				const char *fileName, int lineNum);
#endif
#ifndef Blt_Strndup_DECLARED
#define Blt_Strndup_DECLARED
/* 9 */
BLT_EXTERN const char *	 Blt_Strndup(const char *string, size_t size);
#endif
#ifndef Blt_Strdup_DECLARED
#define Blt_Strdup_DECLARED
/* 10 */
BLT_EXTERN const char *	 Blt_Strdup(const char *string);
#endif
#ifndef Blt_StrdupAbortOnError_DECLARED
#define Blt_StrdupAbortOnError_DECLARED
/* 11 */
BLT_EXTERN const char *	 Blt_StrdupAbortOnError(const char *string,
				const char *fileName, int lineNum);
#endif
#ifndef Blt_StrndupAbortOnError_DECLARED
#define Blt_StrndupAbortOnError_DECLARED
/* 12 */
BLT_EXTERN const char *	 Blt_StrndupAbortOnError(const char *string,
				size_t size, const char *fileName,
				int lineNum);
#endif
#ifndef Blt_Chain_Init_DECLARED
#define Blt_Chain_Init_DECLARED
/* 13 */
BLT_EXTERN void		Blt_Chain_Init(Blt_Chain chain);
#endif
#ifndef Blt_Chain_Create_DECLARED
#define Blt_Chain_Create_DECLARED
/* 14 */
BLT_EXTERN Blt_Chain	Blt_Chain_Create(void );
#endif
#ifndef Blt_Chain_Destroy_DECLARED
#define Blt_Chain_Destroy_DECLARED
/* 15 */
BLT_EXTERN void		Blt_Chain_Destroy(Blt_Chain chain);
#endif
#ifndef Blt_Chain_NewLink_DECLARED
#define Blt_Chain_NewLink_DECLARED
/* 16 */
BLT_EXTERN Blt_ChainLink Blt_Chain_NewLink(void );
#endif
#ifndef Blt_Chain_AllocLink_DECLARED
#define Blt_Chain_AllocLink_DECLARED
/* 17 */
BLT_EXTERN Blt_ChainLink Blt_Chain_AllocLink(size_t size);
#endif
#ifndef Blt_Chain_Append_DECLARED
#define Blt_Chain_Append_DECLARED
/* 18 */
BLT_EXTERN Blt_ChainLink Blt_Chain_Append(Blt_Chain chain,
				ClientData clientData);
#endif
#ifndef Blt_Chain_Prepend_DECLARED
#define Blt_Chain_Prepend_DECLARED
/* 19 */
BLT_EXTERN Blt_ChainLink Blt_Chain_Prepend(Blt_Chain chain,
				ClientData clientData);
#endif
#ifndef Blt_Chain_Reset_DECLARED
#define Blt_Chain_Reset_DECLARED
/* 20 */
BLT_EXTERN void		Blt_Chain_Reset(Blt_Chain chain);
#endif
#ifndef Blt_Chain_InitLink_DECLARED
#define Blt_Chain_InitLink_DECLARED
/* 21 */
BLT_EXTERN void		Blt_Chain_InitLink(Blt_ChainLink link);
#endif
#ifndef Blt_Chain_LinkAfter_DECLARED
#define Blt_Chain_LinkAfter_DECLARED
/* 22 */
BLT_EXTERN void		Blt_Chain_LinkAfter(Blt_Chain chain,
				Blt_ChainLink link, Blt_ChainLink after);
#endif
#ifndef Blt_Chain_LinkBefore_DECLARED
#define Blt_Chain_LinkBefore_DECLARED
/* 23 */
BLT_EXTERN void		Blt_Chain_LinkBefore(Blt_Chain chain,
				Blt_ChainLink link, Blt_ChainLink before);
#endif
#ifndef Blt_Chain_UnlinkLink_DECLARED
#define Blt_Chain_UnlinkLink_DECLARED
/* 24 */
BLT_EXTERN void		Blt_Chain_UnlinkLink(Blt_Chain chain,
				Blt_ChainLink link);
#endif
#ifndef Blt_Chain_DeleteLink_DECLARED
#define Blt_Chain_DeleteLink_DECLARED
/* 25 */
BLT_EXTERN void		Blt_Chain_DeleteLink(Blt_Chain chain,
				Blt_ChainLink link);
#endif
#ifndef Blt_Chain_GetNthLink_DECLARED
#define Blt_Chain_GetNthLink_DECLARED
/* 26 */
BLT_EXTERN Blt_ChainLink Blt_Chain_GetNthLink(Blt_Chain chain, long position);
#endif
#ifndef Blt_Chain_Sort_DECLARED
#define Blt_Chain_Sort_DECLARED
/* 27 */
BLT_EXTERN void		Blt_Chain_Sort(Blt_Chain chain,
				Blt_ChainCompareProc *proc);
#endif
#ifndef Blt_Chain_Reverse_DECLARED
#define Blt_Chain_Reverse_DECLARED
/* 28 */
BLT_EXTERN void		Blt_Chain_Reverse(Blt_Chain chain);
#endif
#ifndef Blt_Chain_IsBefore_DECLARED
#define Blt_Chain_IsBefore_DECLARED
/* 29 */
BLT_EXTERN int		Blt_Chain_IsBefore(Blt_ChainLink first,
				Blt_ChainLink last);
#endif
#ifndef Blt_InitHashTable_DECLARED
#define Blt_InitHashTable_DECLARED
/* 30 */
BLT_EXTERN void		Blt_InitHashTable(Blt_HashTable *tablePtr,
				size_t keyType);
#endif
#ifndef Blt_InitHashTableWithPool_DECLARED
#define Blt_InitHashTableWithPool_DECLARED
/* 31 */
BLT_EXTERN void		Blt_InitHashTableWithPool(Blt_HashTable *tablePtr,
				size_t keyType);
#endif
#ifndef Blt_DeleteHashTable_DECLARED
#define Blt_DeleteHashTable_DECLARED
/* 32 */
BLT_EXTERN void		Blt_DeleteHashTable(Blt_HashTable *tablePtr);
#endif
#ifndef Blt_DeleteHashEntry_DECLARED
#define Blt_DeleteHashEntry_DECLARED
/* 33 */
BLT_EXTERN void		Blt_DeleteHashEntry(Blt_HashTable *tablePtr,
				Blt_HashEntry *entryPtr);
#endif
#ifndef Blt_FirstHashEntry_DECLARED
#define Blt_FirstHashEntry_DECLARED
/* 34 */
BLT_EXTERN Blt_HashEntry * Blt_FirstHashEntry(Blt_HashTable *tablePtr,
				Blt_HashSearch *searchPtr);
#endif
#ifndef Blt_NextHashEntry_DECLARED
#define Blt_NextHashEntry_DECLARED
/* 35 */
BLT_EXTERN Blt_HashEntry * Blt_NextHashEntry(Blt_HashSearch *srchPtr);
#endif
#ifndef Blt_HashStats_DECLARED
#define Blt_HashStats_DECLARED
/* 36 */
BLT_EXTERN const char *	 Blt_HashStats(Blt_HashTable *tablePtr);
#endif
#ifndef Blt_Tags_Create_DECLARED
#define Blt_Tags_Create_DECLARED
/* 37 */
BLT_EXTERN Blt_Tags	Blt_Tags_Create(void );
#endif
#ifndef Blt_Tags_Destroy_DECLARED
#define Blt_Tags_Destroy_DECLARED
/* 38 */
BLT_EXTERN void		Blt_Tags_Destroy(Blt_Tags tags);
#endif
#ifndef Blt_Tags_Init_DECLARED
#define Blt_Tags_Init_DECLARED
/* 39 */
BLT_EXTERN void		Blt_Tags_Init(Blt_Tags tags);
#endif
#ifndef Blt_Tags_Reset_DECLARED
#define Blt_Tags_Reset_DECLARED
/* 40 */
BLT_EXTERN void		Blt_Tags_Reset(Blt_Tags tags);
#endif
#ifndef Blt_Tags_ItemHasTag_DECLARED
#define Blt_Tags_ItemHasTag_DECLARED
/* 41 */
BLT_EXTERN int		Blt_Tags_ItemHasTag(Blt_Tags tags, ClientData item,
				const char *tag);
#endif
#ifndef Blt_Tags_AddTag_DECLARED
#define Blt_Tags_AddTag_DECLARED
/* 42 */
BLT_EXTERN void		Blt_Tags_AddTag(Blt_Tags tags, const char *tag);
#endif
#ifndef Blt_Tags_AddItemToTag_DECLARED
#define Blt_Tags_AddItemToTag_DECLARED
/* 43 */
BLT_EXTERN void		Blt_Tags_AddItemToTag(Blt_Tags tags, const char *tag,
				ClientData item);
#endif
#ifndef Blt_Tags_ForgetTag_DECLARED
#define Blt_Tags_ForgetTag_DECLARED
/* 44 */
BLT_EXTERN void		Blt_Tags_ForgetTag(Blt_Tags tags, const char *tag);
#endif
#ifndef Blt_Tags_RemoveItemFromTag_DECLARED
#define Blt_Tags_RemoveItemFromTag_DECLARED
/* 45 */
BLT_EXTERN void		Blt_Tags_RemoveItemFromTag(Blt_Tags tags,
				const char *tag, ClientData item);
#endif
#ifndef Blt_Tags_ClearTagsFromItem_DECLARED
#define Blt_Tags_ClearTagsFromItem_DECLARED
/* 46 */
BLT_EXTERN void		Blt_Tags_ClearTagsFromItem(Blt_Tags tags,
				ClientData item);
#endif
#ifndef Blt_Tags_AppendTagsToChain_DECLARED
#define Blt_Tags_AppendTagsToChain_DECLARED
/* 47 */
BLT_EXTERN void		Blt_Tags_AppendTagsToChain(Blt_Tags tags,
				ClientData item, Blt_Chain list);
#endif
#ifndef Blt_Tags_AppendTagsToObj_DECLARED
#define Blt_Tags_AppendTagsToObj_DECLARED
/* 48 */
BLT_EXTERN void		Blt_Tags_AppendTagsToObj(Blt_Tags tags,
				ClientData item, Tcl_Obj *objPtr);
#endif
#ifndef Blt_Tags_AppendAllTagsToObj_DECLARED
#define Blt_Tags_AppendAllTagsToObj_DECLARED
/* 49 */
BLT_EXTERN void		Blt_Tags_AppendAllTagsToObj(Blt_Tags tags,
				Tcl_Obj *objPtr);
#endif
#ifndef Blt_Tags_GetItemList_DECLARED
#define Blt_Tags_GetItemList_DECLARED
/* 50 */
BLT_EXTERN Blt_Chain	Blt_Tags_GetItemList(Blt_Tags tags, const char *tag);
#endif
#ifndef Blt_Tags_GetTable_DECLARED
#define Blt_Tags_GetTable_DECLARED
/* 51 */
BLT_EXTERN Blt_HashTable * Blt_Tags_GetTable(Blt_Tags tags);
#endif
#ifndef Blt_List_Init_DECLARED
#define Blt_List_Init_DECLARED
/* 52 */
BLT_EXTERN void		Blt_List_Init(Blt_List list, size_t type);
#endif
#ifndef Blt_List_Reset_DECLARED
#define Blt_List_Reset_DECLARED
/* 53 */
BLT_EXTERN void		Blt_List_Reset(Blt_List list);
#endif
#ifndef Blt_List_Create_DECLARED
#define Blt_List_Create_DECLARED
/* 54 */
BLT_EXTERN Blt_List	Blt_List_Create(size_t type);
#endif
#ifndef Blt_List_Destroy_DECLARED
#define Blt_List_Destroy_DECLARED
/* 55 */
BLT_EXTERN void		Blt_List_Destroy(Blt_List list);
#endif
#ifndef Blt_List_CreateNode_DECLARED
#define Blt_List_CreateNode_DECLARED
/* 56 */
BLT_EXTERN Blt_ListNode	 Blt_List_CreateNode(Blt_List list, const char *key);
#endif
#ifndef Blt_List_DeleteNode_DECLARED
#define Blt_List_DeleteNode_DECLARED
/* 57 */
BLT_EXTERN void		Blt_List_DeleteNode(Blt_ListNode node);
#endif
#ifndef Blt_List_Append_DECLARED
#define Blt_List_Append_DECLARED
/* 58 */
BLT_EXTERN Blt_ListNode	 Blt_List_Append(Blt_List list, const char *key,
				ClientData clientData);
#endif
#ifndef Blt_List_Prepend_DECLARED
#define Blt_List_Prepend_DECLARED
/* 59 */
BLT_EXTERN Blt_ListNode	 Blt_List_Prepend(Blt_List list, const char *key,
				ClientData clientData);
#endif
#ifndef Blt_List_LinkAfter_DECLARED
#define Blt_List_LinkAfter_DECLARED
/* 60 */
BLT_EXTERN void		Blt_List_LinkAfter(Blt_List list, Blt_ListNode node,
				Blt_ListNode afterNode);
#endif
#ifndef Blt_List_LinkBefore_DECLARED
#define Blt_List_LinkBefore_DECLARED
/* 61 */
BLT_EXTERN void		Blt_List_LinkBefore(Blt_List list, Blt_ListNode node,
				Blt_ListNode beforeNode);
#endif
#ifndef Blt_List_UnlinkNode_DECLARED
#define Blt_List_UnlinkNode_DECLARED
/* 62 */
BLT_EXTERN void		Blt_List_UnlinkNode(Blt_ListNode node);
#endif
#ifndef Blt_List_GetNode_DECLARED
#define Blt_List_GetNode_DECLARED
/* 63 */
BLT_EXTERN Blt_ListNode	 Blt_List_GetNode(Blt_List list, const char *key);
#endif
#ifndef Blt_List_DeleteNodeByKey_DECLARED
#define Blt_List_DeleteNodeByKey_DECLARED
/* 64 */
BLT_EXTERN void		Blt_List_DeleteNodeByKey(Blt_List list,
				const char *key);
#endif
#ifndef Blt_List_GetNthNode_DECLARED
#define Blt_List_GetNthNode_DECLARED
/* 65 */
BLT_EXTERN Blt_ListNode	 Blt_List_GetNthNode(Blt_List list, long position,
				int direction);
#endif
#ifndef Blt_List_Sort_DECLARED
#define Blt_List_Sort_DECLARED
/* 66 */
BLT_EXTERN void		Blt_List_Sort(Blt_List list,
				Blt_ListCompareProc *proc);
#endif
#ifndef Blt_Pool_Create_DECLARED
#define Blt_Pool_Create_DECLARED
/* 67 */
BLT_EXTERN Blt_Pool	Blt_Pool_Create(int type);
#endif
#ifndef Blt_Pool_Destroy_DECLARED
#define Blt_Pool_Destroy_DECLARED
/* 68 */
BLT_EXTERN void		Blt_Pool_Destroy(Blt_Pool pool);
#endif
#ifndef Blt_Tree_GetUid_DECLARED
#define Blt_Tree_GetUid_DECLARED
/* 69 */
BLT_EXTERN Blt_TreeUid	Blt_Tree_GetUid(Blt_Tree tree, const char *string);
#endif
#ifndef Blt_Tree_GetUidFromNode_DECLARED
#define Blt_Tree_GetUidFromNode_DECLARED
/* 70 */
BLT_EXTERN Blt_TreeUid	Blt_Tree_GetUidFromNode(Blt_TreeNode node,
				const char *string);
#endif
#ifndef Blt_Tree_GetNextId_DECLARED
#define Blt_Tree_GetNextId_DECLARED
/* 71 */
BLT_EXTERN long		Blt_Tree_GetNextId(Blt_Tree tree);
#endif
#ifndef Blt_Tree_CreateNode_DECLARED
#define Blt_Tree_CreateNode_DECLARED
/* 72 */
BLT_EXTERN Blt_TreeNode	 Blt_Tree_CreateNode(Blt_Tree tree,
				Blt_TreeNode parent, const char *name,
				Blt_TreeNode before);
#endif
#ifndef Blt_Tree_CreateNodeWithId_DECLARED
#define Blt_Tree_CreateNodeWithId_DECLARED
/* 73 */
BLT_EXTERN Blt_TreeNode	 Blt_Tree_CreateNodeWithId(Blt_Tree tree,
				Blt_TreeNode parent, const char *name,
				long inode, Blt_TreeNode before);
#endif
#ifndef Blt_Tree_DeleteNode_DECLARED
#define Blt_Tree_DeleteNode_DECLARED
/* 74 */
BLT_EXTERN int		Blt_Tree_DeleteNode(Blt_Tree tree, Blt_TreeNode node);
#endif
#ifndef Blt_Tree_MoveNode_DECLARED
#define Blt_Tree_MoveNode_DECLARED
/* 75 */
BLT_EXTERN int		Blt_Tree_MoveNode(Blt_Tree tree, Blt_TreeNode node,
				Blt_TreeNode parent, Blt_TreeNode before);
#endif
#ifndef Blt_Tree_GetNodeFromIndex_DECLARED
#define Blt_Tree_GetNodeFromIndex_DECLARED
/* 76 */
BLT_EXTERN Blt_TreeNode	 Blt_Tree_GetNodeFromIndex(Blt_Tree tree, long inode);
#endif
#ifndef Blt_Tree_FindChild_DECLARED
#define Blt_Tree_FindChild_DECLARED
/* 77 */
BLT_EXTERN Blt_TreeNode	 Blt_Tree_FindChild(Blt_TreeNode parent,
				const char *name);
#endif
#ifndef Blt_Tree_NextNode_DECLARED
#define Blt_Tree_NextNode_DECLARED
/* 78 */
BLT_EXTERN Blt_TreeNode	 Blt_Tree_NextNode(Blt_TreeNode root,
				Blt_TreeNode node);
#endif
#ifndef Blt_Tree_PrevNode_DECLARED
#define Blt_Tree_PrevNode_DECLARED
/* 79 */
BLT_EXTERN Blt_TreeNode	 Blt_Tree_PrevNode(Blt_TreeNode root,
				Blt_TreeNode node);
#endif
#ifndef Blt_Tree_FirstChild_DECLARED
#define Blt_Tree_FirstChild_DECLARED
/* 80 */
BLT_EXTERN Blt_TreeNode	 Blt_Tree_FirstChild(Blt_TreeNode parent);
#endif
#ifndef Blt_Tree_LastChild_DECLARED
#define Blt_Tree_LastChild_DECLARED
/* 81 */
BLT_EXTERN Blt_TreeNode	 Blt_Tree_LastChild(Blt_TreeNode parent);
#endif
#ifndef Blt_Tree_IsBefore_DECLARED
#define Blt_Tree_IsBefore_DECLARED
/* 82 */
BLT_EXTERN int		Blt_Tree_IsBefore(Blt_TreeNode node1,
				Blt_TreeNode node2);
#endif
#ifndef Blt_Tree_IsAncestor_DECLARED
#define Blt_Tree_IsAncestor_DECLARED
/* 83 */
BLT_EXTERN int		Blt_Tree_IsAncestor(Blt_TreeNode node1,
				Blt_TreeNode node2);
#endif
#ifndef Blt_Tree_PrivateValue_DECLARED
#define Blt_Tree_PrivateValue_DECLARED
/* 84 */
BLT_EXTERN int		Blt_Tree_PrivateValue(Tcl_Interp *interp,
				Blt_Tree tree, Blt_TreeNode node,
				Blt_TreeUid uid);
#endif
#ifndef Blt_Tree_PublicValue_DECLARED
#define Blt_Tree_PublicValue_DECLARED
/* 85 */
BLT_EXTERN int		Blt_Tree_PublicValue(Tcl_Interp *interp,
				Blt_Tree tree, Blt_TreeNode node,
				Blt_TreeUid uid);
#endif
#ifndef Blt_Tree_GetValue_DECLARED
#define Blt_Tree_GetValue_DECLARED
/* 86 */
BLT_EXTERN int		Blt_Tree_GetValue(Tcl_Interp *interp, Blt_Tree tree,
				Blt_TreeNode node, const char *string,
				Tcl_Obj **valuePtr);
#endif
#ifndef Blt_Tree_ValueExists_DECLARED
#define Blt_Tree_ValueExists_DECLARED
/* 87 */
BLT_EXTERN int		Blt_Tree_ValueExists(Blt_Tree tree,
				Blt_TreeNode node, const char *string);
#endif
#ifndef Blt_Tree_SetValue_DECLARED
#define Blt_Tree_SetValue_DECLARED
/* 88 */
BLT_EXTERN int		Blt_Tree_SetValue(Tcl_Interp *interp, Blt_Tree tree,
				Blt_TreeNode node, const char *string,
				Tcl_Obj *valuePtr);
#endif
#ifndef Blt_Tree_UnsetValue_DECLARED
#define Blt_Tree_UnsetValue_DECLARED
/* 89 */
BLT_EXTERN int		Blt_Tree_UnsetValue(Tcl_Interp *interp,
				Blt_Tree tree, Blt_TreeNode node,
				const char *string);
#endif
#ifndef Blt_Tree_AppendObjValue_DECLARED
#define Blt_Tree_AppendObjValue_DECLARED
/* 90 */
BLT_EXTERN int		Blt_Tree_AppendObjValue(Tcl_Interp *interp,
				Blt_Tree tree, Blt_TreeNode node,
				const char *string, Tcl_Obj *objPtr);
#endif
#ifndef Blt_Tree_ListAppendObjValue_DECLARED
#define Blt_Tree_ListAppendObjValue_DECLARED
/* 91 */
BLT_EXTERN int		Blt_Tree_ListAppendObjValue(Tcl_Interp *interp,
				Blt_Tree tree, Blt_TreeNode node,
				const char *string, Tcl_Obj *objPtr);
#endif
#ifndef Blt_Tree_ListReplaceObjValues_DECLARED
#define Blt_Tree_ListReplaceObjValues_DECLARED
/* 92 */
BLT_EXTERN int		Blt_Tree_ListReplaceObjValues(Tcl_Interp *interp,
				Blt_Tree tree, Blt_TreeNode node,
				const char *string, int firstIndex,
				int lastIndex, int objc,
				Tcl_Obj *const *objv);
#endif
#ifndef Blt_Tree_GetArrayObjValue_DECLARED
#define Blt_Tree_GetArrayObjValue_DECLARED
/* 93 */
BLT_EXTERN int		Blt_Tree_GetArrayObjValue(Tcl_Interp *interp,
				Blt_Tree tree, Blt_TreeNode node,
				const char *arrayName, const char *elemName,
				Tcl_Obj **valueObjPtrPtr);
#endif
#ifndef Blt_Tree_SetArrayValue_DECLARED
#define Blt_Tree_SetArrayValue_DECLARED
/* 94 */
BLT_EXTERN int		Blt_Tree_SetArrayValue(Tcl_Interp *interp,
				Blt_Tree tree, Blt_TreeNode node,
				const char *arrayName, const char *elemName,
				Tcl_Obj *valueObjPtr);
#endif
#ifndef Blt_Tree_UnsetArrayValue_DECLARED
#define Blt_Tree_UnsetArrayValue_DECLARED
/* 95 */
BLT_EXTERN int		Blt_Tree_UnsetArrayValue(Tcl_Interp *interp,
				Blt_Tree tree, Blt_TreeNode node,
				const char *arrayName, const char *elemName);
#endif
#ifndef Blt_Tree_AppendArrayObjValue_DECLARED
#define Blt_Tree_AppendArrayObjValue_DECLARED
/* 96 */
BLT_EXTERN int		Blt_Tree_AppendArrayObjValue(Tcl_Interp *interp,
				Blt_Tree tree, Blt_TreeNode node,
				const char *arrayName, const char *elemName,
				Tcl_Obj *objPtr);
#endif
#ifndef Blt_Tree_ListAppendArrayObjValue_DECLARED
#define Blt_Tree_ListAppendArrayObjValue_DECLARED
/* 97 */
BLT_EXTERN int		Blt_Tree_ListAppendArrayObjValue(Tcl_Interp *interp,
				Blt_Tree tree, Blt_TreeNode node,
				const char *arrayName, const char *elemName,
				Tcl_Obj *valueObjPtr);
#endif
#ifndef Blt_Tree_ListReplaceArrayObjValues_DECLARED
#define Blt_Tree_ListReplaceArrayObjValues_DECLARED
/* 98 */
BLT_EXTERN int		Blt_Tree_ListReplaceArrayObjValues(
				Tcl_Interp *interp, Blt_Tree tree,
				Blt_TreeNode node, const char *string,
				const char *elemName, int firstIndex,
				int lastIndex, int objc,
				Tcl_Obj *const *objv);
#endif
#ifndef Blt_Tree_ArrayValueExists_DECLARED
#define Blt_Tree_ArrayValueExists_DECLARED
/* 99 */
BLT_EXTERN int		Blt_Tree_ArrayValueExists(Blt_Tree tree,
				Blt_TreeNode node, const char *arrayName,
				const char *elemName);
#endif
#ifndef Blt_Tree_ArrayNames_DECLARED
#define Blt_Tree_ArrayNames_DECLARED
/* 100 */
BLT_EXTERN int		Blt_Tree_ArrayNames(Tcl_Interp *interp,
				Blt_Tree tree, Blt_TreeNode node,
				const char *arrayName, Tcl_Obj *listObjPtr);
#endif
#ifndef Blt_Tree_GetScalarValueByUid_DECLARED
#define Blt_Tree_GetScalarValueByUid_DECLARED
/* 101 */
BLT_EXTERN int		Blt_Tree_GetScalarValueByUid(Tcl_Interp *interp,
				Blt_Tree tree, Blt_TreeNode node,
				Blt_TreeUid uid, Tcl_Obj **valuePtr);
#endif
#ifndef Blt_Tree_SetScalarValueByUid_DECLARED
#define Blt_Tree_SetScalarValueByUid_DECLARED
/* 102 */
BLT_EXTERN int		Blt_Tree_SetScalarValueByUid(Tcl_Interp *interp,
				Blt_Tree tree, Blt_TreeNode node,
				Blt_TreeUid uid, Tcl_Obj *valuePtr);
#endif
#ifndef Blt_Tree_UnsetScalarValueByUid_DECLARED
#define Blt_Tree_UnsetScalarValueByUid_DECLARED
/* 103 */
BLT_EXTERN int		Blt_Tree_UnsetScalarValueByUid(Tcl_Interp *interp,
				Blt_Tree tree, Blt_TreeNode node,
				Blt_TreeUid uid);
#endif
#ifndef Blt_Tree_AppendScalarObjValueByUid_DECLARED
#define Blt_Tree_AppendScalarObjValueByUid_DECLARED
/* 104 */
BLT_EXTERN int		Blt_Tree_AppendScalarObjValueByUid(
				Tcl_Interp *interp, Blt_Tree tree,
				Blt_TreeNode node, Blt_TreeUid uid,
				Tcl_Obj *objPtr);
#endif
#ifndef Blt_Tree_ListAppendScalarObjValueByUid_DECLARED
#define Blt_Tree_ListAppendScalarObjValueByUid_DECLARED
/* 105 */
BLT_EXTERN int		Blt_Tree_ListAppendScalarObjValueByUid(
				Tcl_Interp *interp, Blt_Tree tree,
				Blt_TreeNode node, Blt_TreeUid uid,
				Tcl_Obj *objPtr);
#endif
#ifndef Blt_Tree_ListReplaceScalarObjValuesByUid_DECLARED
#define Blt_Tree_ListReplaceScalarObjValuesByUid_DECLARED
/* 106 */
BLT_EXTERN int		Blt_Tree_ListReplaceScalarObjValuesByUid(
				Tcl_Interp *interp, Blt_Tree tree,
				Blt_TreeNode node, Blt_TreeUid uid,
				int firstIndex, int lastIndex, int objc,
				Tcl_Obj *const *objv);
#endif
#ifndef Blt_Tree_ScalarValueExistsByUid_DECLARED
#define Blt_Tree_ScalarValueExistsByUid_DECLARED
/* 107 */
BLT_EXTERN int		Blt_Tree_ScalarValueExistsByUid(Blt_Tree tree,
				Blt_TreeNode node, Blt_TreeUid uid);
#endif
#ifndef Blt_Tree_FirstValue_DECLARED
#define Blt_Tree_FirstValue_DECLARED
/* 108 */
BLT_EXTERN Blt_TreeUid	Blt_Tree_FirstValue(Blt_Tree tree, Blt_TreeNode node,
				Blt_TreeValueIterator *iterPtr);
#endif
#ifndef Blt_Tree_NextValue_DECLARED
#define Blt_Tree_NextValue_DECLARED
/* 109 */
BLT_EXTERN Blt_TreeUid	Blt_Tree_NextValue(Blt_Tree tree,
				Blt_TreeValueIterator *iterPtr);
#endif
#ifndef Blt_Tree_Apply_DECLARED
#define Blt_Tree_Apply_DECLARED
/* 110 */
BLT_EXTERN int		Blt_Tree_Apply(Blt_TreeNode root,
				Blt_TreeApplyProc *proc,
				ClientData clientData);
#endif
#ifndef Blt_Tree_ApplyDFS_DECLARED
#define Blt_Tree_ApplyDFS_DECLARED
/* 111 */
BLT_EXTERN int		Blt_Tree_ApplyDFS(Blt_TreeNode root,
				Blt_TreeApplyProc *proc,
				ClientData clientData, int order);
#endif
#ifndef Blt_Tree_ApplyBFS_DECLARED
#define Blt_Tree_ApplyBFS_DECLARED
/* 112 */
BLT_EXTERN int		Blt_Tree_ApplyBFS(Blt_TreeNode root,
				Blt_TreeApplyProc *proc,
				ClientData clientData);
#endif
#ifndef Blt_Tree_SortNode_DECLARED
#define Blt_Tree_SortNode_DECLARED
/* 113 */
BLT_EXTERN int		Blt_Tree_SortNode(Blt_Tree tree, Blt_TreeNode node,
				Blt_TreeCompareNodesProc *proc);
#endif
#ifndef Blt_Tree_Exists_DECLARED
#define Blt_Tree_Exists_DECLARED
/* 114 */
BLT_EXTERN int		Blt_Tree_Exists(Tcl_Interp *interp, const char *name);
#endif
#ifndef Blt_Tree_Open_DECLARED
#define Blt_Tree_Open_DECLARED
/* 115 */
BLT_EXTERN Blt_Tree	Blt_Tree_Open(Tcl_Interp *interp, const char *name,
				int flags);
#endif
#ifndef Blt_Tree_Close_DECLARED
#define Blt_Tree_Close_DECLARED
/* 116 */
BLT_EXTERN void		Blt_Tree_Close(Blt_Tree tree);
#endif
#ifndef Blt_Tree_Attach_DECLARED
#define Blt_Tree_Attach_DECLARED
/* 117 */
BLT_EXTERN int		Blt_Tree_Attach(Tcl_Interp *interp, Blt_Tree tree,
				const char *name);
#endif
#ifndef Blt_Tree_GetFromObj_DECLARED
#define Blt_Tree_GetFromObj_DECLARED
/* 118 */
BLT_EXTERN Blt_Tree	Blt_Tree_GetFromObj(Tcl_Interp *interp,
				Tcl_Obj *objPtr);
#endif
#ifndef Blt_Tree_Size_DECLARED
#define Blt_Tree_Size_DECLARED
/* 119 */
BLT_EXTERN int		Blt_Tree_Size(Blt_TreeNode node);
#endif
#ifndef Blt_Tree_CreateTrace_DECLARED
#define Blt_Tree_CreateTrace_DECLARED
/* 120 */
BLT_EXTERN Blt_TreeTrace Blt_Tree_CreateTrace(Blt_Tree tree,
				Blt_TreeNode node, const char *keyPattern,
				const char *tagName, unsigned int mask,
				Blt_TreeTraceProc *proc,
				ClientData clientData);
#endif
#ifndef Blt_Tree_DeleteTrace_DECLARED
#define Blt_Tree_DeleteTrace_DECLARED
/* 121 */
BLT_EXTERN void		Blt_Tree_DeleteTrace(Blt_TreeTrace token);
#endif
#ifndef Blt_Tree_CreateEventHandler_DECLARED
#define Blt_Tree_CreateEventHandler_DECLARED
/* 122 */
BLT_EXTERN void		Blt_Tree_CreateEventHandler(Blt_Tree tree,
				unsigned int mask,
				Blt_TreeNotifyEventProc *proc,
				ClientData clientData);
#endif
#ifndef Blt_Tree_DeleteEventHandler_DECLARED
#define Blt_Tree_DeleteEventHandler_DECLARED
/* 123 */
BLT_EXTERN void		Blt_Tree_DeleteEventHandler(Blt_Tree tree,
				unsigned int mask,
				Blt_TreeNotifyEventProc *proc,
				ClientData clientData);
#endif
#ifndef Blt_Tree_RelabelNode_DECLARED
#define Blt_Tree_RelabelNode_DECLARED
/* 124 */
BLT_EXTERN void		Blt_Tree_RelabelNode(Blt_Tree tree,
				Blt_TreeNode node, const char *string);
#endif
#ifndef Blt_Tree_RelabelNodeWithoutNotify_DECLARED
#define Blt_Tree_RelabelNodeWithoutNotify_DECLARED
/* 125 */
BLT_EXTERN void		Blt_Tree_RelabelNodeWithoutNotify(Blt_TreeNode node,
				const char *string);
#endif
#ifndef Blt_Tree_NodeIdAscii_DECLARED
#define Blt_Tree_NodeIdAscii_DECLARED
/* 126 */
BLT_EXTERN const char *	 Blt_Tree_NodeIdAscii(Blt_TreeNode node);
#endif
#ifndef Blt_Tree_NodePathObj_DECLARED
#define Blt_Tree_NodePathObj_DECLARED
/* 127 */
BLT_EXTERN Tcl_Obj *	Blt_Tree_NodePathObj(Blt_TreeNode node);
#endif
#ifndef Blt_Tree_NodePath_DECLARED
#define Blt_Tree_NodePath_DECLARED
/* 128 */
BLT_EXTERN const char *	 Blt_Tree_NodePath(Blt_TreeNode node);
#endif
#ifndef Blt_Tree_NodeRelativePath_DECLARED
#define Blt_Tree_NodeRelativePath_DECLARED
/* 129 */
BLT_EXTERN const char *	 Blt_Tree_NodeRelativePath(Blt_TreeNode root,
				Blt_TreeNode node, const char *separator,
				unsigned int flags, Tcl_Obj *resultPtr);
#endif
#ifndef Blt_Tree_NodePosition_DECLARED
#define Blt_Tree_NodePosition_DECLARED
/* 130 */
BLT_EXTERN long		Blt_Tree_NodePosition(Blt_TreeNode node);
#endif
#ifndef Blt_Tree_ClearTags_DECLARED
#define Blt_Tree_ClearTags_DECLARED
/* 131 */
BLT_EXTERN void		Blt_Tree_ClearTags(Blt_Tree tree, Blt_TreeNode node);
#endif
#ifndef Blt_Tree_HasTag_DECLARED
#define Blt_Tree_HasTag_DECLARED
/* 132 */
BLT_EXTERN int		Blt_Tree_HasTag(Blt_Tree tree, Blt_TreeNode node,
				const char *tagName);
#endif
#ifndef Blt_Tree_AddTag_DECLARED
#define Blt_Tree_AddTag_DECLARED
/* 133 */
BLT_EXTERN void		Blt_Tree_AddTag(Blt_Tree tree, Blt_TreeNode node,
				const char *tagName);
#endif
#ifndef Blt_Tree_RemoveTag_DECLARED
#define Blt_Tree_RemoveTag_DECLARED
/* 134 */
BLT_EXTERN void		Blt_Tree_RemoveTag(Blt_Tree tree, Blt_TreeNode node,
				const char *tagName);
#endif
#ifndef Blt_Tree_ForgetTag_DECLARED
#define Blt_Tree_ForgetTag_DECLARED
/* 135 */
BLT_EXTERN void		Blt_Tree_ForgetTag(Blt_Tree tree,
				const char *tagName);
#endif
#ifndef Blt_Tree_TagHashTable_DECLARED
#define Blt_Tree_TagHashTable_DECLARED
/* 136 */
BLT_EXTERN Blt_HashTable * Blt_Tree_TagHashTable(Blt_Tree tree,
				const char *tagName);
#endif
#ifndef Blt_Tree_TagTableIsShared_DECLARED
#define Blt_Tree_TagTableIsShared_DECLARED
/* 137 */
BLT_EXTERN int		Blt_Tree_TagTableIsShared(Blt_Tree tree);
#endif
#ifndef Blt_Tree_NewTagTable_DECLARED
#define Blt_Tree_NewTagTable_DECLARED
/* 138 */
BLT_EXTERN void		Blt_Tree_NewTagTable(Blt_Tree tree);
#endif
#ifndef Blt_Tree_FirstTag_DECLARED
#define Blt_Tree_FirstTag_DECLARED
/* 139 */
BLT_EXTERN Blt_HashEntry * Blt_Tree_FirstTag(Blt_Tree tree,
				Blt_HashSearch *searchPtr);
#endif
#ifndef Blt_Tree_Depth_DECLARED
#define Blt_Tree_Depth_DECLARED
/* 140 */
BLT_EXTERN long		Blt_Tree_Depth(Blt_Tree tree);
#endif
#ifndef Blt_Tree_RegisterFormat_DECLARED
#define Blt_Tree_RegisterFormat_DECLARED
/* 141 */
BLT_EXTERN int		Blt_Tree_RegisterFormat(Tcl_Interp *interp,
				const char *fmtName,
				Blt_TreeImportProc *importProc,
				Blt_TreeExportProc *exportProc);
#endif
#ifndef Blt_Tree_RememberTag_DECLARED
#define Blt_Tree_RememberTag_DECLARED
/* 142 */
BLT_EXTERN Blt_TreeTagEntry * Blt_Tree_RememberTag(Blt_Tree tree,
				const char *name);
#endif
#ifndef Blt_Tree_GetNodeFromObj_DECLARED
#define Blt_Tree_GetNodeFromObj_DECLARED
/* 143 */
BLT_EXTERN int		Blt_Tree_GetNodeFromObj(Tcl_Interp *interp,
				Blt_Tree tree, Tcl_Obj *objPtr,
				Blt_TreeNode *nodePtr);
#endif
#ifndef Blt_Tree_GetNodeIterator_DECLARED
#define Blt_Tree_GetNodeIterator_DECLARED
/* 144 */
BLT_EXTERN int		Blt_Tree_GetNodeIterator(Tcl_Interp *interp,
				Blt_Tree tree, Tcl_Obj *objPtr,
				Blt_TreeNodeIterator *iterPtr);
#endif
#ifndef Blt_Tree_FirstTaggedNode_DECLARED
#define Blt_Tree_FirstTaggedNode_DECLARED
/* 145 */
BLT_EXTERN Blt_TreeNode	 Blt_Tree_FirstTaggedNode(
				Blt_TreeNodeIterator *iterPtr);
#endif
#ifndef Blt_Tree_NextTaggedNode_DECLARED
#define Blt_Tree_NextTaggedNode_DECLARED
/* 146 */
BLT_EXTERN Blt_TreeNode	 Blt_Tree_NextTaggedNode(
				Blt_TreeNodeIterator *iterPtr);
#endif
#ifndef Blt_Tree_GetPathSeparator_DECLARED
#define Blt_Tree_GetPathSeparator_DECLARED
/* 147 */
BLT_EXTERN const char *	 Blt_Tree_GetPathSeparator(Blt_Tree tree);
#endif
#ifndef Blt_Tree_SetPathSeparator_DECLARED
#define Blt_Tree_SetPathSeparator_DECLARED
/* 148 */
BLT_EXTERN void		Blt_Tree_SetPathSeparator(Blt_Tree tree,
				const char *sep);
#endif
#ifndef blt_table_release_tags_DECLARED
#define blt_table_release_tags_DECLARED
/* 149 */
BLT_EXTERN void		blt_table_release_tags(BLT_TABLE table);
#endif
#ifndef blt_table_new_tags_DECLARED
#define blt_table_new_tags_DECLARED
/* 150 */
BLT_EXTERN void		blt_table_new_tags(BLT_TABLE table);
#endif
#ifndef blt_table_get_column_tag_table_DECLARED
#define blt_table_get_column_tag_table_DECLARED
/* 151 */
BLT_EXTERN Blt_HashTable * blt_table_get_column_tag_table(BLT_TABLE table);
#endif
#ifndef blt_table_get_row_tag_table_DECLARED
#define blt_table_get_row_tag_table_DECLARED
/* 152 */
BLT_EXTERN Blt_HashTable * blt_table_get_row_tag_table(BLT_TABLE table);
#endif
#ifndef blt_table_exists_DECLARED
#define blt_table_exists_DECLARED
/* 153 */
BLT_EXTERN int		blt_table_exists(Tcl_Interp *interp,
				const char *name);
#endif
#ifndef blt_table_create_DECLARED
#define blt_table_create_DECLARED
/* 154 */
BLT_EXTERN int		blt_table_create(Tcl_Interp *interp,
				const char *name, BLT_TABLE *tablePtr);
#endif
#ifndef blt_table_open_DECLARED
#define blt_table_open_DECLARED
/* 155 */
BLT_EXTERN int		blt_table_open(Tcl_Interp *interp, const char *name,
				BLT_TABLE *tablePtr);
#endif
#ifndef blt_table_close_DECLARED
#define blt_table_close_DECLARED
/* 156 */
BLT_EXTERN void		blt_table_close(BLT_TABLE table);
#endif
#ifndef blt_table_clear_DECLARED
#define blt_table_clear_DECLARED
/* 157 */
BLT_EXTERN void		blt_table_clear(BLT_TABLE table);
#endif
#ifndef blt_table_pack_DECLARED
#define blt_table_pack_DECLARED
/* 158 */
BLT_EXTERN void		blt_table_pack(BLT_TABLE table);
#endif
#ifndef blt_table_reset_DECLARED
#define blt_table_reset_DECLARED
/* 159 */
BLT_EXTERN void		blt_table_reset(BLT_TABLE table);
#endif
#ifndef blt_table_same_object_DECLARED
#define blt_table_same_object_DECLARED
/* 160 */
BLT_EXTERN int		blt_table_same_object(BLT_TABLE table1,
				BLT_TABLE table2);
#endif
#ifndef blt_table_row_get_label_table_DECLARED
#define blt_table_row_get_label_table_DECLARED
/* 161 */
BLT_EXTERN Blt_HashTable * blt_table_row_get_label_table(BLT_TABLE table,
				const char *label);
#endif
#ifndef blt_table_column_get_label_table_DECLARED
#define blt_table_column_get_label_table_DECLARED
/* 162 */
BLT_EXTERN Blt_HashTable * blt_table_column_get_label_table(BLT_TABLE table,
				const char *label);
#endif
#ifndef blt_table_get_row_DECLARED
#define blt_table_get_row_DECLARED
/* 163 */
BLT_EXTERN BLT_TABLE_ROW blt_table_get_row(Tcl_Interp *interp,
				BLT_TABLE table, Tcl_Obj *objPtr);
#endif
#ifndef blt_table_get_column_DECLARED
#define blt_table_get_column_DECLARED
/* 164 */
BLT_EXTERN BLT_TABLE_COLUMN blt_table_get_column(Tcl_Interp *interp,
				BLT_TABLE table, Tcl_Obj *objPtr);
#endif
#ifndef blt_table_get_row_by_label_DECLARED
#define blt_table_get_row_by_label_DECLARED
/* 165 */
BLT_EXTERN BLT_TABLE_ROW blt_table_get_row_by_label(BLT_TABLE table,
				const char *label);
#endif
#ifndef blt_table_get_column_by_label_DECLARED
#define blt_table_get_column_by_label_DECLARED
/* 166 */
BLT_EXTERN BLT_TABLE_COLUMN blt_table_get_column_by_label(BLT_TABLE table,
				const char *label);
#endif
#ifndef blt_table_get_row_by_index_DECLARED
#define blt_table_get_row_by_index_DECLARED
/* 167 */
BLT_EXTERN BLT_TABLE_ROW blt_table_get_row_by_index(BLT_TABLE table,
				long index);
#endif
#ifndef blt_table_get_column_by_index_DECLARED
#define blt_table_get_column_by_index_DECLARED
/* 168 */
BLT_EXTERN BLT_TABLE_COLUMN blt_table_get_column_by_index(BLT_TABLE table,
				long index);
#endif
#ifndef blt_table_set_row_label_DECLARED
#define blt_table_set_row_label_DECLARED
/* 169 */
BLT_EXTERN int		blt_table_set_row_label(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_ROW row,
				const char *label);
#endif
#ifndef blt_table_set_column_label_DECLARED
#define blt_table_set_column_label_DECLARED
/* 170 */
BLT_EXTERN int		blt_table_set_column_label(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_COLUMN column,
				const char *label);
#endif
#ifndef blt_table_name_to_column_type_DECLARED
#define blt_table_name_to_column_type_DECLARED
/* 171 */
BLT_EXTERN BLT_TABLE_COLUMN_TYPE blt_table_name_to_column_type(
				const char *typeName);
#endif
#ifndef blt_table_set_column_type_DECLARED
#define blt_table_set_column_type_DECLARED
/* 172 */
BLT_EXTERN int		blt_table_set_column_type(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_COLUMN column,
				BLT_TABLE_COLUMN_TYPE type);
#endif
#ifndef blt_table_column_type_to_name_DECLARED
#define blt_table_column_type_to_name_DECLARED
/* 173 */
BLT_EXTERN const char *	 blt_table_column_type_to_name(
				BLT_TABLE_COLUMN_TYPE type);
#endif
#ifndef blt_table_set_column_tag_DECLARED
#define blt_table_set_column_tag_DECLARED
/* 174 */
BLT_EXTERN int		blt_table_set_column_tag(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_COLUMN column,
				const char *tag);
#endif
#ifndef blt_table_set_row_tag_DECLARED
#define blt_table_set_row_tag_DECLARED
/* 175 */
BLT_EXTERN int		blt_table_set_row_tag(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_ROW row,
				const char *tag);
#endif
#ifndef blt_table_create_row_DECLARED
#define blt_table_create_row_DECLARED
/* 176 */
BLT_EXTERN BLT_TABLE_ROW blt_table_create_row(Tcl_Interp *interp,
				BLT_TABLE table, const char *label);
#endif
#ifndef blt_table_create_column_DECLARED
#define blt_table_create_column_DECLARED
/* 177 */
BLT_EXTERN BLT_TABLE_COLUMN blt_table_create_column(Tcl_Interp *interp,
				BLT_TABLE table, const char *label);
#endif
#ifndef blt_table_extend_rows_DECLARED
#define blt_table_extend_rows_DECLARED
/* 178 */
BLT_EXTERN int		blt_table_extend_rows(Tcl_Interp *interp,
				BLT_TABLE table, size_t n,
				BLT_TABLE_ROW *rows);
#endif
#ifndef blt_table_extend_columns_DECLARED
#define blt_table_extend_columns_DECLARED
/* 179 */
BLT_EXTERN int		blt_table_extend_columns(Tcl_Interp *interp,
				BLT_TABLE table, size_t n,
				BLT_TABLE_COLUMN *columms);
#endif
#ifndef blt_table_delete_row_DECLARED
#define blt_table_delete_row_DECLARED
/* 180 */
BLT_EXTERN int		blt_table_delete_row(BLT_TABLE table,
				BLT_TABLE_ROW row);
#endif
#ifndef blt_table_delete_column_DECLARED
#define blt_table_delete_column_DECLARED
/* 181 */
BLT_EXTERN int		blt_table_delete_column(BLT_TABLE table,
				BLT_TABLE_COLUMN column);
#endif
#ifndef blt_table_move_rows_DECLARED
#define blt_table_move_rows_DECLARED
/* 182 */
BLT_EXTERN int		blt_table_move_rows(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_ROW destRow,
				BLT_TABLE_ROW firstRow,
				BLT_TABLE_ROW lastRow, int after);
#endif
#ifndef blt_table_move_columns_DECLARED
#define blt_table_move_columns_DECLARED
/* 183 */
BLT_EXTERN int		blt_table_move_columns(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_COLUMN destColumn,
				BLT_TABLE_COLUMN firstColumn,
				BLT_TABLE_COLUMN lastColumn, int after);
#endif
#ifndef blt_table_get_obj_DECLARED
#define blt_table_get_obj_DECLARED
/* 184 */
BLT_EXTERN Tcl_Obj *	blt_table_get_obj(BLT_TABLE table, BLT_TABLE_ROW row,
				BLT_TABLE_COLUMN column);
#endif
#ifndef blt_table_set_obj_DECLARED
#define blt_table_set_obj_DECLARED
/* 185 */
BLT_EXTERN int		blt_table_set_obj(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_ROW row,
				BLT_TABLE_COLUMN column, Tcl_Obj *objPtr);
#endif
#ifndef blt_table_get_string_DECLARED
#define blt_table_get_string_DECLARED
/* 186 */
BLT_EXTERN const char *	 blt_table_get_string(BLT_TABLE table,
				BLT_TABLE_ROW row, BLT_TABLE_COLUMN column);
#endif
#ifndef blt_table_set_string_rep_DECLARED
#define blt_table_set_string_rep_DECLARED
/* 187 */
BLT_EXTERN int		blt_table_set_string_rep(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_ROW row,
				BLT_TABLE_COLUMN column, const char *string,
				int length);
#endif
#ifndef blt_table_set_string_DECLARED
#define blt_table_set_string_DECLARED
/* 188 */
BLT_EXTERN int		blt_table_set_string(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_ROW row,
				BLT_TABLE_COLUMN column, const char *string,
				int length);
#endif
#ifndef blt_table_append_string_DECLARED
#define blt_table_append_string_DECLARED
/* 189 */
BLT_EXTERN int		blt_table_append_string(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_ROW row,
				BLT_TABLE_COLUMN column, const char *string,
				int length);
#endif
#ifndef blt_table_set_bytes_DECLARED
#define blt_table_set_bytes_DECLARED
/* 190 */
BLT_EXTERN int		blt_table_set_bytes(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_ROW row,
				BLT_TABLE_COLUMN column,
				const unsigned char *string, int length);
#endif
#ifndef blt_table_get_double_DECLARED
#define blt_table_get_double_DECLARED
/* 191 */
BLT_EXTERN double	blt_table_get_double(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_ROW row,
				BLT_TABLE_COLUMN column);
#endif
#ifndef blt_table_set_double_DECLARED
#define blt_table_set_double_DECLARED
/* 192 */
BLT_EXTERN int		blt_table_set_double(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_ROW row,
				BLT_TABLE_COLUMN column, double value);
#endif
#ifndef blt_table_get_long_DECLARED
#define blt_table_get_long_DECLARED
/* 193 */
BLT_EXTERN long		blt_table_get_long(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_ROW row,
				BLT_TABLE_COLUMN column, long defValue);
#endif
#ifndef blt_table_set_long_DECLARED
#define blt_table_set_long_DECLARED
/* 194 */
BLT_EXTERN int		blt_table_set_long(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_ROW row,
				BLT_TABLE_COLUMN column, long value);
#endif
#ifndef blt_table_get_boolean_DECLARED
#define blt_table_get_boolean_DECLARED
/* 195 */
BLT_EXTERN int		blt_table_get_boolean(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_ROW row,
				BLT_TABLE_COLUMN column, int defValue);
#endif
#ifndef blt_table_set_boolean_DECLARED
#define blt_table_set_boolean_DECLARED
/* 196 */
BLT_EXTERN int		blt_table_set_boolean(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_ROW row,
				BLT_TABLE_COLUMN column, int value);
#endif
#ifndef blt_table_set_int64_DECLARED
#define blt_table_set_int64_DECLARED
/* 197 */
BLT_EXTERN int		blt_table_set_int64(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_ROW row,
				BLT_TABLE_COLUMN column, int64_t value);
#endif
#ifndef blt_table_get_int64_DECLARED
#define blt_table_get_int64_DECLARED
/* 198 */
BLT_EXTERN int64_t	blt_table_get_int64(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_ROW row,
				BLT_TABLE_COLUMN column, int64_t defValue);
#endif
#ifndef blt_table_get_value_DECLARED
#define blt_table_get_value_DECLARED
/* 199 */
BLT_EXTERN BLT_TABLE_VALUE blt_table_get_value(BLT_TABLE table,
				BLT_TABLE_ROW row, BLT_TABLE_COLUMN column);
#endif
#ifndef blt_table_set_value_DECLARED
#define blt_table_set_value_DECLARED
/* 200 */
BLT_EXTERN int		blt_table_set_value(BLT_TABLE table,
				BLT_TABLE_ROW row, BLT_TABLE_COLUMN column,
				BLT_TABLE_VALUE value);
#endif
#ifndef blt_table_unset_value_DECLARED
#define blt_table_unset_value_DECLARED
/* 201 */
BLT_EXTERN int		blt_table_unset_value(BLT_TABLE table,
				BLT_TABLE_ROW row, BLT_TABLE_COLUMN column);
#endif
#ifndef blt_table_value_exists_DECLARED
#define blt_table_value_exists_DECLARED
/* 202 */
BLT_EXTERN int		blt_table_value_exists(BLT_TABLE table,
				BLT_TABLE_ROW row, BLT_TABLE_COLUMN column);
#endif
#ifndef blt_table_value_string_DECLARED
#define blt_table_value_string_DECLARED
/* 203 */
BLT_EXTERN const char *	 blt_table_value_string(BLT_TABLE_VALUE value);
#endif
#ifndef blt_table_value_bytes_DECLARED
#define blt_table_value_bytes_DECLARED
/* 204 */
BLT_EXTERN const unsigned char * blt_table_value_bytes(BLT_TABLE_VALUE value);
#endif
#ifndef blt_table_value_length_DECLARED
#define blt_table_value_length_DECLARED
/* 205 */
BLT_EXTERN unsigned int	 blt_table_value_length(BLT_TABLE_VALUE value);
#endif
#ifndef blt_table_tags_are_shared_DECLARED
#define blt_table_tags_are_shared_DECLARED
/* 206 */
BLT_EXTERN int		blt_table_tags_are_shared(BLT_TABLE table);
#endif
#ifndef blt_table_clear_row_tags_DECLARED
#define blt_table_clear_row_tags_DECLARED
/* 207 */
BLT_EXTERN void		blt_table_clear_row_tags(BLT_TABLE table,
				BLT_TABLE_ROW row);
#endif
#ifndef blt_table_clear_column_tags_DECLARED
#define blt_table_clear_column_tags_DECLARED
/* 208 */
BLT_EXTERN void		blt_table_clear_column_tags(BLT_TABLE table,
				BLT_TABLE_COLUMN col);
#endif
#ifndef blt_table_get_row_tags_DECLARED
#define blt_table_get_row_tags_DECLARED
/* 209 */
BLT_EXTERN Blt_Chain	blt_table_get_row_tags(BLT_TABLE table,
				BLT_TABLE_ROW row);
#endif
#ifndef blt_table_get_column_tags_DECLARED
#define blt_table_get_column_tags_DECLARED
/* 210 */
BLT_EXTERN Blt_Chain	blt_table_get_column_tags(BLT_TABLE table,
				BLT_TABLE_COLUMN column);
#endif
#ifndef blt_table_get_tagged_rows_DECLARED
#define blt_table_get_tagged_rows_DECLARED
/* 211 */
BLT_EXTERN Blt_Chain	blt_table_get_tagged_rows(BLT_TABLE table,
				const char *tag);
#endif
#ifndef blt_table_get_tagged_columns_DECLARED
#define blt_table_get_tagged_columns_DECLARED
/* 212 */
BLT_EXTERN Blt_Chain	blt_table_get_tagged_columns(BLT_TABLE table,
				const char *tag);
#endif
#ifndef blt_table_row_has_tag_DECLARED
#define blt_table_row_has_tag_DECLARED
/* 213 */
BLT_EXTERN int		blt_table_row_has_tag(BLT_TABLE table,
				BLT_TABLE_ROW row, const char *tag);
#endif
#ifndef blt_table_column_has_tag_DECLARED
#define blt_table_column_has_tag_DECLARED
/* 214 */
BLT_EXTERN int		blt_table_column_has_tag(BLT_TABLE table,
				BLT_TABLE_COLUMN column, const char *tag);
#endif
#ifndef blt_table_forget_row_tag_DECLARED
#define blt_table_forget_row_tag_DECLARED
/* 215 */
BLT_EXTERN int		blt_table_forget_row_tag(Tcl_Interp *interp,
				BLT_TABLE table, const char *tag);
#endif
#ifndef blt_table_forget_column_tag_DECLARED
#define blt_table_forget_column_tag_DECLARED
/* 216 */
BLT_EXTERN int		blt_table_forget_column_tag(Tcl_Interp *interp,
				BLT_TABLE table, const char *tag);
#endif
#ifndef blt_table_unset_row_tag_DECLARED
#define blt_table_unset_row_tag_DECLARED
/* 217 */
BLT_EXTERN int		blt_table_unset_row_tag(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_ROW row,
				const char *tag);
#endif
#ifndef blt_table_unset_column_tag_DECLARED
#define blt_table_unset_column_tag_DECLARED
/* 218 */
BLT_EXTERN int		blt_table_unset_column_tag(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_COLUMN column,
				const char *tag);
#endif
#ifndef blt_table_first_column_DECLARED
#define blt_table_first_column_DECLARED
/* 219 */
BLT_EXTERN BLT_TABLE_COLUMN blt_table_first_column(BLT_TABLE table);
#endif
#ifndef blt_table_last_column_DECLARED
#define blt_table_last_column_DECLARED
/* 220 */
BLT_EXTERN BLT_TABLE_COLUMN blt_table_last_column(BLT_TABLE table);
#endif
#ifndef blt_table_next_column_DECLARED
#define blt_table_next_column_DECLARED
/* 221 */
BLT_EXTERN BLT_TABLE_COLUMN blt_table_next_column(BLT_TABLE_COLUMN column);
#endif
#ifndef blt_table_previous_column_DECLARED
#define blt_table_previous_column_DECLARED
/* 222 */
BLT_EXTERN BLT_TABLE_COLUMN blt_table_previous_column(
				BLT_TABLE_COLUMN column);
#endif
#ifndef blt_table_first_row_DECLARED
#define blt_table_first_row_DECLARED
/* 223 */
BLT_EXTERN BLT_TABLE_ROW blt_table_first_row(BLT_TABLE table);
#endif
#ifndef blt_table_last_row_DECLARED
#define blt_table_last_row_DECLARED
/* 224 */
BLT_EXTERN BLT_TABLE_ROW blt_table_last_row(BLT_TABLE table);
#endif
#ifndef blt_table_next_row_DECLARED
#define blt_table_next_row_DECLARED
/* 225 */
BLT_EXTERN BLT_TABLE_ROW blt_table_next_row(BLT_TABLE_ROW row);
#endif
#ifndef blt_table_previous_row_DECLARED
#define blt_table_previous_row_DECLARED
/* 226 */
BLT_EXTERN BLT_TABLE_ROW blt_table_previous_row(BLT_TABLE_ROW row);
#endif
#ifndef blt_table_row_spec_DECLARED
#define blt_table_row_spec_DECLARED
/* 227 */
BLT_EXTERN BLT_TABLE_ROWCOLUMN_SPEC blt_table_row_spec(BLT_TABLE table,
				Tcl_Obj *objPtr, const char **sp);
#endif
#ifndef blt_table_column_spec_DECLARED
#define blt_table_column_spec_DECLARED
/* 228 */
BLT_EXTERN BLT_TABLE_ROWCOLUMN_SPEC blt_table_column_spec(BLT_TABLE table,
				Tcl_Obj *objPtr, const char **sp);
#endif
#ifndef blt_table_iterate_rows_DECLARED
#define blt_table_iterate_rows_DECLARED
/* 229 */
BLT_EXTERN int		blt_table_iterate_rows(Tcl_Interp *interp,
				BLT_TABLE table, Tcl_Obj *objPtr,
				BLT_TABLE_ITERATOR *iter);
#endif
#ifndef blt_table_iterate_columns_DECLARED
#define blt_table_iterate_columns_DECLARED
/* 230 */
BLT_EXTERN int		blt_table_iterate_columns(Tcl_Interp *interp,
				BLT_TABLE table, Tcl_Obj *objPtr,
				BLT_TABLE_ITERATOR *iter);
#endif
#ifndef blt_table_iterate_rows_objv_DECLARED
#define blt_table_iterate_rows_objv_DECLARED
/* 231 */
BLT_EXTERN int		blt_table_iterate_rows_objv(Tcl_Interp *interp,
				BLT_TABLE table, int objc,
				Tcl_Obj *const *objv,
				BLT_TABLE_ITERATOR *iterPtr);
#endif
#ifndef blt_table_iterate_columns_objv_DECLARED
#define blt_table_iterate_columns_objv_DECLARED
/* 232 */
BLT_EXTERN int		blt_table_iterate_columns_objv(Tcl_Interp *interp,
				BLT_TABLE table, int objc,
				Tcl_Obj *const *objv,
				BLT_TABLE_ITERATOR *iterPtr);
#endif
#ifndef blt_table_free_iterator_objv_DECLARED
#define blt_table_free_iterator_objv_DECLARED
/* 233 */
BLT_EXTERN void		blt_table_free_iterator_objv(
				BLT_TABLE_ITERATOR *iterPtr);
#endif
#ifndef blt_table_iterate_all_rows_DECLARED
#define blt_table_iterate_all_rows_DECLARED
/* 234 */
BLT_EXTERN void		blt_table_iterate_all_rows(BLT_TABLE table,
				BLT_TABLE_ITERATOR *iterPtr);
#endif
#ifndef blt_table_iterate_all_columns_DECLARED
#define blt_table_iterate_all_columns_DECLARED
/* 235 */
BLT_EXTERN void		blt_table_iterate_all_columns(BLT_TABLE table,
				BLT_TABLE_ITERATOR *iterPtr);
#endif
#ifndef blt_table_first_tagged_row_DECLARED
#define blt_table_first_tagged_row_DECLARED
/* 236 */
BLT_EXTERN BLT_TABLE_ROW blt_table_first_tagged_row(BLT_TABLE_ITERATOR *iter);
#endif
#ifndef blt_table_first_tagged_column_DECLARED
#define blt_table_first_tagged_column_DECLARED
/* 237 */
BLT_EXTERN BLT_TABLE_COLUMN blt_table_first_tagged_column(
				BLT_TABLE_ITERATOR *iter);
#endif
#ifndef blt_table_next_tagged_row_DECLARED
#define blt_table_next_tagged_row_DECLARED
/* 238 */
BLT_EXTERN BLT_TABLE_ROW blt_table_next_tagged_row(BLT_TABLE_ITERATOR *iter);
#endif
#ifndef blt_table_next_tagged_column_DECLARED
#define blt_table_next_tagged_column_DECLARED
/* 239 */
BLT_EXTERN BLT_TABLE_COLUMN blt_table_next_tagged_column(
				BLT_TABLE_ITERATOR *iter);
#endif
#ifndef blt_table_list_rows_DECLARED
#define blt_table_list_rows_DECLARED
/* 240 */
BLT_EXTERN int		blt_table_list_rows(Tcl_Interp *interp,
				BLT_TABLE table, int objc,
				Tcl_Obj *const *objv, Blt_Chain chain);
#endif
#ifndef blt_table_list_columns_DECLARED
#define blt_table_list_columns_DECLARED
/* 241 */
BLT_EXTERN int		blt_table_list_columns(Tcl_Interp *interp,
				BLT_TABLE table, int objc,
				Tcl_Obj *const *objv, Blt_Chain chain);
#endif
#ifndef blt_table_clear_row_traces_DECLARED
#define blt_table_clear_row_traces_DECLARED
/* 242 */
BLT_EXTERN void		blt_table_clear_row_traces(BLT_TABLE table,
				BLT_TABLE_ROW row);
#endif
#ifndef blt_table_clear_column_traces_DECLARED
#define blt_table_clear_column_traces_DECLARED
/* 243 */
BLT_EXTERN void		blt_table_clear_column_traces(BLT_TABLE table,
				BLT_TABLE_COLUMN column);
#endif
#ifndef blt_table_create_trace_DECLARED
#define blt_table_create_trace_DECLARED
/* 244 */
BLT_EXTERN BLT_TABLE_TRACE blt_table_create_trace(BLT_TABLE table,
				BLT_TABLE_ROW row, BLT_TABLE_COLUMN column,
				const char *rowTag, const char *columnTag,
				unsigned int mask,
				BLT_TABLE_TRACE_PROC *proc,
				BLT_TABLE_TRACE_DELETE_PROC *deleteProc,
				ClientData clientData);
#endif
#ifndef blt_table_trace_column_DECLARED
#define blt_table_trace_column_DECLARED
/* 245 */
BLT_EXTERN void		blt_table_trace_column(BLT_TABLE table,
				BLT_TABLE_COLUMN column, unsigned int mask,
				BLT_TABLE_TRACE_PROC *proc,
				BLT_TABLE_TRACE_DELETE_PROC *deleteProc,
				ClientData clientData);
#endif
#ifndef blt_table_trace_row_DECLARED
#define blt_table_trace_row_DECLARED
/* 246 */
BLT_EXTERN void		blt_table_trace_row(BLT_TABLE table,
				BLT_TABLE_ROW row, unsigned int mask,
				BLT_TABLE_TRACE_PROC *proc,
				BLT_TABLE_TRACE_DELETE_PROC *deleteProc,
				ClientData clientData);
#endif
#ifndef blt_table_create_column_trace_DECLARED
#define blt_table_create_column_trace_DECLARED
/* 247 */
BLT_EXTERN BLT_TABLE_TRACE blt_table_create_column_trace(BLT_TABLE table,
				BLT_TABLE_COLUMN column, unsigned int mask,
				BLT_TABLE_TRACE_PROC *proc,
				BLT_TABLE_TRACE_DELETE_PROC *deleteProc,
				ClientData clientData);
#endif
#ifndef blt_table_create_column_tag_trace_DECLARED
#define blt_table_create_column_tag_trace_DECLARED
/* 248 */
BLT_EXTERN BLT_TABLE_TRACE blt_table_create_column_tag_trace(BLT_TABLE table,
				const char *tag, unsigned int mask,
				BLT_TABLE_TRACE_PROC *proc,
				BLT_TABLE_TRACE_DELETE_PROC *deleteProc,
				ClientData clientData);
#endif
#ifndef blt_table_create_row_trace_DECLARED
#define blt_table_create_row_trace_DECLARED
/* 249 */
BLT_EXTERN BLT_TABLE_TRACE blt_table_create_row_trace(BLT_TABLE table,
				BLT_TABLE_ROW row, unsigned int mask,
				BLT_TABLE_TRACE_PROC *proc,
				BLT_TABLE_TRACE_DELETE_PROC *deleteProc,
				ClientData clientData);
#endif
#ifndef blt_table_create_row_tag_trace_DECLARED
#define blt_table_create_row_tag_trace_DECLARED
/* 250 */
BLT_EXTERN BLT_TABLE_TRACE blt_table_create_row_tag_trace(BLT_TABLE table,
				const char *tag, unsigned int mask,
				BLT_TABLE_TRACE_PROC *proc,
				BLT_TABLE_TRACE_DELETE_PROC *deleteProc,
				ClientData clientData);
#endif
#ifndef blt_table_delete_trace_DECLARED
#define blt_table_delete_trace_DECLARED
/* 251 */
BLT_EXTERN void		blt_table_delete_trace(BLT_TABLE table,
				BLT_TABLE_TRACE trace);
#endif
#ifndef blt_table_create_notifier_DECLARED
#define blt_table_create_notifier_DECLARED
/* 252 */
BLT_EXTERN BLT_TABLE_NOTIFIER blt_table_create_notifier(Tcl_Interp *interp,
				BLT_TABLE table, unsigned int mask,
				BLT_TABLE_NOTIFY_EVENT_PROC *proc,
				BLT_TABLE_NOTIFIER_DELETE_PROC *deleteProc,
				ClientData clientData);
#endif
#ifndef blt_table_create_row_notifier_DECLARED
#define blt_table_create_row_notifier_DECLARED
/* 253 */
BLT_EXTERN BLT_TABLE_NOTIFIER blt_table_create_row_notifier(
				Tcl_Interp *interp, BLT_TABLE table,
				BLT_TABLE_ROW row, unsigned int mask,
				BLT_TABLE_NOTIFY_EVENT_PROC *proc,
				BLT_TABLE_NOTIFIER_DELETE_PROC *deleteProc,
				ClientData clientData);
#endif
#ifndef blt_table_create_row_tag_notifier_DECLARED
#define blt_table_create_row_tag_notifier_DECLARED
/* 254 */
BLT_EXTERN BLT_TABLE_NOTIFIER blt_table_create_row_tag_notifier(
				Tcl_Interp *interp, BLT_TABLE table,
				const char *tag, unsigned int mask,
				BLT_TABLE_NOTIFY_EVENT_PROC *proc,
				BLT_TABLE_NOTIFIER_DELETE_PROC *deleteProc,
				ClientData clientData);
#endif
#ifndef blt_table_create_column_notifier_DECLARED
#define blt_table_create_column_notifier_DECLARED
/* 255 */
BLT_EXTERN BLT_TABLE_NOTIFIER blt_table_create_column_notifier(
				Tcl_Interp *interp, BLT_TABLE table,
				BLT_TABLE_COLUMN column, unsigned int mask,
				BLT_TABLE_NOTIFY_EVENT_PROC *proc,
				BLT_TABLE_NOTIFIER_DELETE_PROC *deleteProc,
				ClientData clientData);
#endif
#ifndef blt_table_create_column_tag_notifier_DECLARED
#define blt_table_create_column_tag_notifier_DECLARED
/* 256 */
BLT_EXTERN BLT_TABLE_NOTIFIER blt_table_create_column_tag_notifier(
				Tcl_Interp *interp, BLT_TABLE table,
				const char *tag, unsigned int mask,
				BLT_TABLE_NOTIFY_EVENT_PROC *proc,
				BLT_TABLE_NOTIFIER_DELETE_PROC *deleteProc,
				ClientData clientData);
#endif
#ifndef blt_table_delete_notifier_DECLARED
#define blt_table_delete_notifier_DECLARED
/* 257 */
BLT_EXTERN void		blt_table_delete_notifier(BLT_TABLE table,
				BLT_TABLE_NOTIFIER notifier);
#endif
#ifndef blt_table_sort_init_DECLARED
#define blt_table_sort_init_DECLARED
/* 258 */
BLT_EXTERN void		blt_table_sort_init(BLT_TABLE table,
				BLT_TABLE_SORT_ORDER *order,
				long numCompares, unsigned int flags);
#endif
#ifndef blt_table_sort_rows_DECLARED
#define blt_table_sort_rows_DECLARED
/* 259 */
BLT_EXTERN BLT_TABLE_ROW * blt_table_sort_rows(BLT_TABLE table);
#endif
#ifndef blt_table_sort_row_map_DECLARED
#define blt_table_sort_row_map_DECLARED
/* 260 */
BLT_EXTERN void		blt_table_sort_row_map(BLT_TABLE table, long numRows,
				BLT_TABLE_ROW *rows);
#endif
#ifndef blt_table_sort_finish_DECLARED
#define blt_table_sort_finish_DECLARED
/* 261 */
BLT_EXTERN void		blt_table_sort_finish(void );
#endif
#ifndef blt_table_get_compare_proc_DECLARED
#define blt_table_get_compare_proc_DECLARED
/* 262 */
BLT_EXTERN BLT_TABLE_COMPARE_PROC * blt_table_get_compare_proc(
				BLT_TABLE table, BLT_TABLE_COLUMN column,
				unsigned int flags);
#endif
#ifndef blt_table_get_row_map_DECLARED
#define blt_table_get_row_map_DECLARED
/* 263 */
BLT_EXTERN BLT_TABLE_ROW * blt_table_get_row_map(BLT_TABLE table);
#endif
#ifndef blt_table_get_column_map_DECLARED
#define blt_table_get_column_map_DECLARED
/* 264 */
BLT_EXTERN BLT_TABLE_COLUMN * blt_table_get_column_map(BLT_TABLE table);
#endif
#ifndef blt_table_set_row_map_DECLARED
#define blt_table_set_row_map_DECLARED
/* 265 */
BLT_EXTERN void		blt_table_set_row_map(BLT_TABLE table,
				BLT_TABLE_ROW *map);
#endif
#ifndef blt_table_set_column_map_DECLARED
#define blt_table_set_column_map_DECLARED
/* 266 */
BLT_EXTERN void		blt_table_set_column_map(BLT_TABLE table,
				BLT_TABLE_COLUMN *map);
#endif
#ifndef blt_table_restore_DECLARED
#define blt_table_restore_DECLARED
/* 267 */
BLT_EXTERN int		blt_table_restore(Tcl_Interp *interp,
				BLT_TABLE table, char *string,
				unsigned int flags);
#endif
#ifndef blt_table_file_restore_DECLARED
#define blt_table_file_restore_DECLARED
/* 268 */
BLT_EXTERN int		blt_table_file_restore(Tcl_Interp *interp,
				BLT_TABLE table, const char *fileName,
				unsigned int flags);
#endif
#ifndef blt_table_register_format_DECLARED
#define blt_table_register_format_DECLARED
/* 269 */
BLT_EXTERN int		blt_table_register_format(Tcl_Interp *interp,
				const char *name,
				BLT_TABLE_IMPORT_PROC *importProc,
				BLT_TABLE_EXPORT_PROC *exportProc);
#endif
#ifndef blt_table_unset_keys_DECLARED
#define blt_table_unset_keys_DECLARED
/* 270 */
BLT_EXTERN void		blt_table_unset_keys(BLT_TABLE table);
#endif
#ifndef blt_table_get_keys_DECLARED
#define blt_table_get_keys_DECLARED
/* 271 */
BLT_EXTERN int		blt_table_get_keys(BLT_TABLE table,
				BLT_TABLE_COLUMN **keysPtr);
#endif
#ifndef blt_table_set_keys_DECLARED
#define blt_table_set_keys_DECLARED
/* 272 */
BLT_EXTERN int		blt_table_set_keys(BLT_TABLE table, int numKeys,
				BLT_TABLE_COLUMN *keys, int unique);
#endif
#ifndef blt_table_key_lookup_DECLARED
#define blt_table_key_lookup_DECLARED
/* 273 */
BLT_EXTERN int		blt_table_key_lookup(Tcl_Interp *interp,
				BLT_TABLE table, int objc,
				Tcl_Obj *const *objv, BLT_TABLE_ROW *rowPtr);
#endif
#ifndef blt_table_get_column_limits_DECLARED
#define blt_table_get_column_limits_DECLARED
/* 274 */
BLT_EXTERN int		blt_table_get_column_limits(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_COLUMN col,
				Tcl_Obj **minObjPtrPtr,
				Tcl_Obj **maxObjPtrPtr);
#endif
#ifndef blt_table_row_DECLARED
#define blt_table_row_DECLARED
/* 275 */
BLT_EXTERN BLT_TABLE_ROW blt_table_row(BLT_TABLE table, long index);
#endif
#ifndef blt_table_column_DECLARED
#define blt_table_column_DECLARED
/* 276 */
BLT_EXTERN BLT_TABLE_COLUMN blt_table_column(BLT_TABLE table, long index);
#endif
#ifndef blt_table_row_index_DECLARED
#define blt_table_row_index_DECLARED
/* 277 */
BLT_EXTERN long		blt_table_row_index(BLT_TABLE table,
				BLT_TABLE_ROW row);
#endif
#ifndef blt_table_column_index_DECLARED
#define blt_table_column_index_DECLARED
/* 278 */
BLT_EXTERN long		blt_table_column_index(BLT_TABLE table,
				BLT_TABLE_COLUMN column);
#endif
#ifndef Blt_VecMin_DECLARED
#define Blt_VecMin_DECLARED
/* 279 */
BLT_EXTERN double	Blt_VecMin(Blt_Vector *vPtr);
#endif
#ifndef Blt_VecMax_DECLARED
#define Blt_VecMax_DECLARED
/* 280 */
BLT_EXTERN double	Blt_VecMax(Blt_Vector *vPtr);
#endif
#ifndef Blt_AllocVectorId_DECLARED
#define Blt_AllocVectorId_DECLARED
/* 281 */
BLT_EXTERN Blt_VectorId	 Blt_AllocVectorId(Tcl_Interp *interp,
				const char *vecName);
#endif
#ifndef Blt_SetVectorChangedProc_DECLARED
#define Blt_SetVectorChangedProc_DECLARED
/* 282 */
BLT_EXTERN void		Blt_SetVectorChangedProc(Blt_VectorId clientId,
				Blt_VectorChangedProc *proc,
				ClientData clientData);
#endif
#ifndef Blt_FreeVectorId_DECLARED
#define Blt_FreeVectorId_DECLARED
/* 283 */
BLT_EXTERN void		Blt_FreeVectorId(Blt_VectorId clientId);
#endif
#ifndef Blt_GetVectorById_DECLARED
#define Blt_GetVectorById_DECLARED
/* 284 */
BLT_EXTERN int		Blt_GetVectorById(Tcl_Interp *interp,
				Blt_VectorId clientId,
				Blt_Vector **vecPtrPtr);
#endif
#ifndef Blt_NameOfVectorId_DECLARED
#define Blt_NameOfVectorId_DECLARED
/* 285 */
BLT_EXTERN const char *	 Blt_NameOfVectorId(Blt_VectorId clientId);
#endif
#ifndef Blt_NameOfVector_DECLARED
#define Blt_NameOfVector_DECLARED
/* 286 */
BLT_EXTERN const char *	 Blt_NameOfVector(Blt_Vector *vecPtr);
#endif
#ifndef Blt_VectorNotifyPending_DECLARED
#define Blt_VectorNotifyPending_DECLARED
/* 287 */
BLT_EXTERN int		Blt_VectorNotifyPending(Blt_VectorId clientId);
#endif
#ifndef Blt_CreateVector_DECLARED
#define Blt_CreateVector_DECLARED
/* 288 */
BLT_EXTERN int		Blt_CreateVector(Tcl_Interp *interp,
				const char *vecName, int size,
				Blt_Vector **vecPtrPtr);
#endif
#ifndef Blt_CreateVector2_DECLARED
#define Blt_CreateVector2_DECLARED
/* 289 */
BLT_EXTERN int		Blt_CreateVector2(Tcl_Interp *interp,
				const char *vecName, const char *cmdName,
				const char *varName, int initialSize,
				Blt_Vector **vecPtrPtr);
#endif
#ifndef Blt_GetVector_DECLARED
#define Blt_GetVector_DECLARED
/* 290 */
BLT_EXTERN int		Blt_GetVector(Tcl_Interp *interp,
				const char *vecName, Blt_Vector **vecPtrPtr);
#endif
#ifndef Blt_GetVectorFromObj_DECLARED
#define Blt_GetVectorFromObj_DECLARED
/* 291 */
BLT_EXTERN int		Blt_GetVectorFromObj(Tcl_Interp *interp,
				Tcl_Obj *objPtr, Blt_Vector **vecPtrPtr);
#endif
#ifndef Blt_VectorExists_DECLARED
#define Blt_VectorExists_DECLARED
/* 292 */
BLT_EXTERN int		Blt_VectorExists(Tcl_Interp *interp,
				const char *vecName);
#endif
#ifndef Blt_ResetVector_DECLARED
#define Blt_ResetVector_DECLARED
/* 293 */
BLT_EXTERN int		Blt_ResetVector(Blt_Vector *vecPtr, double *dataArr,
				int n, int arraySize, Tcl_FreeProc *freeProc);
#endif
#ifndef Blt_ResizeVector_DECLARED
#define Blt_ResizeVector_DECLARED
/* 294 */
BLT_EXTERN int		Blt_ResizeVector(Blt_Vector *vecPtr, int n);
#endif
#ifndef Blt_DeleteVectorByName_DECLARED
#define Blt_DeleteVectorByName_DECLARED
/* 295 */
BLT_EXTERN int		Blt_DeleteVectorByName(Tcl_Interp *interp,
				const char *vecName);
#endif
#ifndef Blt_DeleteVector_DECLARED
#define Blt_DeleteVector_DECLARED
/* 296 */
BLT_EXTERN int		Blt_DeleteVector(Blt_Vector *vecPtr);
#endif
#ifndef Blt_ExprVector_DECLARED
#define Blt_ExprVector_DECLARED
/* 297 */
BLT_EXTERN int		Blt_ExprVector(Tcl_Interp *interp, char *expr,
				Blt_Vector *vecPtr);
#endif
#ifndef Blt_InstallIndexProc_DECLARED
#define Blt_InstallIndexProc_DECLARED
/* 298 */
BLT_EXTERN void		Blt_InstallIndexProc(Tcl_Interp *interp,
				const char *indexName,
				Blt_VectorIndexProc *procPtr);
#endif
#ifndef Blt_VectorExists2_DECLARED
#define Blt_VectorExists2_DECLARED
/* 299 */
BLT_EXTERN int		Blt_VectorExists2(Tcl_Interp *interp,
				const char *vecName);
#endif

typedef struct BltTclStubHooks {
    struct BltTclIntProcs *bltTclIntProcs;
} BltTclStubHooks;

typedef struct BltTclProcs {
    int magic;
    struct BltTclStubHooks *hooks;

    void *reserved0;
    void (*blt_AllocInit) (Blt_MallocProc *mallocProc, Blt_ReallocProc *reallocProc, Blt_FreeProc *freeProc); /* 1 */
    VOID * (*blt_Malloc) (size_t size); /* 2 */
    void (*blt_Free) (const VOID *mem); /* 3 */
    VOID * (*blt_Realloc) (VOID *ptr, size_t size); /* 4 */
    VOID * (*blt_Calloc) (size_t numElem, size_t elemSize); /* 5 */
    VOID * (*blt_MallocAbortOnError) (size_t size, const char *fileName, int lineNum); /* 6 */
    VOID * (*blt_CallocAbortOnError) (size_t numElem, size_t elemSize, const char *fileName, int lineNum); /* 7 */
    VOID * (*blt_ReallocAbortOnError) (VOID *ptr, size_t size, const char *fileName, int lineNum); /* 8 */
    const char * (*blt_Strndup) (const char *string, size_t size); /* 9 */
    const char * (*blt_Strdup) (const char *string); /* 10 */
    const char * (*blt_StrdupAbortOnError) (const char *string, const char *fileName, int lineNum); /* 11 */
    const char * (*blt_StrndupAbortOnError) (const char *string, size_t size, const char *fileName, int lineNum); /* 12 */
    void (*blt_Chain_Init) (Blt_Chain chain); /* 13 */
    Blt_Chain (*blt_Chain_Create) (void); /* 14 */
    void (*blt_Chain_Destroy) (Blt_Chain chain); /* 15 */
    Blt_ChainLink (*blt_Chain_NewLink) (void); /* 16 */
    Blt_ChainLink (*blt_Chain_AllocLink) (size_t size); /* 17 */
    Blt_ChainLink (*blt_Chain_Append) (Blt_Chain chain, ClientData clientData); /* 18 */
    Blt_ChainLink (*blt_Chain_Prepend) (Blt_Chain chain, ClientData clientData); /* 19 */
    void (*blt_Chain_Reset) (Blt_Chain chain); /* 20 */
    void (*blt_Chain_InitLink) (Blt_ChainLink link); /* 21 */
    void (*blt_Chain_LinkAfter) (Blt_Chain chain, Blt_ChainLink link, Blt_ChainLink after); /* 22 */
    void (*blt_Chain_LinkBefore) (Blt_Chain chain, Blt_ChainLink link, Blt_ChainLink before); /* 23 */
    void (*blt_Chain_UnlinkLink) (Blt_Chain chain, Blt_ChainLink link); /* 24 */
    void (*blt_Chain_DeleteLink) (Blt_Chain chain, Blt_ChainLink link); /* 25 */
    Blt_ChainLink (*blt_Chain_GetNthLink) (Blt_Chain chain, long position); /* 26 */
    void (*blt_Chain_Sort) (Blt_Chain chain, Blt_ChainCompareProc *proc); /* 27 */
    void (*blt_Chain_Reverse) (Blt_Chain chain); /* 28 */
    int (*blt_Chain_IsBefore) (Blt_ChainLink first, Blt_ChainLink last); /* 29 */
    void (*blt_InitHashTable) (Blt_HashTable *tablePtr, size_t keyType); /* 30 */
    void (*blt_InitHashTableWithPool) (Blt_HashTable *tablePtr, size_t keyType); /* 31 */
    void (*blt_DeleteHashTable) (Blt_HashTable *tablePtr); /* 32 */
    void (*blt_DeleteHashEntry) (Blt_HashTable *tablePtr, Blt_HashEntry *entryPtr); /* 33 */
    Blt_HashEntry * (*blt_FirstHashEntry) (Blt_HashTable *tablePtr, Blt_HashSearch *searchPtr); /* 34 */
    Blt_HashEntry * (*blt_NextHashEntry) (Blt_HashSearch *srchPtr); /* 35 */
    const char * (*blt_HashStats) (Blt_HashTable *tablePtr); /* 36 */
    Blt_Tags (*blt_Tags_Create) (void); /* 37 */
    void (*blt_Tags_Destroy) (Blt_Tags tags); /* 38 */
    void (*blt_Tags_Init) (Blt_Tags tags); /* 39 */
    void (*blt_Tags_Reset) (Blt_Tags tags); /* 40 */
    int (*blt_Tags_ItemHasTag) (Blt_Tags tags, ClientData item, const char *tag); /* 41 */
    void (*blt_Tags_AddTag) (Blt_Tags tags, const char *tag); /* 42 */
    void (*blt_Tags_AddItemToTag) (Blt_Tags tags, const char *tag, ClientData item); /* 43 */
    void (*blt_Tags_ForgetTag) (Blt_Tags tags, const char *tag); /* 44 */
    void (*blt_Tags_RemoveItemFromTag) (Blt_Tags tags, const char *tag, ClientData item); /* 45 */
    void (*blt_Tags_ClearTagsFromItem) (Blt_Tags tags, ClientData item); /* 46 */
    void (*blt_Tags_AppendTagsToChain) (Blt_Tags tags, ClientData item, Blt_Chain list); /* 47 */
    void (*blt_Tags_AppendTagsToObj) (Blt_Tags tags, ClientData item, Tcl_Obj *objPtr); /* 48 */
    void (*blt_Tags_AppendAllTagsToObj) (Blt_Tags tags, Tcl_Obj *objPtr); /* 49 */
    Blt_Chain (*blt_Tags_GetItemList) (Blt_Tags tags, const char *tag); /* 50 */
    Blt_HashTable * (*blt_Tags_GetTable) (Blt_Tags tags); /* 51 */
    void (*blt_List_Init) (Blt_List list, size_t type); /* 52 */
    void (*blt_List_Reset) (Blt_List list); /* 53 */
    Blt_List (*blt_List_Create) (size_t type); /* 54 */
    void (*blt_List_Destroy) (Blt_List list); /* 55 */
    Blt_ListNode (*blt_List_CreateNode) (Blt_List list, const char *key); /* 56 */
    void (*blt_List_DeleteNode) (Blt_ListNode node); /* 57 */
    Blt_ListNode (*blt_List_Append) (Blt_List list, const char *key, ClientData clientData); /* 58 */
    Blt_ListNode (*blt_List_Prepend) (Blt_List list, const char *key, ClientData clientData); /* 59 */
    void (*blt_List_LinkAfter) (Blt_List list, Blt_ListNode node, Blt_ListNode afterNode); /* 60 */
    void (*blt_List_LinkBefore) (Blt_List list, Blt_ListNode node, Blt_ListNode beforeNode); /* 61 */
    void (*blt_List_UnlinkNode) (Blt_ListNode node); /* 62 */
    Blt_ListNode (*blt_List_GetNode) (Blt_List list, const char *key); /* 63 */
    void (*blt_List_DeleteNodeByKey) (Blt_List list, const char *key); /* 64 */
    Blt_ListNode (*blt_List_GetNthNode) (Blt_List list, long position, int direction); /* 65 */
    void (*blt_List_Sort) (Blt_List list, Blt_ListCompareProc *proc); /* 66 */
    Blt_Pool (*blt_Pool_Create) (int type); /* 67 */
    void (*blt_Pool_Destroy) (Blt_Pool pool); /* 68 */
    Blt_TreeUid (*blt_Tree_GetUid) (Blt_Tree tree, const char *string); /* 69 */
    Blt_TreeUid (*blt_Tree_GetUidFromNode) (Blt_TreeNode node, const char *string); /* 70 */
    long (*blt_Tree_GetNextId) (Blt_Tree tree); /* 71 */
    Blt_TreeNode (*blt_Tree_CreateNode) (Blt_Tree tree, Blt_TreeNode parent, const char *name, Blt_TreeNode before); /* 72 */
    Blt_TreeNode (*blt_Tree_CreateNodeWithId) (Blt_Tree tree, Blt_TreeNode parent, const char *name, long inode, Blt_TreeNode before); /* 73 */
    int (*blt_Tree_DeleteNode) (Blt_Tree tree, Blt_TreeNode node); /* 74 */
    int (*blt_Tree_MoveNode) (Blt_Tree tree, Blt_TreeNode node, Blt_TreeNode parent, Blt_TreeNode before); /* 75 */
    Blt_TreeNode (*blt_Tree_GetNodeFromIndex) (Blt_Tree tree, long inode); /* 76 */
    Blt_TreeNode (*blt_Tree_FindChild) (Blt_TreeNode parent, const char *name); /* 77 */
    Blt_TreeNode (*blt_Tree_NextNode) (Blt_TreeNode root, Blt_TreeNode node); /* 78 */
    Blt_TreeNode (*blt_Tree_PrevNode) (Blt_TreeNode root, Blt_TreeNode node); /* 79 */
    Blt_TreeNode (*blt_Tree_FirstChild) (Blt_TreeNode parent); /* 80 */
    Blt_TreeNode (*blt_Tree_LastChild) (Blt_TreeNode parent); /* 81 */
    int (*blt_Tree_IsBefore) (Blt_TreeNode node1, Blt_TreeNode node2); /* 82 */
    int (*blt_Tree_IsAncestor) (Blt_TreeNode node1, Blt_TreeNode node2); /* 83 */
    int (*blt_Tree_PrivateValue) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, Blt_TreeUid uid); /* 84 */
    int (*blt_Tree_PublicValue) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, Blt_TreeUid uid); /* 85 */
    int (*blt_Tree_GetValue) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, const char *string, Tcl_Obj **valuePtr); /* 86 */
    int (*blt_Tree_ValueExists) (Blt_Tree tree, Blt_TreeNode node, const char *string); /* 87 */
    int (*blt_Tree_SetValue) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, const char *string, Tcl_Obj *valuePtr); /* 88 */
    int (*blt_Tree_UnsetValue) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, const char *string); /* 89 */
    int (*blt_Tree_AppendObjValue) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, const char *string, Tcl_Obj *objPtr); /* 90 */
    int (*blt_Tree_ListAppendObjValue) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, const char *string, Tcl_Obj *objPtr); /* 91 */
    int (*blt_Tree_ListReplaceObjValues) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, const char *string, int firstIndex, int lastIndex, int objc, Tcl_Obj *const *objv); /* 92 */
    int (*blt_Tree_GetArrayObjValue) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, const char *arrayName, const char *elemName, Tcl_Obj **valueObjPtrPtr); /* 93 */
    int (*blt_Tree_SetArrayValue) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, const char *arrayName, const char *elemName, Tcl_Obj *valueObjPtr); /* 94 */
    int (*blt_Tree_UnsetArrayValue) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, const char *arrayName, const char *elemName); /* 95 */
    int (*blt_Tree_AppendArrayObjValue) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, const char *arrayName, const char *elemName, Tcl_Obj *objPtr); /* 96 */
    int (*blt_Tree_ListAppendArrayObjValue) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, const char *arrayName, const char *elemName, Tcl_Obj *valueObjPtr); /* 97 */
    int (*blt_Tree_ListReplaceArrayObjValues) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, const char *string, const char *elemName, int firstIndex, int lastIndex, int objc, Tcl_Obj *const *objv); /* 98 */
    int (*blt_Tree_ArrayValueExists) (Blt_Tree tree, Blt_TreeNode node, const char *arrayName, const char *elemName); /* 99 */
    int (*blt_Tree_ArrayNames) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, const char *arrayName, Tcl_Obj *listObjPtr); /* 100 */
    int (*blt_Tree_GetScalarValueByUid) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, Blt_TreeUid uid, Tcl_Obj **valuePtr); /* 101 */
    int (*blt_Tree_SetScalarValueByUid) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, Blt_TreeUid uid, Tcl_Obj *valuePtr); /* 102 */
    int (*blt_Tree_UnsetScalarValueByUid) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, Blt_TreeUid uid); /* 103 */
    int (*blt_Tree_AppendScalarObjValueByUid) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, Blt_TreeUid uid, Tcl_Obj *objPtr); /* 104 */
    int (*blt_Tree_ListAppendScalarObjValueByUid) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, Blt_TreeUid uid, Tcl_Obj *objPtr); /* 105 */
    int (*blt_Tree_ListReplaceScalarObjValuesByUid) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, Blt_TreeUid uid, int firstIndex, int lastIndex, int objc, Tcl_Obj *const *objv); /* 106 */
    int (*blt_Tree_ScalarValueExistsByUid) (Blt_Tree tree, Blt_TreeNode node, Blt_TreeUid uid); /* 107 */
    Blt_TreeUid (*blt_Tree_FirstValue) (Blt_Tree tree, Blt_TreeNode node, Blt_TreeValueIterator *iterPtr); /* 108 */
    Blt_TreeUid (*blt_Tree_NextValue) (Blt_Tree tree, Blt_TreeValueIterator *iterPtr); /* 109 */
    int (*blt_Tree_Apply) (Blt_TreeNode root, Blt_TreeApplyProc *proc, ClientData clientData); /* 110 */
    int (*blt_Tree_ApplyDFS) (Blt_TreeNode root, Blt_TreeApplyProc *proc, ClientData clientData, int order); /* 111 */
    int (*blt_Tree_ApplyBFS) (Blt_TreeNode root, Blt_TreeApplyProc *proc, ClientData clientData); /* 112 */
    int (*blt_Tree_SortNode) (Blt_Tree tree, Blt_TreeNode node, Blt_TreeCompareNodesProc *proc); /* 113 */
    int (*blt_Tree_Exists) (Tcl_Interp *interp, const char *name); /* 114 */
    Blt_Tree (*blt_Tree_Open) (Tcl_Interp *interp, const char *name, int flags); /* 115 */
    void (*blt_Tree_Close) (Blt_Tree tree); /* 116 */
    int (*blt_Tree_Attach) (Tcl_Interp *interp, Blt_Tree tree, const char *name); /* 117 */
    Blt_Tree (*blt_Tree_GetFromObj) (Tcl_Interp *interp, Tcl_Obj *objPtr); /* 118 */
    int (*blt_Tree_Size) (Blt_TreeNode node); /* 119 */
    Blt_TreeTrace (*blt_Tree_CreateTrace) (Blt_Tree tree, Blt_TreeNode node, const char *keyPattern, const char *tagName, unsigned int mask, Blt_TreeTraceProc *proc, ClientData clientData); /* 120 */
    void (*blt_Tree_DeleteTrace) (Blt_TreeTrace token); /* 121 */
    void (*blt_Tree_CreateEventHandler) (Blt_Tree tree, unsigned int mask, Blt_TreeNotifyEventProc *proc, ClientData clientData); /* 122 */
    void (*blt_Tree_DeleteEventHandler) (Blt_Tree tree, unsigned int mask, Blt_TreeNotifyEventProc *proc, ClientData clientData); /* 123 */
    void (*blt_Tree_RelabelNode) (Blt_Tree tree, Blt_TreeNode node, const char *string); /* 124 */
    void (*blt_Tree_RelabelNodeWithoutNotify) (Blt_TreeNode node, const char *string); /* 125 */
    const char * (*blt_Tree_NodeIdAscii) (Blt_TreeNode node); /* 126 */
    Tcl_Obj * (*blt_Tree_NodePathObj) (Blt_TreeNode node); /* 127 */
    const char * (*blt_Tree_NodePath) (Blt_TreeNode node); /* 128 */
    const char * (*blt_Tree_NodeRelativePath) (Blt_TreeNode root, Blt_TreeNode node, const char *separator, unsigned int flags, Tcl_Obj *resultPtr); /* 129 */
    long (*blt_Tree_NodePosition) (Blt_TreeNode node); /* 130 */
    void (*blt_Tree_ClearTags) (Blt_Tree tree, Blt_TreeNode node); /* 131 */
    int (*blt_Tree_HasTag) (Blt_Tree tree, Blt_TreeNode node, const char *tagName); /* 132 */
    void (*blt_Tree_AddTag) (Blt_Tree tree, Blt_TreeNode node, const char *tagName); /* 133 */
    void (*blt_Tree_RemoveTag) (Blt_Tree tree, Blt_TreeNode node, const char *tagName); /* 134 */
    void (*blt_Tree_ForgetTag) (Blt_Tree tree, const char *tagName); /* 135 */
    Blt_HashTable * (*blt_Tree_TagHashTable) (Blt_Tree tree, const char *tagName); /* 136 */
    int (*blt_Tree_TagTableIsShared) (Blt_Tree tree); /* 137 */
    void (*blt_Tree_NewTagTable) (Blt_Tree tree); /* 138 */
    Blt_HashEntry * (*blt_Tree_FirstTag) (Blt_Tree tree, Blt_HashSearch *searchPtr); /* 139 */
    long (*blt_Tree_Depth) (Blt_Tree tree); /* 140 */
    int (*blt_Tree_RegisterFormat) (Tcl_Interp *interp, const char *fmtName, Blt_TreeImportProc *importProc, Blt_TreeExportProc *exportProc); /* 141 */
    Blt_TreeTagEntry * (*blt_Tree_RememberTag) (Blt_Tree tree, const char *name); /* 142 */
    int (*blt_Tree_GetNodeFromObj) (Tcl_Interp *interp, Blt_Tree tree, Tcl_Obj *objPtr, Blt_TreeNode *nodePtr); /* 143 */
    int (*blt_Tree_GetNodeIterator) (Tcl_Interp *interp, Blt_Tree tree, Tcl_Obj *objPtr, Blt_TreeNodeIterator *iterPtr); /* 144 */
    Blt_TreeNode (*blt_Tree_FirstTaggedNode) (Blt_TreeNodeIterator *iterPtr); /* 145 */
    Blt_TreeNode (*blt_Tree_NextTaggedNode) (Blt_TreeNodeIterator *iterPtr); /* 146 */
    const char * (*blt_Tree_GetPathSeparator) (Blt_Tree tree); /* 147 */
    void (*blt_Tree_SetPathSeparator) (Blt_Tree tree, const char *sep); /* 148 */
    void (*blt_table_release_tags) (BLT_TABLE table); /* 149 */
    void (*blt_table_new_tags) (BLT_TABLE table); /* 150 */
    Blt_HashTable * (*blt_table_get_column_tag_table) (BLT_TABLE table); /* 151 */
    Blt_HashTable * (*blt_table_get_row_tag_table) (BLT_TABLE table); /* 152 */
    int (*blt_table_exists) (Tcl_Interp *interp, const char *name); /* 153 */
    int (*blt_table_create) (Tcl_Interp *interp, const char *name, BLT_TABLE *tablePtr); /* 154 */
    int (*blt_table_open) (Tcl_Interp *interp, const char *name, BLT_TABLE *tablePtr); /* 155 */
    void (*blt_table_close) (BLT_TABLE table); /* 156 */
    void (*blt_table_clear) (BLT_TABLE table); /* 157 */
    void (*blt_table_pack) (BLT_TABLE table); /* 158 */
    void (*blt_table_reset) (BLT_TABLE table); /* 159 */
    int (*blt_table_same_object) (BLT_TABLE table1, BLT_TABLE table2); /* 160 */
    Blt_HashTable * (*blt_table_row_get_label_table) (BLT_TABLE table, const char *label); /* 161 */
    Blt_HashTable * (*blt_table_column_get_label_table) (BLT_TABLE table, const char *label); /* 162 */
    BLT_TABLE_ROW (*blt_table_get_row) (Tcl_Interp *interp, BLT_TABLE table, Tcl_Obj *objPtr); /* 163 */
    BLT_TABLE_COLUMN (*blt_table_get_column) (Tcl_Interp *interp, BLT_TABLE table, Tcl_Obj *objPtr); /* 164 */
    BLT_TABLE_ROW (*blt_table_get_row_by_label) (BLT_TABLE table, const char *label); /* 165 */
    BLT_TABLE_COLUMN (*blt_table_get_column_by_label) (BLT_TABLE table, const char *label); /* 166 */
    BLT_TABLE_ROW (*blt_table_get_row_by_index) (BLT_TABLE table, long index); /* 167 */
    BLT_TABLE_COLUMN (*blt_table_get_column_by_index) (BLT_TABLE table, long index); /* 168 */
    int (*blt_table_set_row_label) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_ROW row, const char *label); /* 169 */
    int (*blt_table_set_column_label) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_COLUMN column, const char *label); /* 170 */
    BLT_TABLE_COLUMN_TYPE (*blt_table_name_to_column_type) (const char *typeName); /* 171 */
    int (*blt_table_set_column_type) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_COLUMN column, BLT_TABLE_COLUMN_TYPE type); /* 172 */
    const char * (*blt_table_column_type_to_name) (BLT_TABLE_COLUMN_TYPE type); /* 173 */
    int (*blt_table_set_column_tag) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_COLUMN column, const char *tag); /* 174 */
    int (*blt_table_set_row_tag) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_ROW row, const char *tag); /* 175 */
    BLT_TABLE_ROW (*blt_table_create_row) (Tcl_Interp *interp, BLT_TABLE table, const char *label); /* 176 */
    BLT_TABLE_COLUMN (*blt_table_create_column) (Tcl_Interp *interp, BLT_TABLE table, const char *label); /* 177 */
    int (*blt_table_extend_rows) (Tcl_Interp *interp, BLT_TABLE table, size_t n, BLT_TABLE_ROW *rows); /* 178 */
    int (*blt_table_extend_columns) (Tcl_Interp *interp, BLT_TABLE table, size_t n, BLT_TABLE_COLUMN *columms); /* 179 */
    int (*blt_table_delete_row) (BLT_TABLE table, BLT_TABLE_ROW row); /* 180 */
    int (*blt_table_delete_column) (BLT_TABLE table, BLT_TABLE_COLUMN column); /* 181 */
    int (*blt_table_move_rows) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_ROW destRow, BLT_TABLE_ROW firstRow, BLT_TABLE_ROW lastRow, int after); /* 182 */
    int (*blt_table_move_columns) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_COLUMN destColumn, BLT_TABLE_COLUMN firstColumn, BLT_TABLE_COLUMN lastColumn, int after); /* 183 */
    Tcl_Obj * (*blt_table_get_obj) (BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column); /* 184 */
    int (*blt_table_set_obj) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column, Tcl_Obj *objPtr); /* 185 */
    const char * (*blt_table_get_string) (BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column); /* 186 */
    int (*blt_table_set_string_rep) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column, const char *string, int length); /* 187 */
    int (*blt_table_set_string) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column, const char *string, int length); /* 188 */
    int (*blt_table_append_string) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column, const char *string, int length); /* 189 */
    int (*blt_table_set_bytes) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column, const unsigned char *string, int length); /* 190 */
    double (*blt_table_get_double) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column); /* 191 */
    int (*blt_table_set_double) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column, double value); /* 192 */
    long (*blt_table_get_long) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column, long defValue); /* 193 */
    int (*blt_table_set_long) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column, long value); /* 194 */
    int (*blt_table_get_boolean) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column, int defValue); /* 195 */
    int (*blt_table_set_boolean) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column, int value); /* 196 */
    int (*blt_table_set_int64) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column, int64_t value); /* 197 */
    int64_t (*blt_table_get_int64) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column, int64_t defValue); /* 198 */
    BLT_TABLE_VALUE (*blt_table_get_value) (BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column); /* 199 */
    int (*blt_table_set_value) (BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column, BLT_TABLE_VALUE value); /* 200 */
    int (*blt_table_unset_value) (BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column); /* 201 */
    int (*blt_table_value_exists) (BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column); /* 202 */
    const char * (*blt_table_value_string) (BLT_TABLE_VALUE value); /* 203 */
    const unsigned char * (*blt_table_value_bytes) (BLT_TABLE_VALUE value); /* 204 */
    unsigned int (*blt_table_value_length) (BLT_TABLE_VALUE value); /* 205 */
    int (*blt_table_tags_are_shared) (BLT_TABLE table); /* 206 */
    void (*blt_table_clear_row_tags) (BLT_TABLE table, BLT_TABLE_ROW row); /* 207 */
    void (*blt_table_clear_column_tags) (BLT_TABLE table, BLT_TABLE_COLUMN col); /* 208 */
    Blt_Chain (*blt_table_get_row_tags) (BLT_TABLE table, BLT_TABLE_ROW row); /* 209 */
    Blt_Chain (*blt_table_get_column_tags) (BLT_TABLE table, BLT_TABLE_COLUMN column); /* 210 */
    Blt_Chain (*blt_table_get_tagged_rows) (BLT_TABLE table, const char *tag); /* 211 */
    Blt_Chain (*blt_table_get_tagged_columns) (BLT_TABLE table, const char *tag); /* 212 */
    int (*blt_table_row_has_tag) (BLT_TABLE table, BLT_TABLE_ROW row, const char *tag); /* 213 */
    int (*blt_table_column_has_tag) (BLT_TABLE table, BLT_TABLE_COLUMN column, const char *tag); /* 214 */
    int (*blt_table_forget_row_tag) (Tcl_Interp *interp, BLT_TABLE table, const char *tag); /* 215 */
    int (*blt_table_forget_column_tag) (Tcl_Interp *interp, BLT_TABLE table, const char *tag); /* 216 */
    int (*blt_table_unset_row_tag) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_ROW row, const char *tag); /* 217 */
    int (*blt_table_unset_column_tag) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_COLUMN column, const char *tag); /* 218 */
    BLT_TABLE_COLUMN (*blt_table_first_column) (BLT_TABLE table); /* 219 */
    BLT_TABLE_COLUMN (*blt_table_last_column) (BLT_TABLE table); /* 220 */
    BLT_TABLE_COLUMN (*blt_table_next_column) (BLT_TABLE_COLUMN column); /* 221 */
    BLT_TABLE_COLUMN (*blt_table_previous_column) (BLT_TABLE_COLUMN column); /* 222 */
    BLT_TABLE_ROW (*blt_table_first_row) (BLT_TABLE table); /* 223 */
    BLT_TABLE_ROW (*blt_table_last_row) (BLT_TABLE table); /* 224 */
    BLT_TABLE_ROW (*blt_table_next_row) (BLT_TABLE_ROW row); /* 225 */
    BLT_TABLE_ROW (*blt_table_previous_row) (BLT_TABLE_ROW row); /* 226 */
    BLT_TABLE_ROWCOLUMN_SPEC (*blt_table_row_spec) (BLT_TABLE table, Tcl_Obj *objPtr, const char **sp); /* 227 */
    BLT_TABLE_ROWCOLUMN_SPEC (*blt_table_column_spec) (BLT_TABLE table, Tcl_Obj *objPtr, const char **sp); /* 228 */
    int (*blt_table_iterate_rows) (Tcl_Interp *interp, BLT_TABLE table, Tcl_Obj *objPtr, BLT_TABLE_ITERATOR *iter); /* 229 */
    int (*blt_table_iterate_columns) (Tcl_Interp *interp, BLT_TABLE table, Tcl_Obj *objPtr, BLT_TABLE_ITERATOR *iter); /* 230 */
    int (*blt_table_iterate_rows_objv) (Tcl_Interp *interp, BLT_TABLE table, int objc, Tcl_Obj *const *objv, BLT_TABLE_ITERATOR *iterPtr); /* 231 */
    int (*blt_table_iterate_columns_objv) (Tcl_Interp *interp, BLT_TABLE table, int objc, Tcl_Obj *const *objv, BLT_TABLE_ITERATOR *iterPtr); /* 232 */
    void (*blt_table_free_iterator_objv) (BLT_TABLE_ITERATOR *iterPtr); /* 233 */
    void (*blt_table_iterate_all_rows) (BLT_TABLE table, BLT_TABLE_ITERATOR *iterPtr); /* 234 */
    void (*blt_table_iterate_all_columns) (BLT_TABLE table, BLT_TABLE_ITERATOR *iterPtr); /* 235 */
    BLT_TABLE_ROW (*blt_table_first_tagged_row) (BLT_TABLE_ITERATOR *iter); /* 236 */
    BLT_TABLE_COLUMN (*blt_table_first_tagged_column) (BLT_TABLE_ITERATOR *iter); /* 237 */
    BLT_TABLE_ROW (*blt_table_next_tagged_row) (BLT_TABLE_ITERATOR *iter); /* 238 */
    BLT_TABLE_COLUMN (*blt_table_next_tagged_column) (BLT_TABLE_ITERATOR *iter); /* 239 */
    int (*blt_table_list_rows) (Tcl_Interp *interp, BLT_TABLE table, int objc, Tcl_Obj *const *objv, Blt_Chain chain); /* 240 */
    int (*blt_table_list_columns) (Tcl_Interp *interp, BLT_TABLE table, int objc, Tcl_Obj *const *objv, Blt_Chain chain); /* 241 */
    void (*blt_table_clear_row_traces) (BLT_TABLE table, BLT_TABLE_ROW row); /* 242 */
    void (*blt_table_clear_column_traces) (BLT_TABLE table, BLT_TABLE_COLUMN column); /* 243 */
    BLT_TABLE_TRACE (*blt_table_create_trace) (BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column, const char *rowTag, const char *columnTag, unsigned int mask, BLT_TABLE_TRACE_PROC *proc, BLT_TABLE_TRACE_DELETE_PROC *deleteProc, ClientData clientData); /* 244 */
    void (*blt_table_trace_column) (BLT_TABLE table, BLT_TABLE_COLUMN column, unsigned int mask, BLT_TABLE_TRACE_PROC *proc, BLT_TABLE_TRACE_DELETE_PROC *deleteProc, ClientData clientData); /* 245 */
    void (*blt_table_trace_row) (BLT_TABLE table, BLT_TABLE_ROW row, unsigned int mask, BLT_TABLE_TRACE_PROC *proc, BLT_TABLE_TRACE_DELETE_PROC *deleteProc, ClientData clientData); /* 246 */
    BLT_TABLE_TRACE (*blt_table_create_column_trace) (BLT_TABLE table, BLT_TABLE_COLUMN column, unsigned int mask, BLT_TABLE_TRACE_PROC *proc, BLT_TABLE_TRACE_DELETE_PROC *deleteProc, ClientData clientData); /* 247 */
    BLT_TABLE_TRACE (*blt_table_create_column_tag_trace) (BLT_TABLE table, const char *tag, unsigned int mask, BLT_TABLE_TRACE_PROC *proc, BLT_TABLE_TRACE_DELETE_PROC *deleteProc, ClientData clientData); /* 248 */
    BLT_TABLE_TRACE (*blt_table_create_row_trace) (BLT_TABLE table, BLT_TABLE_ROW row, unsigned int mask, BLT_TABLE_TRACE_PROC *proc, BLT_TABLE_TRACE_DELETE_PROC *deleteProc, ClientData clientData); /* 249 */
    BLT_TABLE_TRACE (*blt_table_create_row_tag_trace) (BLT_TABLE table, const char *tag, unsigned int mask, BLT_TABLE_TRACE_PROC *proc, BLT_TABLE_TRACE_DELETE_PROC *deleteProc, ClientData clientData); /* 250 */
    void (*blt_table_delete_trace) (BLT_TABLE table, BLT_TABLE_TRACE trace); /* 251 */
    BLT_TABLE_NOTIFIER (*blt_table_create_notifier) (Tcl_Interp *interp, BLT_TABLE table, unsigned int mask, BLT_TABLE_NOTIFY_EVENT_PROC *proc, BLT_TABLE_NOTIFIER_DELETE_PROC *deleteProc, ClientData clientData); /* 252 */
    BLT_TABLE_NOTIFIER (*blt_table_create_row_notifier) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_ROW row, unsigned int mask, BLT_TABLE_NOTIFY_EVENT_PROC *proc, BLT_TABLE_NOTIFIER_DELETE_PROC *deleteProc, ClientData clientData); /* 253 */
    BLT_TABLE_NOTIFIER (*blt_table_create_row_tag_notifier) (Tcl_Interp *interp, BLT_TABLE table, const char *tag, unsigned int mask, BLT_TABLE_NOTIFY_EVENT_PROC *proc, BLT_TABLE_NOTIFIER_DELETE_PROC *deleteProc, ClientData clientData); /* 254 */
    BLT_TABLE_NOTIFIER (*blt_table_create_column_notifier) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_COLUMN column, unsigned int mask, BLT_TABLE_NOTIFY_EVENT_PROC *proc, BLT_TABLE_NOTIFIER_DELETE_PROC *deleteProc, ClientData clientData); /* 255 */
    BLT_TABLE_NOTIFIER (*blt_table_create_column_tag_notifier) (Tcl_Interp *interp, BLT_TABLE table, const char *tag, unsigned int mask, BLT_TABLE_NOTIFY_EVENT_PROC *proc, BLT_TABLE_NOTIFIER_DELETE_PROC *deleteProc, ClientData clientData); /* 256 */
    void (*blt_table_delete_notifier) (BLT_TABLE table, BLT_TABLE_NOTIFIER notifier); /* 257 */
    void (*blt_table_sort_init) (BLT_TABLE table, BLT_TABLE_SORT_ORDER *order, long numCompares, unsigned int flags); /* 258 */
    BLT_TABLE_ROW * (*blt_table_sort_rows) (BLT_TABLE table); /* 259 */
    void (*blt_table_sort_row_map) (BLT_TABLE table, long numRows, BLT_TABLE_ROW *rows); /* 260 */
    void (*blt_table_sort_finish) (void); /* 261 */
    BLT_TABLE_COMPARE_PROC * (*blt_table_get_compare_proc) (BLT_TABLE table, BLT_TABLE_COLUMN column, unsigned int flags); /* 262 */
    BLT_TABLE_ROW * (*blt_table_get_row_map) (BLT_TABLE table); /* 263 */
    BLT_TABLE_COLUMN * (*blt_table_get_column_map) (BLT_TABLE table); /* 264 */
    void (*blt_table_set_row_map) (BLT_TABLE table, BLT_TABLE_ROW *map); /* 265 */
    void (*blt_table_set_column_map) (BLT_TABLE table, BLT_TABLE_COLUMN *map); /* 266 */
    int (*blt_table_restore) (Tcl_Interp *interp, BLT_TABLE table, char *string, unsigned int flags); /* 267 */
    int (*blt_table_file_restore) (Tcl_Interp *interp, BLT_TABLE table, const char *fileName, unsigned int flags); /* 268 */
    int (*blt_table_register_format) (Tcl_Interp *interp, const char *name, BLT_TABLE_IMPORT_PROC *importProc, BLT_TABLE_EXPORT_PROC *exportProc); /* 269 */
    void (*blt_table_unset_keys) (BLT_TABLE table); /* 270 */
    int (*blt_table_get_keys) (BLT_TABLE table, BLT_TABLE_COLUMN **keysPtr); /* 271 */
    int (*blt_table_set_keys) (BLT_TABLE table, int numKeys, BLT_TABLE_COLUMN *keys, int unique); /* 272 */
    int (*blt_table_key_lookup) (Tcl_Interp *interp, BLT_TABLE table, int objc, Tcl_Obj *const *objv, BLT_TABLE_ROW *rowPtr); /* 273 */
    int (*blt_table_get_column_limits) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_COLUMN col, Tcl_Obj **minObjPtrPtr, Tcl_Obj **maxObjPtrPtr); /* 274 */
    BLT_TABLE_ROW (*blt_table_row) (BLT_TABLE table, long index); /* 275 */
    BLT_TABLE_COLUMN (*blt_table_column) (BLT_TABLE table, long index); /* 276 */
    long (*blt_table_row_index) (BLT_TABLE table, BLT_TABLE_ROW row); /* 277 */
    long (*blt_table_column_index) (BLT_TABLE table, BLT_TABLE_COLUMN column); /* 278 */
    double (*blt_VecMin) (Blt_Vector *vPtr); /* 279 */
    double (*blt_VecMax) (Blt_Vector *vPtr); /* 280 */
    Blt_VectorId (*blt_AllocVectorId) (Tcl_Interp *interp, const char *vecName); /* 281 */
    void (*blt_SetVectorChangedProc) (Blt_VectorId clientId, Blt_VectorChangedProc *proc, ClientData clientData); /* 282 */
    void (*blt_FreeVectorId) (Blt_VectorId clientId); /* 283 */
    int (*blt_GetVectorById) (Tcl_Interp *interp, Blt_VectorId clientId, Blt_Vector **vecPtrPtr); /* 284 */
    const char * (*blt_NameOfVectorId) (Blt_VectorId clientId); /* 285 */
    const char * (*blt_NameOfVector) (Blt_Vector *vecPtr); /* 286 */
    int (*blt_VectorNotifyPending) (Blt_VectorId clientId); /* 287 */
    int (*blt_CreateVector) (Tcl_Interp *interp, const char *vecName, int size, Blt_Vector **vecPtrPtr); /* 288 */
    int (*blt_CreateVector2) (Tcl_Interp *interp, const char *vecName, const char *cmdName, const char *varName, int initialSize, Blt_Vector **vecPtrPtr); /* 289 */
    int (*blt_GetVector) (Tcl_Interp *interp, const char *vecName, Blt_Vector **vecPtrPtr); /* 290 */
    int (*blt_GetVectorFromObj) (Tcl_Interp *interp, Tcl_Obj *objPtr, Blt_Vector **vecPtrPtr); /* 291 */
    int (*blt_VectorExists) (Tcl_Interp *interp, const char *vecName); /* 292 */
    int (*blt_ResetVector) (Blt_Vector *vecPtr, double *dataArr, int n, int arraySize, Tcl_FreeProc *freeProc); /* 293 */
    int (*blt_ResizeVector) (Blt_Vector *vecPtr, int n); /* 294 */
    int (*blt_DeleteVectorByName) (Tcl_Interp *interp, const char *vecName); /* 295 */
    int (*blt_DeleteVector) (Blt_Vector *vecPtr); /* 296 */
    int (*blt_ExprVector) (Tcl_Interp *interp, char *expr, Blt_Vector *vecPtr); /* 297 */
    void (*blt_InstallIndexProc) (Tcl_Interp *interp, const char *indexName, Blt_VectorIndexProc *procPtr); /* 298 */
    int (*blt_VectorExists2) (Tcl_Interp *interp, const char *vecName); /* 299 */
} BltTclProcs;

#ifdef __cplusplus
extern "C" {
#endif
extern BltTclProcs *bltTclProcsPtr;
#ifdef __cplusplus
}
#endif

#if defined(USE_BLT_STUBS) && !defined(BUILD_BLT_TCL_PROCS)

/*
 * Inline function declarations:
 */

/* Slot 0 is reserved */
#ifndef Blt_AllocInit
#define Blt_AllocInit \
	(bltTclProcsPtr->blt_AllocInit) /* 1 */
#endif
#ifndef Blt_Malloc
#define Blt_Malloc \
	(bltTclProcsPtr->blt_Malloc) /* 2 */
#endif
#ifndef Blt_Free
#define Blt_Free \
	(bltTclProcsPtr->blt_Free) /* 3 */
#endif
#ifndef Blt_Realloc
#define Blt_Realloc \
	(bltTclProcsPtr->blt_Realloc) /* 4 */
#endif
#ifndef Blt_Calloc
#define Blt_Calloc \
	(bltTclProcsPtr->blt_Calloc) /* 5 */
#endif
#ifndef Blt_MallocAbortOnError
#define Blt_MallocAbortOnError \
	(bltTclProcsPtr->blt_MallocAbortOnError) /* 6 */
#endif
#ifndef Blt_CallocAbortOnError
#define Blt_CallocAbortOnError \
	(bltTclProcsPtr->blt_CallocAbortOnError) /* 7 */
#endif
#ifndef Blt_ReallocAbortOnError
#define Blt_ReallocAbortOnError \
	(bltTclProcsPtr->blt_ReallocAbortOnError) /* 8 */
#endif
#ifndef Blt_Strndup
#define Blt_Strndup \
	(bltTclProcsPtr->blt_Strndup) /* 9 */
#endif
#ifndef Blt_Strdup
#define Blt_Strdup \
	(bltTclProcsPtr->blt_Strdup) /* 10 */
#endif
#ifndef Blt_StrdupAbortOnError
#define Blt_StrdupAbortOnError \
	(bltTclProcsPtr->blt_StrdupAbortOnError) /* 11 */
#endif
#ifndef Blt_StrndupAbortOnError
#define Blt_StrndupAbortOnError \
	(bltTclProcsPtr->blt_StrndupAbortOnError) /* 12 */
#endif
#ifndef Blt_Chain_Init
#define Blt_Chain_Init \
	(bltTclProcsPtr->blt_Chain_Init) /* 13 */
#endif
#ifndef Blt_Chain_Create
#define Blt_Chain_Create \
	(bltTclProcsPtr->blt_Chain_Create) /* 14 */
#endif
#ifndef Blt_Chain_Destroy
#define Blt_Chain_Destroy \
	(bltTclProcsPtr->blt_Chain_Destroy) /* 15 */
#endif
#ifndef Blt_Chain_NewLink
#define Blt_Chain_NewLink \
	(bltTclProcsPtr->blt_Chain_NewLink) /* 16 */
#endif
#ifndef Blt_Chain_AllocLink
#define Blt_Chain_AllocLink \
	(bltTclProcsPtr->blt_Chain_AllocLink) /* 17 */
#endif
#ifndef Blt_Chain_Append
#define Blt_Chain_Append \
	(bltTclProcsPtr->blt_Chain_Append) /* 18 */
#endif
#ifndef Blt_Chain_Prepend
#define Blt_Chain_Prepend \
	(bltTclProcsPtr->blt_Chain_Prepend) /* 19 */
#endif
#ifndef Blt_Chain_Reset
#define Blt_Chain_Reset \
	(bltTclProcsPtr->blt_Chain_Reset) /* 20 */
#endif
#ifndef Blt_Chain_InitLink
#define Blt_Chain_InitLink \
	(bltTclProcsPtr->blt_Chain_InitLink) /* 21 */
#endif
#ifndef Blt_Chain_LinkAfter
#define Blt_Chain_LinkAfter \
	(bltTclProcsPtr->blt_Chain_LinkAfter) /* 22 */
#endif
#ifndef Blt_Chain_LinkBefore
#define Blt_Chain_LinkBefore \
	(bltTclProcsPtr->blt_Chain_LinkBefore) /* 23 */
#endif
#ifndef Blt_Chain_UnlinkLink
#define Blt_Chain_UnlinkLink \
	(bltTclProcsPtr->blt_Chain_UnlinkLink) /* 24 */
#endif
#ifndef Blt_Chain_DeleteLink
#define Blt_Chain_DeleteLink \
	(bltTclProcsPtr->blt_Chain_DeleteLink) /* 25 */
#endif
#ifndef Blt_Chain_GetNthLink
#define Blt_Chain_GetNthLink \
	(bltTclProcsPtr->blt_Chain_GetNthLink) /* 26 */
#endif
#ifndef Blt_Chain_Sort
#define Blt_Chain_Sort \
	(bltTclProcsPtr->blt_Chain_Sort) /* 27 */
#endif
#ifndef Blt_Chain_Reverse
#define Blt_Chain_Reverse \
	(bltTclProcsPtr->blt_Chain_Reverse) /* 28 */
#endif
#ifndef Blt_Chain_IsBefore
#define Blt_Chain_IsBefore \
	(bltTclProcsPtr->blt_Chain_IsBefore) /* 29 */
#endif
#ifndef Blt_InitHashTable
#define Blt_InitHashTable \
	(bltTclProcsPtr->blt_InitHashTable) /* 30 */
#endif
#ifndef Blt_InitHashTableWithPool
#define Blt_InitHashTableWithPool \
	(bltTclProcsPtr->blt_InitHashTableWithPool) /* 31 */
#endif
#ifndef Blt_DeleteHashTable
#define Blt_DeleteHashTable \
	(bltTclProcsPtr->blt_DeleteHashTable) /* 32 */
#endif
#ifndef Blt_DeleteHashEntry
#define Blt_DeleteHashEntry \
	(bltTclProcsPtr->blt_DeleteHashEntry) /* 33 */
#endif
#ifndef Blt_FirstHashEntry
#define Blt_FirstHashEntry \
	(bltTclProcsPtr->blt_FirstHashEntry) /* 34 */
#endif
#ifndef Blt_NextHashEntry
#define Blt_NextHashEntry \
	(bltTclProcsPtr->blt_NextHashEntry) /* 35 */
#endif
#ifndef Blt_HashStats
#define Blt_HashStats \
	(bltTclProcsPtr->blt_HashStats) /* 36 */
#endif
#ifndef Blt_Tags_Create
#define Blt_Tags_Create \
	(bltTclProcsPtr->blt_Tags_Create) /* 37 */
#endif
#ifndef Blt_Tags_Destroy
#define Blt_Tags_Destroy \
	(bltTclProcsPtr->blt_Tags_Destroy) /* 38 */
#endif
#ifndef Blt_Tags_Init
#define Blt_Tags_Init \
	(bltTclProcsPtr->blt_Tags_Init) /* 39 */
#endif
#ifndef Blt_Tags_Reset
#define Blt_Tags_Reset \
	(bltTclProcsPtr->blt_Tags_Reset) /* 40 */
#endif
#ifndef Blt_Tags_ItemHasTag
#define Blt_Tags_ItemHasTag \
	(bltTclProcsPtr->blt_Tags_ItemHasTag) /* 41 */
#endif
#ifndef Blt_Tags_AddTag
#define Blt_Tags_AddTag \
	(bltTclProcsPtr->blt_Tags_AddTag) /* 42 */
#endif
#ifndef Blt_Tags_AddItemToTag
#define Blt_Tags_AddItemToTag \
	(bltTclProcsPtr->blt_Tags_AddItemToTag) /* 43 */
#endif
#ifndef Blt_Tags_ForgetTag
#define Blt_Tags_ForgetTag \
	(bltTclProcsPtr->blt_Tags_ForgetTag) /* 44 */
#endif
#ifndef Blt_Tags_RemoveItemFromTag
#define Blt_Tags_RemoveItemFromTag \
	(bltTclProcsPtr->blt_Tags_RemoveItemFromTag) /* 45 */
#endif
#ifndef Blt_Tags_ClearTagsFromItem
#define Blt_Tags_ClearTagsFromItem \
	(bltTclProcsPtr->blt_Tags_ClearTagsFromItem) /* 46 */
#endif
#ifndef Blt_Tags_AppendTagsToChain
#define Blt_Tags_AppendTagsToChain \
	(bltTclProcsPtr->blt_Tags_AppendTagsToChain) /* 47 */
#endif
#ifndef Blt_Tags_AppendTagsToObj
#define Blt_Tags_AppendTagsToObj \
	(bltTclProcsPtr->blt_Tags_AppendTagsToObj) /* 48 */
#endif
#ifndef Blt_Tags_AppendAllTagsToObj
#define Blt_Tags_AppendAllTagsToObj \
	(bltTclProcsPtr->blt_Tags_AppendAllTagsToObj) /* 49 */
#endif
#ifndef Blt_Tags_GetItemList
#define Blt_Tags_GetItemList \
	(bltTclProcsPtr->blt_Tags_GetItemList) /* 50 */
#endif
#ifndef Blt_Tags_GetTable
#define Blt_Tags_GetTable \
	(bltTclProcsPtr->blt_Tags_GetTable) /* 51 */
#endif
#ifndef Blt_List_Init
#define Blt_List_Init \
	(bltTclProcsPtr->blt_List_Init) /* 52 */
#endif
#ifndef Blt_List_Reset
#define Blt_List_Reset \
	(bltTclProcsPtr->blt_List_Reset) /* 53 */
#endif
#ifndef Blt_List_Create
#define Blt_List_Create \
	(bltTclProcsPtr->blt_List_Create) /* 54 */
#endif
#ifndef Blt_List_Destroy
#define Blt_List_Destroy \
	(bltTclProcsPtr->blt_List_Destroy) /* 55 */
#endif
#ifndef Blt_List_CreateNode
#define Blt_List_CreateNode \
	(bltTclProcsPtr->blt_List_CreateNode) /* 56 */
#endif
#ifndef Blt_List_DeleteNode
#define Blt_List_DeleteNode \
	(bltTclProcsPtr->blt_List_DeleteNode) /* 57 */
#endif
#ifndef Blt_List_Append
#define Blt_List_Append \
	(bltTclProcsPtr->blt_List_Append) /* 58 */
#endif
#ifndef Blt_List_Prepend
#define Blt_List_Prepend \
	(bltTclProcsPtr->blt_List_Prepend) /* 59 */
#endif
#ifndef Blt_List_LinkAfter
#define Blt_List_LinkAfter \
	(bltTclProcsPtr->blt_List_LinkAfter) /* 60 */
#endif
#ifndef Blt_List_LinkBefore
#define Blt_List_LinkBefore \
	(bltTclProcsPtr->blt_List_LinkBefore) /* 61 */
#endif
#ifndef Blt_List_UnlinkNode
#define Blt_List_UnlinkNode \
	(bltTclProcsPtr->blt_List_UnlinkNode) /* 62 */
#endif
#ifndef Blt_List_GetNode
#define Blt_List_GetNode \
	(bltTclProcsPtr->blt_List_GetNode) /* 63 */
#endif
#ifndef Blt_List_DeleteNodeByKey
#define Blt_List_DeleteNodeByKey \
	(bltTclProcsPtr->blt_List_DeleteNodeByKey) /* 64 */
#endif
#ifndef Blt_List_GetNthNode
#define Blt_List_GetNthNode \
	(bltTclProcsPtr->blt_List_GetNthNode) /* 65 */
#endif
#ifndef Blt_List_Sort
#define Blt_List_Sort \
	(bltTclProcsPtr->blt_List_Sort) /* 66 */
#endif
#ifndef Blt_Pool_Create
#define Blt_Pool_Create \
	(bltTclProcsPtr->blt_Pool_Create) /* 67 */
#endif
#ifndef Blt_Pool_Destroy
#define Blt_Pool_Destroy \
	(bltTclProcsPtr->blt_Pool_Destroy) /* 68 */
#endif
#ifndef Blt_Tree_GetUid
#define Blt_Tree_GetUid \
	(bltTclProcsPtr->blt_Tree_GetUid) /* 69 */
#endif
#ifndef Blt_Tree_GetUidFromNode
#define Blt_Tree_GetUidFromNode \
	(bltTclProcsPtr->blt_Tree_GetUidFromNode) /* 70 */
#endif
#ifndef Blt_Tree_GetNextId
#define Blt_Tree_GetNextId \
	(bltTclProcsPtr->blt_Tree_GetNextId) /* 71 */
#endif
#ifndef Blt_Tree_CreateNode
#define Blt_Tree_CreateNode \
	(bltTclProcsPtr->blt_Tree_CreateNode) /* 72 */
#endif
#ifndef Blt_Tree_CreateNodeWithId
#define Blt_Tree_CreateNodeWithId \
	(bltTclProcsPtr->blt_Tree_CreateNodeWithId) /* 73 */
#endif
#ifndef Blt_Tree_DeleteNode
#define Blt_Tree_DeleteNode \
	(bltTclProcsPtr->blt_Tree_DeleteNode) /* 74 */
#endif
#ifndef Blt_Tree_MoveNode
#define Blt_Tree_MoveNode \
	(bltTclProcsPtr->blt_Tree_MoveNode) /* 75 */
#endif
#ifndef Blt_Tree_GetNodeFromIndex
#define Blt_Tree_GetNodeFromIndex \
	(bltTclProcsPtr->blt_Tree_GetNodeFromIndex) /* 76 */
#endif
#ifndef Blt_Tree_FindChild
#define Blt_Tree_FindChild \
	(bltTclProcsPtr->blt_Tree_FindChild) /* 77 */
#endif
#ifndef Blt_Tree_NextNode
#define Blt_Tree_NextNode \
	(bltTclProcsPtr->blt_Tree_NextNode) /* 78 */
#endif
#ifndef Blt_Tree_PrevNode
#define Blt_Tree_PrevNode \
	(bltTclProcsPtr->blt_Tree_PrevNode) /* 79 */
#endif
#ifndef Blt_Tree_FirstChild
#define Blt_Tree_FirstChild \
	(bltTclProcsPtr->blt_Tree_FirstChild) /* 80 */
#endif
#ifndef Blt_Tree_LastChild
#define Blt_Tree_LastChild \
	(bltTclProcsPtr->blt_Tree_LastChild) /* 81 */
#endif
#ifndef Blt_Tree_IsBefore
#define Blt_Tree_IsBefore \
	(bltTclProcsPtr->blt_Tree_IsBefore) /* 82 */
#endif
#ifndef Blt_Tree_IsAncestor
#define Blt_Tree_IsAncestor \
	(bltTclProcsPtr->blt_Tree_IsAncestor) /* 83 */
#endif
#ifndef Blt_Tree_PrivateValue
#define Blt_Tree_PrivateValue \
	(bltTclProcsPtr->blt_Tree_PrivateValue) /* 84 */
#endif
#ifndef Blt_Tree_PublicValue
#define Blt_Tree_PublicValue \
	(bltTclProcsPtr->blt_Tree_PublicValue) /* 85 */
#endif
#ifndef Blt_Tree_GetValue
#define Blt_Tree_GetValue \
	(bltTclProcsPtr->blt_Tree_GetValue) /* 86 */
#endif
#ifndef Blt_Tree_ValueExists
#define Blt_Tree_ValueExists \
	(bltTclProcsPtr->blt_Tree_ValueExists) /* 87 */
#endif
#ifndef Blt_Tree_SetValue
#define Blt_Tree_SetValue \
	(bltTclProcsPtr->blt_Tree_SetValue) /* 88 */
#endif
#ifndef Blt_Tree_UnsetValue
#define Blt_Tree_UnsetValue \
	(bltTclProcsPtr->blt_Tree_UnsetValue) /* 89 */
#endif
#ifndef Blt_Tree_AppendObjValue
#define Blt_Tree_AppendObjValue \
	(bltTclProcsPtr->blt_Tree_AppendObjValue) /* 90 */
#endif
#ifndef Blt_Tree_ListAppendObjValue
#define Blt_Tree_ListAppendObjValue \
	(bltTclProcsPtr->blt_Tree_ListAppendObjValue) /* 91 */
#endif
#ifndef Blt_Tree_ListReplaceObjValues
#define Blt_Tree_ListReplaceObjValues \
	(bltTclProcsPtr->blt_Tree_ListReplaceObjValues) /* 92 */
#endif
#ifndef Blt_Tree_GetArrayObjValue
#define Blt_Tree_GetArrayObjValue \
	(bltTclProcsPtr->blt_Tree_GetArrayObjValue) /* 93 */
#endif
#ifndef Blt_Tree_SetArrayValue
#define Blt_Tree_SetArrayValue \
	(bltTclProcsPtr->blt_Tree_SetArrayValue) /* 94 */
#endif
#ifndef Blt_Tree_UnsetArrayValue
#define Blt_Tree_UnsetArrayValue \
	(bltTclProcsPtr->blt_Tree_UnsetArrayValue) /* 95 */
#endif
#ifndef Blt_Tree_AppendArrayObjValue
#define Blt_Tree_AppendArrayObjValue \
	(bltTclProcsPtr->blt_Tree_AppendArrayObjValue) /* 96 */
#endif
#ifndef Blt_Tree_ListAppendArrayObjValue
#define Blt_Tree_ListAppendArrayObjValue \
	(bltTclProcsPtr->blt_Tree_ListAppendArrayObjValue) /* 97 */
#endif
#ifndef Blt_Tree_ListReplaceArrayObjValues
#define Blt_Tree_ListReplaceArrayObjValues \
	(bltTclProcsPtr->blt_Tree_ListReplaceArrayObjValues) /* 98 */
#endif
#ifndef Blt_Tree_ArrayValueExists
#define Blt_Tree_ArrayValueExists \
	(bltTclProcsPtr->blt_Tree_ArrayValueExists) /* 99 */
#endif
#ifndef Blt_Tree_ArrayNames
#define Blt_Tree_ArrayNames \
	(bltTclProcsPtr->blt_Tree_ArrayNames) /* 100 */
#endif
#ifndef Blt_Tree_GetScalarValueByUid
#define Blt_Tree_GetScalarValueByUid \
	(bltTclProcsPtr->blt_Tree_GetScalarValueByUid) /* 101 */
#endif
#ifndef Blt_Tree_SetScalarValueByUid
#define Blt_Tree_SetScalarValueByUid \
	(bltTclProcsPtr->blt_Tree_SetScalarValueByUid) /* 102 */
#endif
#ifndef Blt_Tree_UnsetScalarValueByUid
#define Blt_Tree_UnsetScalarValueByUid \
	(bltTclProcsPtr->blt_Tree_UnsetScalarValueByUid) /* 103 */
#endif
#ifndef Blt_Tree_AppendScalarObjValueByUid
#define Blt_Tree_AppendScalarObjValueByUid \
	(bltTclProcsPtr->blt_Tree_AppendScalarObjValueByUid) /* 104 */
#endif
#ifndef Blt_Tree_ListAppendScalarObjValueByUid
#define Blt_Tree_ListAppendScalarObjValueByUid \
	(bltTclProcsPtr->blt_Tree_ListAppendScalarObjValueByUid) /* 105 */
#endif
#ifndef Blt_Tree_ListReplaceScalarObjValuesByUid
#define Blt_Tree_ListReplaceScalarObjValuesByUid \
	(bltTclProcsPtr->blt_Tree_ListReplaceScalarObjValuesByUid) /* 106 */
#endif
#ifndef Blt_Tree_ScalarValueExistsByUid
#define Blt_Tree_ScalarValueExistsByUid \
	(bltTclProcsPtr->blt_Tree_ScalarValueExistsByUid) /* 107 */
#endif
#ifndef Blt_Tree_FirstValue
#define Blt_Tree_FirstValue \
	(bltTclProcsPtr->blt_Tree_FirstValue) /* 108 */
#endif
#ifndef Blt_Tree_NextValue
#define Blt_Tree_NextValue \
	(bltTclProcsPtr->blt_Tree_NextValue) /* 109 */
#endif
#ifndef Blt_Tree_Apply
#define Blt_Tree_Apply \
	(bltTclProcsPtr->blt_Tree_Apply) /* 110 */
#endif
#ifndef Blt_Tree_ApplyDFS
#define Blt_Tree_ApplyDFS \
	(bltTclProcsPtr->blt_Tree_ApplyDFS) /* 111 */
#endif
#ifndef Blt_Tree_ApplyBFS
#define Blt_Tree_ApplyBFS \
	(bltTclProcsPtr->blt_Tree_ApplyBFS) /* 112 */
#endif
#ifndef Blt_Tree_SortNode
#define Blt_Tree_SortNode \
	(bltTclProcsPtr->blt_Tree_SortNode) /* 113 */
#endif
#ifndef Blt_Tree_Exists
#define Blt_Tree_Exists \
	(bltTclProcsPtr->blt_Tree_Exists) /* 114 */
#endif
#ifndef Blt_Tree_Open
#define Blt_Tree_Open \
	(bltTclProcsPtr->blt_Tree_Open) /* 115 */
#endif
#ifndef Blt_Tree_Close
#define Blt_Tree_Close \
	(bltTclProcsPtr->blt_Tree_Close) /* 116 */
#endif
#ifndef Blt_Tree_Attach
#define Blt_Tree_Attach \
	(bltTclProcsPtr->blt_Tree_Attach) /* 117 */
#endif
#ifndef Blt_Tree_GetFromObj
#define Blt_Tree_GetFromObj \
	(bltTclProcsPtr->blt_Tree_GetFromObj) /* 118 */
#endif
#ifndef Blt_Tree_Size
#define Blt_Tree_Size \
	(bltTclProcsPtr->blt_Tree_Size) /* 119 */
#endif
#ifndef Blt_Tree_CreateTrace
#define Blt_Tree_CreateTrace \
	(bltTclProcsPtr->blt_Tree_CreateTrace) /* 120 */
#endif
#ifndef Blt_Tree_DeleteTrace
#define Blt_Tree_DeleteTrace \
	(bltTclProcsPtr->blt_Tree_DeleteTrace) /* 121 */
#endif
#ifndef Blt_Tree_CreateEventHandler
#define Blt_Tree_CreateEventHandler \
	(bltTclProcsPtr->blt_Tree_CreateEventHandler) /* 122 */
#endif
#ifndef Blt_Tree_DeleteEventHandler
#define Blt_Tree_DeleteEventHandler \
	(bltTclProcsPtr->blt_Tree_DeleteEventHandler) /* 123 */
#endif
#ifndef Blt_Tree_RelabelNode
#define Blt_Tree_RelabelNode \
	(bltTclProcsPtr->blt_Tree_RelabelNode) /* 124 */
#endif
#ifndef Blt_Tree_RelabelNodeWithoutNotify
#define Blt_Tree_RelabelNodeWithoutNotify \
	(bltTclProcsPtr->blt_Tree_RelabelNodeWithoutNotify) /* 125 */
#endif
#ifndef Blt_Tree_NodeIdAscii
#define Blt_Tree_NodeIdAscii \
	(bltTclProcsPtr->blt_Tree_NodeIdAscii) /* 126 */
#endif
#ifndef Blt_Tree_NodePathObj
#define Blt_Tree_NodePathObj \
	(bltTclProcsPtr->blt_Tree_NodePathObj) /* 127 */
#endif
#ifndef Blt_Tree_NodePath
#define Blt_Tree_NodePath \
	(bltTclProcsPtr->blt_Tree_NodePath) /* 128 */
#endif
#ifndef Blt_Tree_NodeRelativePath
#define Blt_Tree_NodeRelativePath \
	(bltTclProcsPtr->blt_Tree_NodeRelativePath) /* 129 */
#endif
#ifndef Blt_Tree_NodePosition
#define Blt_Tree_NodePosition \
	(bltTclProcsPtr->blt_Tree_NodePosition) /* 130 */
#endif
#ifndef Blt_Tree_ClearTags
#define Blt_Tree_ClearTags \
	(bltTclProcsPtr->blt_Tree_ClearTags) /* 131 */
#endif
#ifndef Blt_Tree_HasTag
#define Blt_Tree_HasTag \
	(bltTclProcsPtr->blt_Tree_HasTag) /* 132 */
#endif
#ifndef Blt_Tree_AddTag
#define Blt_Tree_AddTag \
	(bltTclProcsPtr->blt_Tree_AddTag) /* 133 */
#endif
#ifndef Blt_Tree_RemoveTag
#define Blt_Tree_RemoveTag \
	(bltTclProcsPtr->blt_Tree_RemoveTag) /* 134 */
#endif
#ifndef Blt_Tree_ForgetTag
#define Blt_Tree_ForgetTag \
	(bltTclProcsPtr->blt_Tree_ForgetTag) /* 135 */
#endif
#ifndef Blt_Tree_TagHashTable
#define Blt_Tree_TagHashTable \
	(bltTclProcsPtr->blt_Tree_TagHashTable) /* 136 */
#endif
#ifndef Blt_Tree_TagTableIsShared
#define Blt_Tree_TagTableIsShared \
	(bltTclProcsPtr->blt_Tree_TagTableIsShared) /* 137 */
#endif
#ifndef Blt_Tree_NewTagTable
#define Blt_Tree_NewTagTable \
	(bltTclProcsPtr->blt_Tree_NewTagTable) /* 138 */
#endif
#ifndef Blt_Tree_FirstTag
#define Blt_Tree_FirstTag \
	(bltTclProcsPtr->blt_Tree_FirstTag) /* 139 */
#endif
#ifndef Blt_Tree_Depth
#define Blt_Tree_Depth \
	(bltTclProcsPtr->blt_Tree_Depth) /* 140 */
#endif
#ifndef Blt_Tree_RegisterFormat
#define Blt_Tree_RegisterFormat \
	(bltTclProcsPtr->blt_Tree_RegisterFormat) /* 141 */
#endif
#ifndef Blt_Tree_RememberTag
#define Blt_Tree_RememberTag \
	(bltTclProcsPtr->blt_Tree_RememberTag) /* 142 */
#endif
#ifndef Blt_Tree_GetNodeFromObj
#define Blt_Tree_GetNodeFromObj \
	(bltTclProcsPtr->blt_Tree_GetNodeFromObj) /* 143 */
#endif
#ifndef Blt_Tree_GetNodeIterator
#define Blt_Tree_GetNodeIterator \
	(bltTclProcsPtr->blt_Tree_GetNodeIterator) /* 144 */
#endif
#ifndef Blt_Tree_FirstTaggedNode
#define Blt_Tree_FirstTaggedNode \
	(bltTclProcsPtr->blt_Tree_FirstTaggedNode) /* 145 */
#endif
#ifndef Blt_Tree_NextTaggedNode
#define Blt_Tree_NextTaggedNode \
	(bltTclProcsPtr->blt_Tree_NextTaggedNode) /* 146 */
#endif
#ifndef Blt_Tree_GetPathSeparator
#define Blt_Tree_GetPathSeparator \
	(bltTclProcsPtr->blt_Tree_GetPathSeparator) /* 147 */
#endif
#ifndef Blt_Tree_SetPathSeparator
#define Blt_Tree_SetPathSeparator \
	(bltTclProcsPtr->blt_Tree_SetPathSeparator) /* 148 */
#endif
#ifndef blt_table_release_tags
#define blt_table_release_tags \
	(bltTclProcsPtr->blt_table_release_tags) /* 149 */
#endif
#ifndef blt_table_new_tags
#define blt_table_new_tags \
	(bltTclProcsPtr->blt_table_new_tags) /* 150 */
#endif
#ifndef blt_table_get_column_tag_table
#define blt_table_get_column_tag_table \
	(bltTclProcsPtr->blt_table_get_column_tag_table) /* 151 */
#endif
#ifndef blt_table_get_row_tag_table
#define blt_table_get_row_tag_table \
	(bltTclProcsPtr->blt_table_get_row_tag_table) /* 152 */
#endif
#ifndef blt_table_exists
#define blt_table_exists \
	(bltTclProcsPtr->blt_table_exists) /* 153 */
#endif
#ifndef blt_table_create
#define blt_table_create \
	(bltTclProcsPtr->blt_table_create) /* 154 */
#endif
#ifndef blt_table_open
#define blt_table_open \
	(bltTclProcsPtr->blt_table_open) /* 155 */
#endif
#ifndef blt_table_close
#define blt_table_close \
	(bltTclProcsPtr->blt_table_close) /* 156 */
#endif
#ifndef blt_table_clear
#define blt_table_clear \
	(bltTclProcsPtr->blt_table_clear) /* 157 */
#endif
#ifndef blt_table_pack
#define blt_table_pack \
	(bltTclProcsPtr->blt_table_pack) /* 158 */
#endif
#ifndef blt_table_reset
#define blt_table_reset \
	(bltTclProcsPtr->blt_table_reset) /* 159 */
#endif
#ifndef blt_table_same_object
#define blt_table_same_object \
	(bltTclProcsPtr->blt_table_same_object) /* 160 */
#endif
#ifndef blt_table_row_get_label_table
#define blt_table_row_get_label_table \
	(bltTclProcsPtr->blt_table_row_get_label_table) /* 161 */
#endif
#ifndef blt_table_column_get_label_table
#define blt_table_column_get_label_table \
	(bltTclProcsPtr->blt_table_column_get_label_table) /* 162 */
#endif
#ifndef blt_table_get_row
#define blt_table_get_row \
	(bltTclProcsPtr->blt_table_get_row) /* 163 */
#endif
#ifndef blt_table_get_column
#define blt_table_get_column \
	(bltTclProcsPtr->blt_table_get_column) /* 164 */
#endif
#ifndef blt_table_get_row_by_label
#define blt_table_get_row_by_label \
	(bltTclProcsPtr->blt_table_get_row_by_label) /* 165 */
#endif
#ifndef blt_table_get_column_by_label
#define blt_table_get_column_by_label \
	(bltTclProcsPtr->blt_table_get_column_by_label) /* 166 */
#endif
#ifndef blt_table_get_row_by_index
#define blt_table_get_row_by_index \
	(bltTclProcsPtr->blt_table_get_row_by_index) /* 167 */
#endif
#ifndef blt_table_get_column_by_index
#define blt_table_get_column_by_index \
	(bltTclProcsPtr->blt_table_get_column_by_index) /* 168 */
#endif
#ifndef blt_table_set_row_label
#define blt_table_set_row_label \
	(bltTclProcsPtr->blt_table_set_row_label) /* 169 */
#endif
#ifndef blt_table_set_column_label
#define blt_table_set_column_label \
	(bltTclProcsPtr->blt_table_set_column_label) /* 170 */
#endif
#ifndef blt_table_name_to_column_type
#define blt_table_name_to_column_type \
	(bltTclProcsPtr->blt_table_name_to_column_type) /* 171 */
#endif
#ifndef blt_table_set_column_type
#define blt_table_set_column_type \
	(bltTclProcsPtr->blt_table_set_column_type) /* 172 */
#endif
#ifndef blt_table_column_type_to_name
#define blt_table_column_type_to_name \
	(bltTclProcsPtr->blt_table_column_type_to_name) /* 173 */
#endif
#ifndef blt_table_set_column_tag
#define blt_table_set_column_tag \
	(bltTclProcsPtr->blt_table_set_column_tag) /* 174 */
#endif
#ifndef blt_table_set_row_tag
#define blt_table_set_row_tag \
	(bltTclProcsPtr->blt_table_set_row_tag) /* 175 */
#endif
#ifndef blt_table_create_row
#define blt_table_create_row \
	(bltTclProcsPtr->blt_table_create_row) /* 176 */
#endif
#ifndef blt_table_create_column
#define blt_table_create_column \
	(bltTclProcsPtr->blt_table_create_column) /* 177 */
#endif
#ifndef blt_table_extend_rows
#define blt_table_extend_rows \
	(bltTclProcsPtr->blt_table_extend_rows) /* 178 */
#endif
#ifndef blt_table_extend_columns
#define blt_table_extend_columns \
	(bltTclProcsPtr->blt_table_extend_columns) /* 179 */
#endif
#ifndef blt_table_delete_row
#define blt_table_delete_row \
	(bltTclProcsPtr->blt_table_delete_row) /* 180 */
#endif
#ifndef blt_table_delete_column
#define blt_table_delete_column \
	(bltTclProcsPtr->blt_table_delete_column) /* 181 */
#endif
#ifndef blt_table_move_rows
#define blt_table_move_rows \
	(bltTclProcsPtr->blt_table_move_rows) /* 182 */
#endif
#ifndef blt_table_move_columns
#define blt_table_move_columns \
	(bltTclProcsPtr->blt_table_move_columns) /* 183 */
#endif
#ifndef blt_table_get_obj
#define blt_table_get_obj \
	(bltTclProcsPtr->blt_table_get_obj) /* 184 */
#endif
#ifndef blt_table_set_obj
#define blt_table_set_obj \
	(bltTclProcsPtr->blt_table_set_obj) /* 185 */
#endif
#ifndef blt_table_get_string
#define blt_table_get_string \
	(bltTclProcsPtr->blt_table_get_string) /* 186 */
#endif
#ifndef blt_table_set_string_rep
#define blt_table_set_string_rep \
	(bltTclProcsPtr->blt_table_set_string_rep) /* 187 */
#endif
#ifndef blt_table_set_string
#define blt_table_set_string \
	(bltTclProcsPtr->blt_table_set_string) /* 188 */
#endif
#ifndef blt_table_append_string
#define blt_table_append_string \
	(bltTclProcsPtr->blt_table_append_string) /* 189 */
#endif
#ifndef blt_table_set_bytes
#define blt_table_set_bytes \
	(bltTclProcsPtr->blt_table_set_bytes) /* 190 */
#endif
#ifndef blt_table_get_double
#define blt_table_get_double \
	(bltTclProcsPtr->blt_table_get_double) /* 191 */
#endif
#ifndef blt_table_set_double
#define blt_table_set_double \
	(bltTclProcsPtr->blt_table_set_double) /* 192 */
#endif
#ifndef blt_table_get_long
#define blt_table_get_long \
	(bltTclProcsPtr->blt_table_get_long) /* 193 */
#endif
#ifndef blt_table_set_long
#define blt_table_set_long \
	(bltTclProcsPtr->blt_table_set_long) /* 194 */
#endif
#ifndef blt_table_get_boolean
#define blt_table_get_boolean \
	(bltTclProcsPtr->blt_table_get_boolean) /* 195 */
#endif
#ifndef blt_table_set_boolean
#define blt_table_set_boolean \
	(bltTclProcsPtr->blt_table_set_boolean) /* 196 */
#endif
#ifndef blt_table_set_int64
#define blt_table_set_int64 \
	(bltTclProcsPtr->blt_table_set_int64) /* 197 */
#endif
#ifndef blt_table_get_int64
#define blt_table_get_int64 \
	(bltTclProcsPtr->blt_table_get_int64) /* 198 */
#endif
#ifndef blt_table_get_value
#define blt_table_get_value \
	(bltTclProcsPtr->blt_table_get_value) /* 199 */
#endif
#ifndef blt_table_set_value
#define blt_table_set_value \
	(bltTclProcsPtr->blt_table_set_value) /* 200 */
#endif
#ifndef blt_table_unset_value
#define blt_table_unset_value \
	(bltTclProcsPtr->blt_table_unset_value) /* 201 */
#endif
#ifndef blt_table_value_exists
#define blt_table_value_exists \
	(bltTclProcsPtr->blt_table_value_exists) /* 202 */
#endif
#ifndef blt_table_value_string
#define blt_table_value_string \
	(bltTclProcsPtr->blt_table_value_string) /* 203 */
#endif
#ifndef blt_table_value_bytes
#define blt_table_value_bytes \
	(bltTclProcsPtr->blt_table_value_bytes) /* 204 */
#endif
#ifndef blt_table_value_length
#define blt_table_value_length \
	(bltTclProcsPtr->blt_table_value_length) /* 205 */
#endif
#ifndef blt_table_tags_are_shared
#define blt_table_tags_are_shared \
	(bltTclProcsPtr->blt_table_tags_are_shared) /* 206 */
#endif
#ifndef blt_table_clear_row_tags
#define blt_table_clear_row_tags \
	(bltTclProcsPtr->blt_table_clear_row_tags) /* 207 */
#endif
#ifndef blt_table_clear_column_tags
#define blt_table_clear_column_tags \
	(bltTclProcsPtr->blt_table_clear_column_tags) /* 208 */
#endif
#ifndef blt_table_get_row_tags
#define blt_table_get_row_tags \
	(bltTclProcsPtr->blt_table_get_row_tags) /* 209 */
#endif
#ifndef blt_table_get_column_tags
#define blt_table_get_column_tags \
	(bltTclProcsPtr->blt_table_get_column_tags) /* 210 */
#endif
#ifndef blt_table_get_tagged_rows
#define blt_table_get_tagged_rows \
	(bltTclProcsPtr->blt_table_get_tagged_rows) /* 211 */
#endif
#ifndef blt_table_get_tagged_columns
#define blt_table_get_tagged_columns \
	(bltTclProcsPtr->blt_table_get_tagged_columns) /* 212 */
#endif
#ifndef blt_table_row_has_tag
#define blt_table_row_has_tag \
	(bltTclProcsPtr->blt_table_row_has_tag) /* 213 */
#endif
#ifndef blt_table_column_has_tag
#define blt_table_column_has_tag \
	(bltTclProcsPtr->blt_table_column_has_tag) /* 214 */
#endif
#ifndef blt_table_forget_row_tag
#define blt_table_forget_row_tag \
	(bltTclProcsPtr->blt_table_forget_row_tag) /* 215 */
#endif
#ifndef blt_table_forget_column_tag
#define blt_table_forget_column_tag \
	(bltTclProcsPtr->blt_table_forget_column_tag) /* 216 */
#endif
#ifndef blt_table_unset_row_tag
#define blt_table_unset_row_tag \
	(bltTclProcsPtr->blt_table_unset_row_tag) /* 217 */
#endif
#ifndef blt_table_unset_column_tag
#define blt_table_unset_column_tag \
	(bltTclProcsPtr->blt_table_unset_column_tag) /* 218 */
#endif
#ifndef blt_table_first_column
#define blt_table_first_column \
	(bltTclProcsPtr->blt_table_first_column) /* 219 */
#endif
#ifndef blt_table_last_column
#define blt_table_last_column \
	(bltTclProcsPtr->blt_table_last_column) /* 220 */
#endif
#ifndef blt_table_next_column
#define blt_table_next_column \
	(bltTclProcsPtr->blt_table_next_column) /* 221 */
#endif
#ifndef blt_table_previous_column
#define blt_table_previous_column \
	(bltTclProcsPtr->blt_table_previous_column) /* 222 */
#endif
#ifndef blt_table_first_row
#define blt_table_first_row \
	(bltTclProcsPtr->blt_table_first_row) /* 223 */
#endif
#ifndef blt_table_last_row
#define blt_table_last_row \
	(bltTclProcsPtr->blt_table_last_row) /* 224 */
#endif
#ifndef blt_table_next_row
#define blt_table_next_row \
	(bltTclProcsPtr->blt_table_next_row) /* 225 */
#endif
#ifndef blt_table_previous_row
#define blt_table_previous_row \
	(bltTclProcsPtr->blt_table_previous_row) /* 226 */
#endif
#ifndef blt_table_row_spec
#define blt_table_row_spec \
	(bltTclProcsPtr->blt_table_row_spec) /* 227 */
#endif
#ifndef blt_table_column_spec
#define blt_table_column_spec \
	(bltTclProcsPtr->blt_table_column_spec) /* 228 */
#endif
#ifndef blt_table_iterate_rows
#define blt_table_iterate_rows \
	(bltTclProcsPtr->blt_table_iterate_rows) /* 229 */
#endif
#ifndef blt_table_iterate_columns
#define blt_table_iterate_columns \
	(bltTclProcsPtr->blt_table_iterate_columns) /* 230 */
#endif
#ifndef blt_table_iterate_rows_objv
#define blt_table_iterate_rows_objv \
	(bltTclProcsPtr->blt_table_iterate_rows_objv) /* 231 */
#endif
#ifndef blt_table_iterate_columns_objv
#define blt_table_iterate_columns_objv \
	(bltTclProcsPtr->blt_table_iterate_columns_objv) /* 232 */
#endif
#ifndef blt_table_free_iterator_objv
#define blt_table_free_iterator_objv \
	(bltTclProcsPtr->blt_table_free_iterator_objv) /* 233 */
#endif
#ifndef blt_table_iterate_all_rows
#define blt_table_iterate_all_rows \
	(bltTclProcsPtr->blt_table_iterate_all_rows) /* 234 */
#endif
#ifndef blt_table_iterate_all_columns
#define blt_table_iterate_all_columns \
	(bltTclProcsPtr->blt_table_iterate_all_columns) /* 235 */
#endif
#ifndef blt_table_first_tagged_row
#define blt_table_first_tagged_row \
	(bltTclProcsPtr->blt_table_first_tagged_row) /* 236 */
#endif
#ifndef blt_table_first_tagged_column
#define blt_table_first_tagged_column \
	(bltTclProcsPtr->blt_table_first_tagged_column) /* 237 */
#endif
#ifndef blt_table_next_tagged_row
#define blt_table_next_tagged_row \
	(bltTclProcsPtr->blt_table_next_tagged_row) /* 238 */
#endif
#ifndef blt_table_next_tagged_column
#define blt_table_next_tagged_column \
	(bltTclProcsPtr->blt_table_next_tagged_column) /* 239 */
#endif
#ifndef blt_table_list_rows
#define blt_table_list_rows \
	(bltTclProcsPtr->blt_table_list_rows) /* 240 */
#endif
#ifndef blt_table_list_columns
#define blt_table_list_columns \
	(bltTclProcsPtr->blt_table_list_columns) /* 241 */
#endif
#ifndef blt_table_clear_row_traces
#define blt_table_clear_row_traces \
	(bltTclProcsPtr->blt_table_clear_row_traces) /* 242 */
#endif
#ifndef blt_table_clear_column_traces
#define blt_table_clear_column_traces \
	(bltTclProcsPtr->blt_table_clear_column_traces) /* 243 */
#endif
#ifndef blt_table_create_trace
#define blt_table_create_trace \
	(bltTclProcsPtr->blt_table_create_trace) /* 244 */
#endif
#ifndef blt_table_trace_column
#define blt_table_trace_column \
	(bltTclProcsPtr->blt_table_trace_column) /* 245 */
#endif
#ifndef blt_table_trace_row
#define blt_table_trace_row \
	(bltTclProcsPtr->blt_table_trace_row) /* 246 */
#endif
#ifndef blt_table_create_column_trace
#define blt_table_create_column_trace \
	(bltTclProcsPtr->blt_table_create_column_trace) /* 247 */
#endif
#ifndef blt_table_create_column_tag_trace
#define blt_table_create_column_tag_trace \
	(bltTclProcsPtr->blt_table_create_column_tag_trace) /* 248 */
#endif
#ifndef blt_table_create_row_trace
#define blt_table_create_row_trace \
	(bltTclProcsPtr->blt_table_create_row_trace) /* 249 */
#endif
#ifndef blt_table_create_row_tag_trace
#define blt_table_create_row_tag_trace \
	(bltTclProcsPtr->blt_table_create_row_tag_trace) /* 250 */
#endif
#ifndef blt_table_delete_trace
#define blt_table_delete_trace \
	(bltTclProcsPtr->blt_table_delete_trace) /* 251 */
#endif
#ifndef blt_table_create_notifier
#define blt_table_create_notifier \
	(bltTclProcsPtr->blt_table_create_notifier) /* 252 */
#endif
#ifndef blt_table_create_row_notifier
#define blt_table_create_row_notifier \
	(bltTclProcsPtr->blt_table_create_row_notifier) /* 253 */
#endif
#ifndef blt_table_create_row_tag_notifier
#define blt_table_create_row_tag_notifier \
	(bltTclProcsPtr->blt_table_create_row_tag_notifier) /* 254 */
#endif
#ifndef blt_table_create_column_notifier
#define blt_table_create_column_notifier \
	(bltTclProcsPtr->blt_table_create_column_notifier) /* 255 */
#endif
#ifndef blt_table_create_column_tag_notifier
#define blt_table_create_column_tag_notifier \
	(bltTclProcsPtr->blt_table_create_column_tag_notifier) /* 256 */
#endif
#ifndef blt_table_delete_notifier
#define blt_table_delete_notifier \
	(bltTclProcsPtr->blt_table_delete_notifier) /* 257 */
#endif
#ifndef blt_table_sort_init
#define blt_table_sort_init \
	(bltTclProcsPtr->blt_table_sort_init) /* 258 */
#endif
#ifndef blt_table_sort_rows
#define blt_table_sort_rows \
	(bltTclProcsPtr->blt_table_sort_rows) /* 259 */
#endif
#ifndef blt_table_sort_row_map
#define blt_table_sort_row_map \
	(bltTclProcsPtr->blt_table_sort_row_map) /* 260 */
#endif
#ifndef blt_table_sort_finish
#define blt_table_sort_finish \
	(bltTclProcsPtr->blt_table_sort_finish) /* 261 */
#endif
#ifndef blt_table_get_compare_proc
#define blt_table_get_compare_proc \
	(bltTclProcsPtr->blt_table_get_compare_proc) /* 262 */
#endif
#ifndef blt_table_get_row_map
#define blt_table_get_row_map \
	(bltTclProcsPtr->blt_table_get_row_map) /* 263 */
#endif
#ifndef blt_table_get_column_map
#define blt_table_get_column_map \
	(bltTclProcsPtr->blt_table_get_column_map) /* 264 */
#endif
#ifndef blt_table_set_row_map
#define blt_table_set_row_map \
	(bltTclProcsPtr->blt_table_set_row_map) /* 265 */
#endif
#ifndef blt_table_set_column_map
#define blt_table_set_column_map \
	(bltTclProcsPtr->blt_table_set_column_map) /* 266 */
#endif
#ifndef blt_table_restore
#define blt_table_restore \
	(bltTclProcsPtr->blt_table_restore) /* 267 */
#endif
#ifndef blt_table_file_restore
#define blt_table_file_restore \
	(bltTclProcsPtr->blt_table_file_restore) /* 268 */
#endif
#ifndef blt_table_register_format
#define blt_table_register_format \
	(bltTclProcsPtr->blt_table_register_format) /* 269 */
#endif
#ifndef blt_table_unset_keys
#define blt_table_unset_keys \
	(bltTclProcsPtr->blt_table_unset_keys) /* 270 */
#endif
#ifndef blt_table_get_keys
#define blt_table_get_keys \
	(bltTclProcsPtr->blt_table_get_keys) /* 271 */
#endif
#ifndef blt_table_set_keys
#define blt_table_set_keys \
	(bltTclProcsPtr->blt_table_set_keys) /* 272 */
#endif
#ifndef blt_table_key_lookup
#define blt_table_key_lookup \
	(bltTclProcsPtr->blt_table_key_lookup) /* 273 */
#endif
#ifndef blt_table_get_column_limits
#define blt_table_get_column_limits \
	(bltTclProcsPtr->blt_table_get_column_limits) /* 274 */
#endif
#ifndef blt_table_row
#define blt_table_row \
	(bltTclProcsPtr->blt_table_row) /* 275 */
#endif
#ifndef blt_table_column
#define blt_table_column \
	(bltTclProcsPtr->blt_table_column) /* 276 */
#endif
#ifndef blt_table_row_index
#define blt_table_row_index \
	(bltTclProcsPtr->blt_table_row_index) /* 277 */
#endif
#ifndef blt_table_column_index
#define blt_table_column_index \
	(bltTclProcsPtr->blt_table_column_index) /* 278 */
#endif
#ifndef Blt_VecMin
#define Blt_VecMin \
	(bltTclProcsPtr->blt_VecMin) /* 279 */
#endif
#ifndef Blt_VecMax
#define Blt_VecMax \
	(bltTclProcsPtr->blt_VecMax) /* 280 */
#endif
#ifndef Blt_AllocVectorId
#define Blt_AllocVectorId \
	(bltTclProcsPtr->blt_AllocVectorId) /* 281 */
#endif
#ifndef Blt_SetVectorChangedProc
#define Blt_SetVectorChangedProc \
	(bltTclProcsPtr->blt_SetVectorChangedProc) /* 282 */
#endif
#ifndef Blt_FreeVectorId
#define Blt_FreeVectorId \
	(bltTclProcsPtr->blt_FreeVectorId) /* 283 */
#endif
#ifndef Blt_GetVectorById
#define Blt_GetVectorById \
	(bltTclProcsPtr->blt_GetVectorById) /* 284 */
#endif
#ifndef Blt_NameOfVectorId
#define Blt_NameOfVectorId \
	(bltTclProcsPtr->blt_NameOfVectorId) /* 285 */
#endif
#ifndef Blt_NameOfVector
#define Blt_NameOfVector \
	(bltTclProcsPtr->blt_NameOfVector) /* 286 */
#endif
#ifndef Blt_VectorNotifyPending
#define Blt_VectorNotifyPending \
	(bltTclProcsPtr->blt_VectorNotifyPending) /* 287 */
#endif
#ifndef Blt_CreateVector
#define Blt_CreateVector \
	(bltTclProcsPtr->blt_CreateVector) /* 288 */
#endif
#ifndef Blt_CreateVector2
#define Blt_CreateVector2 \
	(bltTclProcsPtr->blt_CreateVector2) /* 289 */
#endif
#ifndef Blt_GetVector
#define Blt_GetVector \
	(bltTclProcsPtr->blt_GetVector) /* 290 */
#endif
#ifndef Blt_GetVectorFromObj
#define Blt_GetVectorFromObj \
	(bltTclProcsPtr->blt_GetVectorFromObj) /* 291 */
#endif
#ifndef Blt_VectorExists
#define Blt_VectorExists \
	(bltTclProcsPtr->blt_VectorExists) /* 292 */
#endif
#ifndef Blt_ResetVector
#define Blt_ResetVector \
	(bltTclProcsPtr->blt_ResetVector) /* 293 */
#endif
#ifndef Blt_ResizeVector
#define Blt_ResizeVector \
	(bltTclProcsPtr->blt_ResizeVector) /* 294 */
#endif
#ifndef Blt_DeleteVectorByName
#define Blt_DeleteVectorByName \
	(bltTclProcsPtr->blt_DeleteVectorByName) /* 295 */
#endif
#ifndef Blt_DeleteVector
#define Blt_DeleteVector \
	(bltTclProcsPtr->blt_DeleteVector) /* 296 */
#endif
#ifndef Blt_ExprVector
#define Blt_ExprVector \
	(bltTclProcsPtr->blt_ExprVector) /* 297 */
#endif
#ifndef Blt_InstallIndexProc
#define Blt_InstallIndexProc \
	(bltTclProcsPtr->blt_InstallIndexProc) /* 298 */
#endif
#ifndef Blt_VectorExists2
#define Blt_VectorExists2 \
	(bltTclProcsPtr->blt_VectorExists2) /* 299 */
#endif

#endif /* defined(USE_BLT_STUBS) && !defined(BUILD_BLT_TCL_PROCS) */

/* !END!: Do not edit above this line. */
#endif /* _BLT_TCLPROCS_H */
