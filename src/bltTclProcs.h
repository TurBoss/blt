/* -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil -*- */
#ifndef _BLT_TCLPROCS_H
#define _BLT_TCLPROCS_H

#include "bltAlloc.h"
#include "bltChain.h"
#include "bltHash.h"
#include "bltTags.h"
#include "bltList.h"
#include "bltPool.h"
#include "bltSwitch.h"
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
#ifndef Blt_ExprDoubleFromObj_DECLARED
#define Blt_ExprDoubleFromObj_DECLARED
/* 69 */
BLT_EXTERN int		Blt_ExprDoubleFromObj(Tcl_Interp *interp,
				Tcl_Obj *objPtr, double *valuePtr);
#endif
#ifndef Blt_ExprIntFromObj_DECLARED
#define Blt_ExprIntFromObj_DECLARED
/* 70 */
BLT_EXTERN int		Blt_ExprIntFromObj(Tcl_Interp *interp,
				Tcl_Obj *objPtr, int *valuePtr);
#endif
#ifndef Blt_GetStateFromObj_DECLARED
#define Blt_GetStateFromObj_DECLARED
/* 71 */
BLT_EXTERN int		Blt_GetStateFromObj(Tcl_Interp *interp,
				Tcl_Obj *objPtr, int *statePtr);
#endif
#ifndef Blt_NameOfState_DECLARED
#define Blt_NameOfState_DECLARED
/* 72 */
BLT_EXTERN const char *	 Blt_NameOfState(int state);
#endif
#ifndef Blt_GetFillFromObj_DECLARED
#define Blt_GetFillFromObj_DECLARED
/* 73 */
BLT_EXTERN int		Blt_GetFillFromObj(Tcl_Interp *interp,
				Tcl_Obj *objPtr, int *fillPtr);
#endif
#ifndef Blt_NameOfFill_DECLARED
#define Blt_NameOfFill_DECLARED
/* 74 */
BLT_EXTERN const char *	 Blt_NameOfFill(int fill);
#endif
#ifndef Blt_GetResizeFromObj_DECLARED
#define Blt_GetResizeFromObj_DECLARED
/* 75 */
BLT_EXTERN int		Blt_GetResizeFromObj(Tcl_Interp *interp,
				Tcl_Obj *objPtr, int *fillPtr);
#endif
#ifndef Blt_NameOfResize_DECLARED
#define Blt_NameOfResize_DECLARED
/* 76 */
BLT_EXTERN const char *	 Blt_NameOfResize(int resize);
#endif
#ifndef Blt_GetSideFromObj_DECLARED
#define Blt_GetSideFromObj_DECLARED
/* 77 */
BLT_EXTERN int		Blt_GetSideFromObj(Tcl_Interp *interp,
				Tcl_Obj *objPtr, int *sidePtr);
#endif
#ifndef Blt_NameOfSide_DECLARED
#define Blt_NameOfSide_DECLARED
/* 78 */
BLT_EXTERN const char *	 Blt_NameOfSide(int side);
#endif
#ifndef Blt_GetCount_DECLARED
#define Blt_GetCount_DECLARED
/* 79 */
BLT_EXTERN int		Blt_GetCount(Tcl_Interp *interp, const char *string,
				int check, long *countPtr);
#endif
#ifndef Blt_GetCountFromObj_DECLARED
#define Blt_GetCountFromObj_DECLARED
/* 80 */
BLT_EXTERN int		Blt_GetCountFromObj(Tcl_Interp *interp,
				Tcl_Obj *objPtr, int check, long *countPtr);
#endif
#ifndef Blt_ParseSwitches_DECLARED
#define Blt_ParseSwitches_DECLARED
/* 81 */
BLT_EXTERN int		Blt_ParseSwitches(Tcl_Interp *interp,
				Blt_SwitchSpec *specPtr, int objc,
				Tcl_Obj *const *objv, void *rec, int flags);
#endif
#ifndef Blt_FreeSwitches_DECLARED
#define Blt_FreeSwitches_DECLARED
/* 82 */
BLT_EXTERN void		Blt_FreeSwitches(Blt_SwitchSpec *specs, void *rec,
				int flags);
#endif
#ifndef Blt_SwitchChanged_DECLARED
#define Blt_SwitchChanged_DECLARED
/* 83 */
BLT_EXTERN int		Blt_SwitchChanged(Blt_SwitchSpec *specs, ...);
#endif
#ifndef Blt_SwitchInfo_DECLARED
#define Blt_SwitchInfo_DECLARED
/* 84 */
BLT_EXTERN int		Blt_SwitchInfo(Tcl_Interp *interp,
				Blt_SwitchSpec *specs, void *record,
				Tcl_Obj *objPtr, int flags);
#endif
#ifndef Blt_SwitchValue_DECLARED
#define Blt_SwitchValue_DECLARED
/* 85 */
BLT_EXTERN int		Blt_SwitchValue(Tcl_Interp *interp,
				Blt_SwitchSpec *specs, void *record,
				Tcl_Obj *objPtr, int flags);
#endif
#ifndef Blt_Tree_GetUid_DECLARED
#define Blt_Tree_GetUid_DECLARED
/* 86 */
BLT_EXTERN Blt_TreeUid	Blt_Tree_GetUid(Blt_Tree tree, const char *string);
#endif
#ifndef Blt_Tree_GetUidFromNode_DECLARED
#define Blt_Tree_GetUidFromNode_DECLARED
/* 87 */
BLT_EXTERN Blt_TreeUid	Blt_Tree_GetUidFromNode(Blt_TreeNode node,
				const char *string);
#endif
#ifndef Blt_Tree_GetNextId_DECLARED
#define Blt_Tree_GetNextId_DECLARED
/* 88 */
BLT_EXTERN long		Blt_Tree_GetNextId(Blt_Tree tree);
#endif
#ifndef Blt_Tree_CreateNode_DECLARED
#define Blt_Tree_CreateNode_DECLARED
/* 89 */
BLT_EXTERN Blt_TreeNode	 Blt_Tree_CreateNode(Blt_Tree tree,
				Blt_TreeNode parent, const char *name,
				Blt_TreeNode before);
#endif
#ifndef Blt_Tree_CreateNodeWithId_DECLARED
#define Blt_Tree_CreateNodeWithId_DECLARED
/* 90 */
BLT_EXTERN Blt_TreeNode	 Blt_Tree_CreateNodeWithId(Blt_Tree tree,
				Blt_TreeNode parent, const char *name,
				long inode, Blt_TreeNode before);
#endif
#ifndef Blt_Tree_DeleteNode_DECLARED
#define Blt_Tree_DeleteNode_DECLARED
/* 91 */
BLT_EXTERN int		Blt_Tree_DeleteNode(Blt_Tree tree, Blt_TreeNode node);
#endif
#ifndef Blt_Tree_MoveNode_DECLARED
#define Blt_Tree_MoveNode_DECLARED
/* 92 */
BLT_EXTERN int		Blt_Tree_MoveNode(Blt_Tree tree, Blt_TreeNode node,
				Blt_TreeNode parent, Blt_TreeNode before);
#endif
#ifndef Blt_Tree_GetNodeFromIndex_DECLARED
#define Blt_Tree_GetNodeFromIndex_DECLARED
/* 93 */
BLT_EXTERN Blt_TreeNode	 Blt_Tree_GetNodeFromIndex(Blt_Tree tree, long inode);
#endif
#ifndef Blt_Tree_FindChild_DECLARED
#define Blt_Tree_FindChild_DECLARED
/* 94 */
BLT_EXTERN Blt_TreeNode	 Blt_Tree_FindChild(Blt_TreeNode parent,
				const char *name);
#endif
#ifndef Blt_Tree_NextNode_DECLARED
#define Blt_Tree_NextNode_DECLARED
/* 95 */
BLT_EXTERN Blt_TreeNode	 Blt_Tree_NextNode(Blt_TreeNode root,
				Blt_TreeNode node);
#endif
#ifndef Blt_Tree_PrevNode_DECLARED
#define Blt_Tree_PrevNode_DECLARED
/* 96 */
BLT_EXTERN Blt_TreeNode	 Blt_Tree_PrevNode(Blt_TreeNode root,
				Blt_TreeNode node);
#endif
#ifndef Blt_Tree_FirstChild_DECLARED
#define Blt_Tree_FirstChild_DECLARED
/* 97 */
BLT_EXTERN Blt_TreeNode	 Blt_Tree_FirstChild(Blt_TreeNode parent);
#endif
#ifndef Blt_Tree_LastChild_DECLARED
#define Blt_Tree_LastChild_DECLARED
/* 98 */
BLT_EXTERN Blt_TreeNode	 Blt_Tree_LastChild(Blt_TreeNode parent);
#endif
#ifndef Blt_Tree_IsBefore_DECLARED
#define Blt_Tree_IsBefore_DECLARED
/* 99 */
BLT_EXTERN int		Blt_Tree_IsBefore(Blt_TreeNode node1,
				Blt_TreeNode node2);
#endif
#ifndef Blt_Tree_IsAncestor_DECLARED
#define Blt_Tree_IsAncestor_DECLARED
/* 100 */
BLT_EXTERN int		Blt_Tree_IsAncestor(Blt_TreeNode node1,
				Blt_TreeNode node2);
#endif
#ifndef Blt_Tree_PrivateVariable_DECLARED
#define Blt_Tree_PrivateVariable_DECLARED
/* 101 */
BLT_EXTERN int		Blt_Tree_PrivateVariable(Tcl_Interp *interp,
				Blt_Tree tree, Blt_TreeNode node,
				Blt_TreeUid uid);
#endif
#ifndef Blt_Tree_PublicVariable_DECLARED
#define Blt_Tree_PublicVariable_DECLARED
/* 102 */
BLT_EXTERN int		Blt_Tree_PublicVariable(Tcl_Interp *interp,
				Blt_Tree tree, Blt_TreeNode node,
				Blt_TreeUid uid);
#endif
#ifndef Blt_Tree_GetVariable_DECLARED
#define Blt_Tree_GetVariable_DECLARED
/* 103 */
BLT_EXTERN int		Blt_Tree_GetVariable(Tcl_Interp *interp,
				Blt_Tree tree, Blt_TreeNode node,
				const char *varName, Tcl_Obj **valuePtr);
#endif
#ifndef Blt_Tree_VariableExists_DECLARED
#define Blt_Tree_VariableExists_DECLARED
/* 104 */
BLT_EXTERN int		Blt_Tree_VariableExists(Blt_Tree tree,
				Blt_TreeNode node, const char *varName);
#endif
#ifndef Blt_Tree_SetVariable_DECLARED
#define Blt_Tree_SetVariable_DECLARED
/* 105 */
BLT_EXTERN int		Blt_Tree_SetVariable(Tcl_Interp *interp,
				Blt_Tree tree, Blt_TreeNode node,
				const char *varName, Tcl_Obj *valuePtr);
#endif
#ifndef Blt_Tree_UnsetVariable_DECLARED
#define Blt_Tree_UnsetVariable_DECLARED
/* 106 */
BLT_EXTERN int		Blt_Tree_UnsetVariable(Tcl_Interp *interp,
				Blt_Tree tree, Blt_TreeNode node,
				const char *varName);
#endif
#ifndef Blt_Tree_AppendVariable_DECLARED
#define Blt_Tree_AppendVariable_DECLARED
/* 107 */
BLT_EXTERN int		Blt_Tree_AppendVariable(Tcl_Interp *interp,
				Blt_Tree tree, Blt_TreeNode node,
				const char *varName, Tcl_Obj *objPtr);
#endif
#ifndef Blt_Tree_ListAppendVariable_DECLARED
#define Blt_Tree_ListAppendVariable_DECLARED
/* 108 */
BLT_EXTERN int		Blt_Tree_ListAppendVariable(Tcl_Interp *interp,
				Blt_Tree tree, Blt_TreeNode node,
				const char *varName, Tcl_Obj *objPtr);
#endif
#ifndef Blt_Tree_ListReplaceVariable_DECLARED
#define Blt_Tree_ListReplaceVariable_DECLARED
/* 109 */
BLT_EXTERN int		Blt_Tree_ListReplaceVariable(Tcl_Interp *interp,
				Blt_Tree tree, Blt_TreeNode node,
				const char *varName, int firstIndex,
				int lastIndex, int objc,
				Tcl_Obj *const *objv);
#endif
#ifndef Blt_Tree_GetArrayVariable_DECLARED
#define Blt_Tree_GetArrayVariable_DECLARED
/* 110 */
BLT_EXTERN int		Blt_Tree_GetArrayVariable(Tcl_Interp *interp,
				Blt_Tree tree, Blt_TreeNode node,
				const char *varName, const char *elemName,
				Tcl_Obj **valueObjPtrPtr);
#endif
#ifndef Blt_Tree_SetArrayVariable_DECLARED
#define Blt_Tree_SetArrayVariable_DECLARED
/* 111 */
BLT_EXTERN int		Blt_Tree_SetArrayVariable(Tcl_Interp *interp,
				Blt_Tree tree, Blt_TreeNode node,
				const char *varName, const char *elemName,
				Tcl_Obj *valueObjPtr);
#endif
#ifndef Blt_Tree_UnsetArrayVariable_DECLARED
#define Blt_Tree_UnsetArrayVariable_DECLARED
/* 112 */
BLT_EXTERN int		Blt_Tree_UnsetArrayVariable(Tcl_Interp *interp,
				Blt_Tree tree, Blt_TreeNode node,
				const char *varName, const char *elemName);
#endif
#ifndef Blt_Tree_AppendArrayVariable_DECLARED
#define Blt_Tree_AppendArrayVariable_DECLARED
/* 113 */
BLT_EXTERN int		Blt_Tree_AppendArrayVariable(Tcl_Interp *interp,
				Blt_Tree tree, Blt_TreeNode node,
				const char *varName, const char *elemName,
				Tcl_Obj *objPtr);
#endif
#ifndef Blt_Tree_ListAppendArrayVariable_DECLARED
#define Blt_Tree_ListAppendArrayVariable_DECLARED
/* 114 */
BLT_EXTERN int		Blt_Tree_ListAppendArrayVariable(Tcl_Interp *interp,
				Blt_Tree tree, Blt_TreeNode node,
				const char *varName, const char *elemName,
				Tcl_Obj *valueObjPtr);
#endif
#ifndef Blt_Tree_ListReplaceArrayVariable_DECLARED
#define Blt_Tree_ListReplaceArrayVariable_DECLARED
/* 115 */
BLT_EXTERN int		Blt_Tree_ListReplaceArrayVariable(
				Tcl_Interp *interp, Blt_Tree tree,
				Blt_TreeNode node, const char *varName,
				const char *elemName, int firstIndex,
				int lastIndex, int objc,
				Tcl_Obj *const *objv);
#endif
#ifndef Blt_Tree_ArrayVariableExists_DECLARED
#define Blt_Tree_ArrayVariableExists_DECLARED
/* 116 */
BLT_EXTERN int		Blt_Tree_ArrayVariableExists(Blt_Tree tree,
				Blt_TreeNode node, const char *varName,
				const char *elemName);
#endif
#ifndef Blt_Tree_ArrayNames_DECLARED
#define Blt_Tree_ArrayNames_DECLARED
/* 117 */
BLT_EXTERN int		Blt_Tree_ArrayNames(Tcl_Interp *interp,
				Blt_Tree tree, Blt_TreeNode node,
				const char *varName, Tcl_Obj *listObjPtr);
#endif
#ifndef Blt_Tree_GetScalarVariableByUid_DECLARED
#define Blt_Tree_GetScalarVariableByUid_DECLARED
/* 118 */
BLT_EXTERN int		Blt_Tree_GetScalarVariableByUid(Tcl_Interp *interp,
				Blt_Tree tree, Blt_TreeNode node,
				Blt_TreeUid uid, Tcl_Obj **valuePtr);
#endif
#ifndef Blt_Tree_SetScalarVariableByUid_DECLARED
#define Blt_Tree_SetScalarVariableByUid_DECLARED
/* 119 */
BLT_EXTERN int		Blt_Tree_SetScalarVariableByUid(Tcl_Interp *interp,
				Blt_Tree tree, Blt_TreeNode node,
				Blt_TreeUid uid, Tcl_Obj *valuePtr);
#endif
#ifndef Blt_Tree_UnsetScalarVariableByUid_DECLARED
#define Blt_Tree_UnsetScalarVariableByUid_DECLARED
/* 120 */
BLT_EXTERN int		Blt_Tree_UnsetScalarVariableByUid(Tcl_Interp *interp,
				Blt_Tree tree, Blt_TreeNode node,
				Blt_TreeUid uid);
#endif
#ifndef Blt_Tree_AppendScalarVariableByUid_DECLARED
#define Blt_Tree_AppendScalarVariableByUid_DECLARED
/* 121 */
BLT_EXTERN int		Blt_Tree_AppendScalarVariableByUid(
				Tcl_Interp *interp, Blt_Tree tree,
				Blt_TreeNode node, Blt_TreeUid uid,
				Tcl_Obj *objPtr);
#endif
#ifndef Blt_Tree_ListAppendScalarVariableByUid_DECLARED
#define Blt_Tree_ListAppendScalarVariableByUid_DECLARED
/* 122 */
BLT_EXTERN int		Blt_Tree_ListAppendScalarVariableByUid(
				Tcl_Interp *interp, Blt_Tree tree,
				Blt_TreeNode node, Blt_TreeUid uid,
				Tcl_Obj *objPtr);
#endif
#ifndef Blt_Tree_ListReplaceScalarVariableByUid_DECLARED
#define Blt_Tree_ListReplaceScalarVariableByUid_DECLARED
/* 123 */
BLT_EXTERN int		Blt_Tree_ListReplaceScalarVariableByUid(
				Tcl_Interp *interp, Blt_Tree tree,
				Blt_TreeNode node, Blt_TreeUid uid,
				int firstIndex, int lastIndex, int objc,
				Tcl_Obj *const *objv);
#endif
#ifndef Blt_Tree_ScalarVariableExistsByUid_DECLARED
#define Blt_Tree_ScalarVariableExistsByUid_DECLARED
/* 124 */
BLT_EXTERN int		Blt_Tree_ScalarVariableExistsByUid(Blt_Tree tree,
				Blt_TreeNode node, Blt_TreeUid uid);
#endif
#ifndef Blt_Tree_FirstVariable_DECLARED
#define Blt_Tree_FirstVariable_DECLARED
/* 125 */
BLT_EXTERN Blt_TreeUid	Blt_Tree_FirstVariable(Blt_Tree tree,
				Blt_TreeNode node,
				Blt_TreeVariableIterator *iterPtr);
#endif
#ifndef Blt_Tree_NextVariable_DECLARED
#define Blt_Tree_NextVariable_DECLARED
/* 126 */
BLT_EXTERN Blt_TreeUid	Blt_Tree_NextVariable(Blt_Tree tree,
				Blt_TreeVariableIterator *iterPtr);
#endif
#ifndef Blt_Tree_Apply_DECLARED
#define Blt_Tree_Apply_DECLARED
/* 127 */
BLT_EXTERN int		Blt_Tree_Apply(Blt_TreeNode root,
				Blt_TreeApplyProc *proc,
				ClientData clientData);
#endif
#ifndef Blt_Tree_ApplyDFS_DECLARED
#define Blt_Tree_ApplyDFS_DECLARED
/* 128 */
BLT_EXTERN int		Blt_Tree_ApplyDFS(Blt_TreeNode root,
				Blt_TreeApplyProc *proc,
				ClientData clientData, int order);
#endif
#ifndef Blt_Tree_ApplyBFS_DECLARED
#define Blt_Tree_ApplyBFS_DECLARED
/* 129 */
BLT_EXTERN int		Blt_Tree_ApplyBFS(Blt_TreeNode root,
				Blt_TreeApplyProc *proc,
				ClientData clientData);
#endif
#ifndef Blt_Tree_SortNode_DECLARED
#define Blt_Tree_SortNode_DECLARED
/* 130 */
BLT_EXTERN int		Blt_Tree_SortNode(Blt_Tree tree, Blt_TreeNode node,
				Blt_TreeCompareNodesProc *proc);
#endif
#ifndef Blt_Tree_Exists_DECLARED
#define Blt_Tree_Exists_DECLARED
/* 131 */
BLT_EXTERN int		Blt_Tree_Exists(Tcl_Interp *interp, const char *name);
#endif
#ifndef Blt_Tree_Open_DECLARED
#define Blt_Tree_Open_DECLARED
/* 132 */
BLT_EXTERN Blt_Tree	Blt_Tree_Open(Tcl_Interp *interp, const char *name,
				int flags);
#endif
#ifndef Blt_Tree_Close_DECLARED
#define Blt_Tree_Close_DECLARED
/* 133 */
BLT_EXTERN void		Blt_Tree_Close(Blt_Tree tree);
#endif
#ifndef Blt_Tree_Attach_DECLARED
#define Blt_Tree_Attach_DECLARED
/* 134 */
BLT_EXTERN int		Blt_Tree_Attach(Tcl_Interp *interp, Blt_Tree tree,
				const char *name);
#endif
#ifndef Blt_Tree_GetFromObj_DECLARED
#define Blt_Tree_GetFromObj_DECLARED
/* 135 */
BLT_EXTERN Blt_Tree	Blt_Tree_GetFromObj(Tcl_Interp *interp,
				Tcl_Obj *objPtr);
#endif
#ifndef Blt_Tree_Size_DECLARED
#define Blt_Tree_Size_DECLARED
/* 136 */
BLT_EXTERN int		Blt_Tree_Size(Blt_TreeNode node);
#endif
#ifndef Blt_Tree_CreateTrace_DECLARED
#define Blt_Tree_CreateTrace_DECLARED
/* 137 */
BLT_EXTERN Blt_TreeTrace Blt_Tree_CreateTrace(Blt_Tree tree,
				Blt_TreeNode node, const char *keyPattern,
				const char *tagName, unsigned int mask,
				Blt_TreeTraceProc *proc,
				ClientData clientData);
#endif
#ifndef Blt_Tree_DeleteTrace_DECLARED
#define Blt_Tree_DeleteTrace_DECLARED
/* 138 */
BLT_EXTERN void		Blt_Tree_DeleteTrace(Blt_TreeTrace token);
#endif
#ifndef Blt_Tree_CreateEventHandler_DECLARED
#define Blt_Tree_CreateEventHandler_DECLARED
/* 139 */
BLT_EXTERN void		Blt_Tree_CreateEventHandler(Blt_Tree tree,
				unsigned int mask,
				Blt_TreeNotifyEventProc *proc,
				ClientData clientData);
#endif
#ifndef Blt_Tree_DeleteEventHandler_DECLARED
#define Blt_Tree_DeleteEventHandler_DECLARED
/* 140 */
BLT_EXTERN void		Blt_Tree_DeleteEventHandler(Blt_Tree tree,
				unsigned int mask,
				Blt_TreeNotifyEventProc *proc,
				ClientData clientData);
#endif
#ifndef Blt_Tree_RelabelNode_DECLARED
#define Blt_Tree_RelabelNode_DECLARED
/* 141 */
BLT_EXTERN void		Blt_Tree_RelabelNode(Blt_Tree tree,
				Blt_TreeNode node, const char *string);
#endif
#ifndef Blt_Tree_RelabelNodeWithoutNotify_DECLARED
#define Blt_Tree_RelabelNodeWithoutNotify_DECLARED
/* 142 */
BLT_EXTERN void		Blt_Tree_RelabelNodeWithoutNotify(Blt_TreeNode node,
				const char *string);
#endif
#ifndef Blt_Tree_NodeIdAscii_DECLARED
#define Blt_Tree_NodeIdAscii_DECLARED
/* 143 */
BLT_EXTERN const char *	 Blt_Tree_NodeIdAscii(Blt_TreeNode node);
#endif
#ifndef Blt_Tree_NodePathObj_DECLARED
#define Blt_Tree_NodePathObj_DECLARED
/* 144 */
BLT_EXTERN Tcl_Obj *	Blt_Tree_NodePathObj(Blt_TreeNode node);
#endif
#ifndef Blt_Tree_NodePath_DECLARED
#define Blt_Tree_NodePath_DECLARED
/* 145 */
BLT_EXTERN const char *	 Blt_Tree_NodePath(Blt_TreeNode node);
#endif
#ifndef Blt_Tree_NodeRelativePath_DECLARED
#define Blt_Tree_NodeRelativePath_DECLARED
/* 146 */
BLT_EXTERN const char *	 Blt_Tree_NodeRelativePath(Blt_TreeNode root,
				Blt_TreeNode node, const char *separator,
				unsigned int flags, Tcl_Obj *resultPtr);
#endif
#ifndef Blt_Tree_NodePosition_DECLARED
#define Blt_Tree_NodePosition_DECLARED
/* 147 */
BLT_EXTERN long		Blt_Tree_NodePosition(Blt_TreeNode node);
#endif
#ifndef Blt_Tree_ClearTags_DECLARED
#define Blt_Tree_ClearTags_DECLARED
/* 148 */
BLT_EXTERN void		Blt_Tree_ClearTags(Blt_Tree tree, Blt_TreeNode node);
#endif
#ifndef Blt_Tree_HasTag_DECLARED
#define Blt_Tree_HasTag_DECLARED
/* 149 */
BLT_EXTERN int		Blt_Tree_HasTag(Blt_Tree tree, Blt_TreeNode node,
				const char *tagName);
#endif
#ifndef Blt_Tree_AddTag_DECLARED
#define Blt_Tree_AddTag_DECLARED
/* 150 */
BLT_EXTERN void		Blt_Tree_AddTag(Blt_Tree tree, Blt_TreeNode node,
				const char *tagName);
#endif
#ifndef Blt_Tree_RemoveTag_DECLARED
#define Blt_Tree_RemoveTag_DECLARED
/* 151 */
BLT_EXTERN void		Blt_Tree_RemoveTag(Blt_Tree tree, Blt_TreeNode node,
				const char *tagName);
#endif
#ifndef Blt_Tree_ForgetTag_DECLARED
#define Blt_Tree_ForgetTag_DECLARED
/* 152 */
BLT_EXTERN void		Blt_Tree_ForgetTag(Blt_Tree tree,
				const char *tagName);
#endif
#ifndef Blt_Tree_TagHashTable_DECLARED
#define Blt_Tree_TagHashTable_DECLARED
/* 153 */
BLT_EXTERN Blt_HashTable * Blt_Tree_TagHashTable(Blt_Tree tree,
				const char *tagName);
#endif
#ifndef Blt_Tree_TagTableIsShared_DECLARED
#define Blt_Tree_TagTableIsShared_DECLARED
/* 154 */
BLT_EXTERN int		Blt_Tree_TagTableIsShared(Blt_Tree tree);
#endif
#ifndef Blt_Tree_NewTagTable_DECLARED
#define Blt_Tree_NewTagTable_DECLARED
/* 155 */
BLT_EXTERN void		Blt_Tree_NewTagTable(Blt_Tree tree);
#endif
#ifndef Blt_Tree_FirstTag_DECLARED
#define Blt_Tree_FirstTag_DECLARED
/* 156 */
BLT_EXTERN Blt_HashEntry * Blt_Tree_FirstTag(Blt_Tree tree,
				Blt_HashSearch *searchPtr);
#endif
#ifndef Blt_Tree_Depth_DECLARED
#define Blt_Tree_Depth_DECLARED
/* 157 */
BLT_EXTERN long		Blt_Tree_Depth(Blt_Tree tree);
#endif
#ifndef Blt_Tree_RegisterFormat_DECLARED
#define Blt_Tree_RegisterFormat_DECLARED
/* 158 */
BLT_EXTERN int		Blt_Tree_RegisterFormat(Tcl_Interp *interp,
				const char *fmtName,
				Blt_TreeImportProc *importProc,
				Blt_TreeExportProc *exportProc);
#endif
#ifndef Blt_Tree_RememberTag_DECLARED
#define Blt_Tree_RememberTag_DECLARED
/* 159 */
BLT_EXTERN Blt_TreeTagEntry * Blt_Tree_RememberTag(Blt_Tree tree,
				const char *name);
#endif
#ifndef Blt_Tree_GetNodeFromObj_DECLARED
#define Blt_Tree_GetNodeFromObj_DECLARED
/* 160 */
BLT_EXTERN int		Blt_Tree_GetNodeFromObj(Tcl_Interp *interp,
				Blt_Tree tree, Tcl_Obj *objPtr,
				Blt_TreeNode *nodePtr);
#endif
#ifndef Blt_Tree_GetNodeIterator_DECLARED
#define Blt_Tree_GetNodeIterator_DECLARED
/* 161 */
BLT_EXTERN int		Blt_Tree_GetNodeIterator(Tcl_Interp *interp,
				Blt_Tree tree, Tcl_Obj *objPtr,
				Blt_TreeNodeIterator *iterPtr);
#endif
#ifndef Blt_Tree_FirstTaggedNode_DECLARED
#define Blt_Tree_FirstTaggedNode_DECLARED
/* 162 */
BLT_EXTERN Blt_TreeNode	 Blt_Tree_FirstTaggedNode(
				Blt_TreeNodeIterator *iterPtr);
#endif
#ifndef Blt_Tree_NextTaggedNode_DECLARED
#define Blt_Tree_NextTaggedNode_DECLARED
/* 163 */
BLT_EXTERN Blt_TreeNode	 Blt_Tree_NextTaggedNode(
				Blt_TreeNodeIterator *iterPtr);
#endif
#ifndef Blt_Tree_GetPathSeparator_DECLARED
#define Blt_Tree_GetPathSeparator_DECLARED
/* 164 */
BLT_EXTERN const char *	 Blt_Tree_GetPathSeparator(Blt_Tree tree);
#endif
#ifndef Blt_Tree_SetPathSeparator_DECLARED
#define Blt_Tree_SetPathSeparator_DECLARED
/* 165 */
BLT_EXTERN void		Blt_Tree_SetPathSeparator(Blt_Tree tree,
				const char *sep);
#endif
#ifndef blt_table_release_tags_DECLARED
#define blt_table_release_tags_DECLARED
/* 166 */
BLT_EXTERN void		blt_table_release_tags(BLT_TABLE table);
#endif
#ifndef blt_table_new_tags_DECLARED
#define blt_table_new_tags_DECLARED
/* 167 */
BLT_EXTERN void		blt_table_new_tags(BLT_TABLE table);
#endif
#ifndef blt_table_get_column_tag_table_DECLARED
#define blt_table_get_column_tag_table_DECLARED
/* 168 */
BLT_EXTERN Blt_HashTable * blt_table_get_column_tag_table(BLT_TABLE table);
#endif
#ifndef blt_table_get_row_tag_table_DECLARED
#define blt_table_get_row_tag_table_DECLARED
/* 169 */
BLT_EXTERN Blt_HashTable * blt_table_get_row_tag_table(BLT_TABLE table);
#endif
#ifndef blt_table_exists_DECLARED
#define blt_table_exists_DECLARED
/* 170 */
BLT_EXTERN int		blt_table_exists(Tcl_Interp *interp,
				const char *name);
#endif
#ifndef blt_table_create_DECLARED
#define blt_table_create_DECLARED
/* 171 */
BLT_EXTERN int		blt_table_create(Tcl_Interp *interp,
				const char *name, BLT_TABLE *tablePtr);
#endif
#ifndef blt_table_open_DECLARED
#define blt_table_open_DECLARED
/* 172 */
BLT_EXTERN int		blt_table_open(Tcl_Interp *interp, const char *name,
				BLT_TABLE *tablePtr);
#endif
#ifndef blt_table_close_DECLARED
#define blt_table_close_DECLARED
/* 173 */
BLT_EXTERN void		blt_table_close(BLT_TABLE table);
#endif
#ifndef blt_table_clear_DECLARED
#define blt_table_clear_DECLARED
/* 174 */
BLT_EXTERN void		blt_table_clear(BLT_TABLE table);
#endif
#ifndef blt_table_pack_DECLARED
#define blt_table_pack_DECLARED
/* 175 */
BLT_EXTERN void		blt_table_pack(BLT_TABLE table);
#endif
#ifndef blt_table_reset_DECLARED
#define blt_table_reset_DECLARED
/* 176 */
BLT_EXTERN void		blt_table_reset(BLT_TABLE table);
#endif
#ifndef blt_table_same_object_DECLARED
#define blt_table_same_object_DECLARED
/* 177 */
BLT_EXTERN int		blt_table_same_object(BLT_TABLE table1,
				BLT_TABLE table2);
#endif
#ifndef blt_table_row_get_label_table_DECLARED
#define blt_table_row_get_label_table_DECLARED
/* 178 */
BLT_EXTERN Blt_HashTable * blt_table_row_get_label_table(BLT_TABLE table,
				const char *label);
#endif
#ifndef blt_table_column_get_label_table_DECLARED
#define blt_table_column_get_label_table_DECLARED
/* 179 */
BLT_EXTERN Blt_HashTable * blt_table_column_get_label_table(BLT_TABLE table,
				const char *label);
#endif
#ifndef blt_table_get_row_DECLARED
#define blt_table_get_row_DECLARED
/* 180 */
BLT_EXTERN BLT_TABLE_ROW blt_table_get_row(Tcl_Interp *interp,
				BLT_TABLE table, Tcl_Obj *objPtr);
#endif
#ifndef blt_table_get_column_DECLARED
#define blt_table_get_column_DECLARED
/* 181 */
BLT_EXTERN BLT_TABLE_COLUMN blt_table_get_column(Tcl_Interp *interp,
				BLT_TABLE table, Tcl_Obj *objPtr);
#endif
#ifndef blt_table_get_row_by_label_DECLARED
#define blt_table_get_row_by_label_DECLARED
/* 182 */
BLT_EXTERN BLT_TABLE_ROW blt_table_get_row_by_label(BLT_TABLE table,
				const char *label);
#endif
#ifndef blt_table_get_column_by_label_DECLARED
#define blt_table_get_column_by_label_DECLARED
/* 183 */
BLT_EXTERN BLT_TABLE_COLUMN blt_table_get_column_by_label(BLT_TABLE table,
				const char *label);
#endif
#ifndef blt_table_get_row_by_index_DECLARED
#define blt_table_get_row_by_index_DECLARED
/* 184 */
BLT_EXTERN BLT_TABLE_ROW blt_table_get_row_by_index(BLT_TABLE table,
				long index);
#endif
#ifndef blt_table_get_column_by_index_DECLARED
#define blt_table_get_column_by_index_DECLARED
/* 185 */
BLT_EXTERN BLT_TABLE_COLUMN blt_table_get_column_by_index(BLT_TABLE table,
				long index);
#endif
#ifndef blt_table_set_row_label_DECLARED
#define blt_table_set_row_label_DECLARED
/* 186 */
BLT_EXTERN int		blt_table_set_row_label(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_ROW row,
				const char *label);
#endif
#ifndef blt_table_set_column_label_DECLARED
#define blt_table_set_column_label_DECLARED
/* 187 */
BLT_EXTERN int		blt_table_set_column_label(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_COLUMN column,
				const char *label);
#endif
#ifndef blt_table_name_to_column_type_DECLARED
#define blt_table_name_to_column_type_DECLARED
/* 188 */
BLT_EXTERN BLT_TABLE_COLUMN_TYPE blt_table_name_to_column_type(
				const char *typeName);
#endif
#ifndef blt_table_set_column_type_DECLARED
#define blt_table_set_column_type_DECLARED
/* 189 */
BLT_EXTERN int		blt_table_set_column_type(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_COLUMN column,
				BLT_TABLE_COLUMN_TYPE type);
#endif
#ifndef blt_table_column_type_to_name_DECLARED
#define blt_table_column_type_to_name_DECLARED
/* 190 */
BLT_EXTERN const char *	 blt_table_column_type_to_name(
				BLT_TABLE_COLUMN_TYPE type);
#endif
#ifndef blt_table_set_column_tag_DECLARED
#define blt_table_set_column_tag_DECLARED
/* 191 */
BLT_EXTERN int		blt_table_set_column_tag(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_COLUMN column,
				const char *tag);
#endif
#ifndef blt_table_set_row_tag_DECLARED
#define blt_table_set_row_tag_DECLARED
/* 192 */
BLT_EXTERN int		blt_table_set_row_tag(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_ROW row,
				const char *tag);
#endif
#ifndef blt_table_create_row_DECLARED
#define blt_table_create_row_DECLARED
/* 193 */
BLT_EXTERN BLT_TABLE_ROW blt_table_create_row(Tcl_Interp *interp,
				BLT_TABLE table, const char *label);
#endif
#ifndef blt_table_create_column_DECLARED
#define blt_table_create_column_DECLARED
/* 194 */
BLT_EXTERN BLT_TABLE_COLUMN blt_table_create_column(Tcl_Interp *interp,
				BLT_TABLE table, const char *label);
#endif
#ifndef blt_table_extend_rows_DECLARED
#define blt_table_extend_rows_DECLARED
/* 195 */
BLT_EXTERN int		blt_table_extend_rows(Tcl_Interp *interp,
				BLT_TABLE table, size_t n,
				BLT_TABLE_ROW *rows);
#endif
#ifndef blt_table_extend_columns_DECLARED
#define blt_table_extend_columns_DECLARED
/* 196 */
BLT_EXTERN int		blt_table_extend_columns(Tcl_Interp *interp,
				BLT_TABLE table, size_t n,
				BLT_TABLE_COLUMN *columms);
#endif
#ifndef blt_table_delete_row_DECLARED
#define blt_table_delete_row_DECLARED
/* 197 */
BLT_EXTERN int		blt_table_delete_row(BLT_TABLE table,
				BLT_TABLE_ROW row);
#endif
#ifndef blt_table_delete_column_DECLARED
#define blt_table_delete_column_DECLARED
/* 198 */
BLT_EXTERN int		blt_table_delete_column(BLT_TABLE table,
				BLT_TABLE_COLUMN column);
#endif
#ifndef blt_table_move_rows_DECLARED
#define blt_table_move_rows_DECLARED
/* 199 */
BLT_EXTERN int		blt_table_move_rows(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_ROW destRow,
				BLT_TABLE_ROW firstRow,
				BLT_TABLE_ROW lastRow, int after);
#endif
#ifndef blt_table_move_columns_DECLARED
#define blt_table_move_columns_DECLARED
/* 200 */
BLT_EXTERN int		blt_table_move_columns(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_COLUMN destColumn,
				BLT_TABLE_COLUMN firstColumn,
				BLT_TABLE_COLUMN lastColumn, int after);
#endif
#ifndef blt_table_get_obj_DECLARED
#define blt_table_get_obj_DECLARED
/* 201 */
BLT_EXTERN Tcl_Obj *	blt_table_get_obj(BLT_TABLE table, BLT_TABLE_ROW row,
				BLT_TABLE_COLUMN column);
#endif
#ifndef blt_table_set_obj_DECLARED
#define blt_table_set_obj_DECLARED
/* 202 */
BLT_EXTERN int		blt_table_set_obj(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_ROW row,
				BLT_TABLE_COLUMN column, Tcl_Obj *objPtr);
#endif
#ifndef blt_table_get_string_DECLARED
#define blt_table_get_string_DECLARED
/* 203 */
BLT_EXTERN const char *	 blt_table_get_string(BLT_TABLE table,
				BLT_TABLE_ROW row, BLT_TABLE_COLUMN column);
#endif
#ifndef blt_table_set_string_rep_DECLARED
#define blt_table_set_string_rep_DECLARED
/* 204 */
BLT_EXTERN int		blt_table_set_string_rep(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_ROW row,
				BLT_TABLE_COLUMN column, const char *string,
				int length);
#endif
#ifndef blt_table_set_string_DECLARED
#define blt_table_set_string_DECLARED
/* 205 */
BLT_EXTERN int		blt_table_set_string(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_ROW row,
				BLT_TABLE_COLUMN column, const char *string,
				int length);
#endif
#ifndef blt_table_append_string_DECLARED
#define blt_table_append_string_DECLARED
/* 206 */
BLT_EXTERN int		blt_table_append_string(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_ROW row,
				BLT_TABLE_COLUMN column, const char *string,
				int length);
#endif
#ifndef blt_table_set_bytes_DECLARED
#define blt_table_set_bytes_DECLARED
/* 207 */
BLT_EXTERN int		blt_table_set_bytes(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_ROW row,
				BLT_TABLE_COLUMN column,
				const unsigned char *string, int length);
#endif
#ifndef blt_table_get_double_DECLARED
#define blt_table_get_double_DECLARED
/* 208 */
BLT_EXTERN double	blt_table_get_double(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_ROW row,
				BLT_TABLE_COLUMN column);
#endif
#ifndef blt_table_set_double_DECLARED
#define blt_table_set_double_DECLARED
/* 209 */
BLT_EXTERN int		blt_table_set_double(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_ROW row,
				BLT_TABLE_COLUMN column, double value);
#endif
#ifndef blt_table_get_long_DECLARED
#define blt_table_get_long_DECLARED
/* 210 */
BLT_EXTERN long		blt_table_get_long(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_ROW row,
				BLT_TABLE_COLUMN column, long defValue);
#endif
#ifndef blt_table_set_long_DECLARED
#define blt_table_set_long_DECLARED
/* 211 */
BLT_EXTERN int		blt_table_set_long(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_ROW row,
				BLT_TABLE_COLUMN column, long value);
#endif
#ifndef blt_table_get_boolean_DECLARED
#define blt_table_get_boolean_DECLARED
/* 212 */
BLT_EXTERN int		blt_table_get_boolean(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_ROW row,
				BLT_TABLE_COLUMN column, int defValue);
#endif
#ifndef blt_table_set_boolean_DECLARED
#define blt_table_set_boolean_DECLARED
/* 213 */
BLT_EXTERN int		blt_table_set_boolean(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_ROW row,
				BLT_TABLE_COLUMN column, int value);
#endif
#ifndef blt_table_set_int64_DECLARED
#define blt_table_set_int64_DECLARED
/* 214 */
BLT_EXTERN int		blt_table_set_int64(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_ROW row,
				BLT_TABLE_COLUMN column, int64_t value);
#endif
#ifndef blt_table_get_int64_DECLARED
#define blt_table_get_int64_DECLARED
/* 215 */
BLT_EXTERN int64_t	blt_table_get_int64(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_ROW row,
				BLT_TABLE_COLUMN column, int64_t defValue);
#endif
#ifndef blt_table_get_value_DECLARED
#define blt_table_get_value_DECLARED
/* 216 */
BLT_EXTERN BLT_TABLE_VALUE blt_table_get_value(BLT_TABLE table,
				BLT_TABLE_ROW row, BLT_TABLE_COLUMN column);
#endif
#ifndef blt_table_set_value_DECLARED
#define blt_table_set_value_DECLARED
/* 217 */
BLT_EXTERN int		blt_table_set_value(BLT_TABLE table,
				BLT_TABLE_ROW row, BLT_TABLE_COLUMN column,
				BLT_TABLE_VALUE value);
#endif
#ifndef blt_table_unset_value_DECLARED
#define blt_table_unset_value_DECLARED
/* 218 */
BLT_EXTERN int		blt_table_unset_value(BLT_TABLE table,
				BLT_TABLE_ROW row, BLT_TABLE_COLUMN column);
#endif
#ifndef blt_table_value_exists_DECLARED
#define blt_table_value_exists_DECLARED
/* 219 */
BLT_EXTERN int		blt_table_value_exists(BLT_TABLE table,
				BLT_TABLE_ROW row, BLT_TABLE_COLUMN column);
#endif
#ifndef blt_table_value_string_DECLARED
#define blt_table_value_string_DECLARED
/* 220 */
BLT_EXTERN const char *	 blt_table_value_string(BLT_TABLE_VALUE value);
#endif
#ifndef blt_table_value_bytes_DECLARED
#define blt_table_value_bytes_DECLARED
/* 221 */
BLT_EXTERN const unsigned char * blt_table_value_bytes(BLT_TABLE_VALUE value);
#endif
#ifndef blt_table_value_length_DECLARED
#define blt_table_value_length_DECLARED
/* 222 */
BLT_EXTERN unsigned int	 blt_table_value_length(BLT_TABLE_VALUE value);
#endif
#ifndef blt_table_tags_are_shared_DECLARED
#define blt_table_tags_are_shared_DECLARED
/* 223 */
BLT_EXTERN int		blt_table_tags_are_shared(BLT_TABLE table);
#endif
#ifndef blt_table_clear_row_tags_DECLARED
#define blt_table_clear_row_tags_DECLARED
/* 224 */
BLT_EXTERN void		blt_table_clear_row_tags(BLT_TABLE table,
				BLT_TABLE_ROW row);
#endif
#ifndef blt_table_clear_column_tags_DECLARED
#define blt_table_clear_column_tags_DECLARED
/* 225 */
BLT_EXTERN void		blt_table_clear_column_tags(BLT_TABLE table,
				BLT_TABLE_COLUMN col);
#endif
#ifndef blt_table_get_row_tags_DECLARED
#define blt_table_get_row_tags_DECLARED
/* 226 */
BLT_EXTERN Blt_Chain	blt_table_get_row_tags(BLT_TABLE table,
				BLT_TABLE_ROW row);
#endif
#ifndef blt_table_get_column_tags_DECLARED
#define blt_table_get_column_tags_DECLARED
/* 227 */
BLT_EXTERN Blt_Chain	blt_table_get_column_tags(BLT_TABLE table,
				BLT_TABLE_COLUMN column);
#endif
#ifndef blt_table_get_tagged_rows_DECLARED
#define blt_table_get_tagged_rows_DECLARED
/* 228 */
BLT_EXTERN Blt_Chain	blt_table_get_tagged_rows(BLT_TABLE table,
				const char *tag);
#endif
#ifndef blt_table_get_tagged_columns_DECLARED
#define blt_table_get_tagged_columns_DECLARED
/* 229 */
BLT_EXTERN Blt_Chain	blt_table_get_tagged_columns(BLT_TABLE table,
				const char *tag);
#endif
#ifndef blt_table_row_has_tag_DECLARED
#define blt_table_row_has_tag_DECLARED
/* 230 */
BLT_EXTERN int		blt_table_row_has_tag(BLT_TABLE table,
				BLT_TABLE_ROW row, const char *tag);
#endif
#ifndef blt_table_column_has_tag_DECLARED
#define blt_table_column_has_tag_DECLARED
/* 231 */
BLT_EXTERN int		blt_table_column_has_tag(BLT_TABLE table,
				BLT_TABLE_COLUMN column, const char *tag);
#endif
#ifndef blt_table_forget_row_tag_DECLARED
#define blt_table_forget_row_tag_DECLARED
/* 232 */
BLT_EXTERN int		blt_table_forget_row_tag(Tcl_Interp *interp,
				BLT_TABLE table, const char *tag);
#endif
#ifndef blt_table_forget_column_tag_DECLARED
#define blt_table_forget_column_tag_DECLARED
/* 233 */
BLT_EXTERN int		blt_table_forget_column_tag(Tcl_Interp *interp,
				BLT_TABLE table, const char *tag);
#endif
#ifndef blt_table_unset_row_tag_DECLARED
#define blt_table_unset_row_tag_DECLARED
/* 234 */
BLT_EXTERN int		blt_table_unset_row_tag(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_ROW row,
				const char *tag);
#endif
#ifndef blt_table_unset_column_tag_DECLARED
#define blt_table_unset_column_tag_DECLARED
/* 235 */
BLT_EXTERN int		blt_table_unset_column_tag(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_COLUMN column,
				const char *tag);
#endif
#ifndef blt_table_first_column_DECLARED
#define blt_table_first_column_DECLARED
/* 236 */
BLT_EXTERN BLT_TABLE_COLUMN blt_table_first_column(BLT_TABLE table);
#endif
#ifndef blt_table_last_column_DECLARED
#define blt_table_last_column_DECLARED
/* 237 */
BLT_EXTERN BLT_TABLE_COLUMN blt_table_last_column(BLT_TABLE table);
#endif
#ifndef blt_table_next_column_DECLARED
#define blt_table_next_column_DECLARED
/* 238 */
BLT_EXTERN BLT_TABLE_COLUMN blt_table_next_column(BLT_TABLE_COLUMN column);
#endif
#ifndef blt_table_previous_column_DECLARED
#define blt_table_previous_column_DECLARED
/* 239 */
BLT_EXTERN BLT_TABLE_COLUMN blt_table_previous_column(
				BLT_TABLE_COLUMN column);
#endif
#ifndef blt_table_first_row_DECLARED
#define blt_table_first_row_DECLARED
/* 240 */
BLT_EXTERN BLT_TABLE_ROW blt_table_first_row(BLT_TABLE table);
#endif
#ifndef blt_table_last_row_DECLARED
#define blt_table_last_row_DECLARED
/* 241 */
BLT_EXTERN BLT_TABLE_ROW blt_table_last_row(BLT_TABLE table);
#endif
#ifndef blt_table_next_row_DECLARED
#define blt_table_next_row_DECLARED
/* 242 */
BLT_EXTERN BLT_TABLE_ROW blt_table_next_row(BLT_TABLE_ROW row);
#endif
#ifndef blt_table_previous_row_DECLARED
#define blt_table_previous_row_DECLARED
/* 243 */
BLT_EXTERN BLT_TABLE_ROW blt_table_previous_row(BLT_TABLE_ROW row);
#endif
#ifndef blt_table_row_spec_DECLARED
#define blt_table_row_spec_DECLARED
/* 244 */
BLT_EXTERN BLT_TABLE_ROWCOLUMN_SPEC blt_table_row_spec(BLT_TABLE table,
				Tcl_Obj *objPtr, const char **sp);
#endif
#ifndef blt_table_column_spec_DECLARED
#define blt_table_column_spec_DECLARED
/* 245 */
BLT_EXTERN BLT_TABLE_ROWCOLUMN_SPEC blt_table_column_spec(BLT_TABLE table,
				Tcl_Obj *objPtr, const char **sp);
#endif
#ifndef blt_table_iterate_rows_DECLARED
#define blt_table_iterate_rows_DECLARED
/* 246 */
BLT_EXTERN int		blt_table_iterate_rows(Tcl_Interp *interp,
				BLT_TABLE table, Tcl_Obj *objPtr,
				BLT_TABLE_ITERATOR *iter);
#endif
#ifndef blt_table_iterate_columns_DECLARED
#define blt_table_iterate_columns_DECLARED
/* 247 */
BLT_EXTERN int		blt_table_iterate_columns(Tcl_Interp *interp,
				BLT_TABLE table, Tcl_Obj *objPtr,
				BLT_TABLE_ITERATOR *iter);
#endif
#ifndef blt_table_iterate_rows_objv_DECLARED
#define blt_table_iterate_rows_objv_DECLARED
/* 248 */
BLT_EXTERN int		blt_table_iterate_rows_objv(Tcl_Interp *interp,
				BLT_TABLE table, int objc,
				Tcl_Obj *const *objv,
				BLT_TABLE_ITERATOR *iterPtr);
#endif
#ifndef blt_table_iterate_columns_objv_DECLARED
#define blt_table_iterate_columns_objv_DECLARED
/* 249 */
BLT_EXTERN int		blt_table_iterate_columns_objv(Tcl_Interp *interp,
				BLT_TABLE table, int objc,
				Tcl_Obj *const *objv,
				BLT_TABLE_ITERATOR *iterPtr);
#endif
#ifndef blt_table_free_iterator_objv_DECLARED
#define blt_table_free_iterator_objv_DECLARED
/* 250 */
BLT_EXTERN void		blt_table_free_iterator_objv(
				BLT_TABLE_ITERATOR *iterPtr);
#endif
#ifndef blt_table_iterate_all_rows_DECLARED
#define blt_table_iterate_all_rows_DECLARED
/* 251 */
BLT_EXTERN void		blt_table_iterate_all_rows(BLT_TABLE table,
				BLT_TABLE_ITERATOR *iterPtr);
#endif
#ifndef blt_table_iterate_all_columns_DECLARED
#define blt_table_iterate_all_columns_DECLARED
/* 252 */
BLT_EXTERN void		blt_table_iterate_all_columns(BLT_TABLE table,
				BLT_TABLE_ITERATOR *iterPtr);
#endif
#ifndef blt_table_first_tagged_row_DECLARED
#define blt_table_first_tagged_row_DECLARED
/* 253 */
BLT_EXTERN BLT_TABLE_ROW blt_table_first_tagged_row(BLT_TABLE_ITERATOR *iter);
#endif
#ifndef blt_table_first_tagged_column_DECLARED
#define blt_table_first_tagged_column_DECLARED
/* 254 */
BLT_EXTERN BLT_TABLE_COLUMN blt_table_first_tagged_column(
				BLT_TABLE_ITERATOR *iter);
#endif
#ifndef blt_table_next_tagged_row_DECLARED
#define blt_table_next_tagged_row_DECLARED
/* 255 */
BLT_EXTERN BLT_TABLE_ROW blt_table_next_tagged_row(BLT_TABLE_ITERATOR *iter);
#endif
#ifndef blt_table_next_tagged_column_DECLARED
#define blt_table_next_tagged_column_DECLARED
/* 256 */
BLT_EXTERN BLT_TABLE_COLUMN blt_table_next_tagged_column(
				BLT_TABLE_ITERATOR *iter);
#endif
#ifndef blt_table_list_rows_DECLARED
#define blt_table_list_rows_DECLARED
/* 257 */
BLT_EXTERN int		blt_table_list_rows(Tcl_Interp *interp,
				BLT_TABLE table, int objc,
				Tcl_Obj *const *objv, Blt_Chain chain);
#endif
#ifndef blt_table_list_columns_DECLARED
#define blt_table_list_columns_DECLARED
/* 258 */
BLT_EXTERN int		blt_table_list_columns(Tcl_Interp *interp,
				BLT_TABLE table, int objc,
				Tcl_Obj *const *objv, Blt_Chain chain);
#endif
#ifndef blt_table_clear_row_traces_DECLARED
#define blt_table_clear_row_traces_DECLARED
/* 259 */
BLT_EXTERN void		blt_table_clear_row_traces(BLT_TABLE table,
				BLT_TABLE_ROW row);
#endif
#ifndef blt_table_clear_column_traces_DECLARED
#define blt_table_clear_column_traces_DECLARED
/* 260 */
BLT_EXTERN void		blt_table_clear_column_traces(BLT_TABLE table,
				BLT_TABLE_COLUMN column);
#endif
#ifndef blt_table_create_trace_DECLARED
#define blt_table_create_trace_DECLARED
/* 261 */
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
/* 262 */
BLT_EXTERN void		blt_table_trace_column(BLT_TABLE table,
				BLT_TABLE_COLUMN column, unsigned int mask,
				BLT_TABLE_TRACE_PROC *proc,
				BLT_TABLE_TRACE_DELETE_PROC *deleteProc,
				ClientData clientData);
#endif
#ifndef blt_table_trace_row_DECLARED
#define blt_table_trace_row_DECLARED
/* 263 */
BLT_EXTERN void		blt_table_trace_row(BLT_TABLE table,
				BLT_TABLE_ROW row, unsigned int mask,
				BLT_TABLE_TRACE_PROC *proc,
				BLT_TABLE_TRACE_DELETE_PROC *deleteProc,
				ClientData clientData);
#endif
#ifndef blt_table_create_column_trace_DECLARED
#define blt_table_create_column_trace_DECLARED
/* 264 */
BLT_EXTERN BLT_TABLE_TRACE blt_table_create_column_trace(BLT_TABLE table,
				BLT_TABLE_COLUMN column, unsigned int mask,
				BLT_TABLE_TRACE_PROC *proc,
				BLT_TABLE_TRACE_DELETE_PROC *deleteProc,
				ClientData clientData);
#endif
#ifndef blt_table_create_column_tag_trace_DECLARED
#define blt_table_create_column_tag_trace_DECLARED
/* 265 */
BLT_EXTERN BLT_TABLE_TRACE blt_table_create_column_tag_trace(BLT_TABLE table,
				const char *tag, unsigned int mask,
				BLT_TABLE_TRACE_PROC *proc,
				BLT_TABLE_TRACE_DELETE_PROC *deleteProc,
				ClientData clientData);
#endif
#ifndef blt_table_create_row_trace_DECLARED
#define blt_table_create_row_trace_DECLARED
/* 266 */
BLT_EXTERN BLT_TABLE_TRACE blt_table_create_row_trace(BLT_TABLE table,
				BLT_TABLE_ROW row, unsigned int mask,
				BLT_TABLE_TRACE_PROC *proc,
				BLT_TABLE_TRACE_DELETE_PROC *deleteProc,
				ClientData clientData);
#endif
#ifndef blt_table_create_row_tag_trace_DECLARED
#define blt_table_create_row_tag_trace_DECLARED
/* 267 */
BLT_EXTERN BLT_TABLE_TRACE blt_table_create_row_tag_trace(BLT_TABLE table,
				const char *tag, unsigned int mask,
				BLT_TABLE_TRACE_PROC *proc,
				BLT_TABLE_TRACE_DELETE_PROC *deleteProc,
				ClientData clientData);
#endif
#ifndef blt_table_delete_trace_DECLARED
#define blt_table_delete_trace_DECLARED
/* 268 */
BLT_EXTERN void		blt_table_delete_trace(BLT_TABLE table,
				BLT_TABLE_TRACE trace);
#endif
#ifndef blt_table_create_notifier_DECLARED
#define blt_table_create_notifier_DECLARED
/* 269 */
BLT_EXTERN BLT_TABLE_NOTIFIER blt_table_create_notifier(Tcl_Interp *interp,
				BLT_TABLE table, unsigned int mask,
				BLT_TABLE_NOTIFY_EVENT_PROC *proc,
				BLT_TABLE_NOTIFIER_DELETE_PROC *deleteProc,
				ClientData clientData);
#endif
#ifndef blt_table_create_row_notifier_DECLARED
#define blt_table_create_row_notifier_DECLARED
/* 270 */
BLT_EXTERN BLT_TABLE_NOTIFIER blt_table_create_row_notifier(
				Tcl_Interp *interp, BLT_TABLE table,
				BLT_TABLE_ROW row, unsigned int mask,
				BLT_TABLE_NOTIFY_EVENT_PROC *proc,
				BLT_TABLE_NOTIFIER_DELETE_PROC *deleteProc,
				ClientData clientData);
#endif
#ifndef blt_table_create_row_tag_notifier_DECLARED
#define blt_table_create_row_tag_notifier_DECLARED
/* 271 */
BLT_EXTERN BLT_TABLE_NOTIFIER blt_table_create_row_tag_notifier(
				Tcl_Interp *interp, BLT_TABLE table,
				const char *tag, unsigned int mask,
				BLT_TABLE_NOTIFY_EVENT_PROC *proc,
				BLT_TABLE_NOTIFIER_DELETE_PROC *deleteProc,
				ClientData clientData);
#endif
#ifndef blt_table_create_column_notifier_DECLARED
#define blt_table_create_column_notifier_DECLARED
/* 272 */
BLT_EXTERN BLT_TABLE_NOTIFIER blt_table_create_column_notifier(
				Tcl_Interp *interp, BLT_TABLE table,
				BLT_TABLE_COLUMN column, unsigned int mask,
				BLT_TABLE_NOTIFY_EVENT_PROC *proc,
				BLT_TABLE_NOTIFIER_DELETE_PROC *deleteProc,
				ClientData clientData);
#endif
#ifndef blt_table_create_column_tag_notifier_DECLARED
#define blt_table_create_column_tag_notifier_DECLARED
/* 273 */
BLT_EXTERN BLT_TABLE_NOTIFIER blt_table_create_column_tag_notifier(
				Tcl_Interp *interp, BLT_TABLE table,
				const char *tag, unsigned int mask,
				BLT_TABLE_NOTIFY_EVENT_PROC *proc,
				BLT_TABLE_NOTIFIER_DELETE_PROC *deleteProc,
				ClientData clientData);
#endif
#ifndef blt_table_delete_notifier_DECLARED
#define blt_table_delete_notifier_DECLARED
/* 274 */
BLT_EXTERN void		blt_table_delete_notifier(BLT_TABLE table,
				BLT_TABLE_NOTIFIER notifier);
#endif
#ifndef blt_table_sort_init_DECLARED
#define blt_table_sort_init_DECLARED
/* 275 */
BLT_EXTERN void		blt_table_sort_init(BLT_TABLE table,
				BLT_TABLE_SORT_ORDER *order,
				long numCompares, unsigned int flags);
#endif
#ifndef blt_table_sort_rows_DECLARED
#define blt_table_sort_rows_DECLARED
/* 276 */
BLT_EXTERN BLT_TABLE_ROW * blt_table_sort_rows(BLT_TABLE table);
#endif
#ifndef blt_table_sort_row_map_DECLARED
#define blt_table_sort_row_map_DECLARED
/* 277 */
BLT_EXTERN void		blt_table_sort_row_map(BLT_TABLE table, long numRows,
				BLT_TABLE_ROW *rows);
#endif
#ifndef blt_table_sort_finish_DECLARED
#define blt_table_sort_finish_DECLARED
/* 278 */
BLT_EXTERN void		blt_table_sort_finish(void );
#endif
#ifndef blt_table_get_compare_proc_DECLARED
#define blt_table_get_compare_proc_DECLARED
/* 279 */
BLT_EXTERN BLT_TABLE_COMPARE_PROC * blt_table_get_compare_proc(
				BLT_TABLE table, BLT_TABLE_COLUMN column,
				unsigned int flags);
#endif
#ifndef blt_table_get_row_map_DECLARED
#define blt_table_get_row_map_DECLARED
/* 280 */
BLT_EXTERN BLT_TABLE_ROW * blt_table_get_row_map(BLT_TABLE table);
#endif
#ifndef blt_table_get_column_map_DECLARED
#define blt_table_get_column_map_DECLARED
/* 281 */
BLT_EXTERN BLT_TABLE_COLUMN * blt_table_get_column_map(BLT_TABLE table);
#endif
#ifndef blt_table_set_row_map_DECLARED
#define blt_table_set_row_map_DECLARED
/* 282 */
BLT_EXTERN void		blt_table_set_row_map(BLT_TABLE table,
				BLT_TABLE_ROW *map);
#endif
#ifndef blt_table_set_column_map_DECLARED
#define blt_table_set_column_map_DECLARED
/* 283 */
BLT_EXTERN void		blt_table_set_column_map(BLT_TABLE table,
				BLT_TABLE_COLUMN *map);
#endif
#ifndef blt_table_restore_DECLARED
#define blt_table_restore_DECLARED
/* 284 */
BLT_EXTERN int		blt_table_restore(Tcl_Interp *interp,
				BLT_TABLE table, char *string,
				unsigned int flags);
#endif
#ifndef blt_table_file_restore_DECLARED
#define blt_table_file_restore_DECLARED
/* 285 */
BLT_EXTERN int		blt_table_file_restore(Tcl_Interp *interp,
				BLT_TABLE table, const char *fileName,
				unsigned int flags);
#endif
#ifndef blt_table_register_format_DECLARED
#define blt_table_register_format_DECLARED
/* 286 */
BLT_EXTERN int		blt_table_register_format(Tcl_Interp *interp,
				const char *name,
				BLT_TABLE_IMPORT_PROC *importProc,
				BLT_TABLE_EXPORT_PROC *exportProc);
#endif
#ifndef blt_table_unset_keys_DECLARED
#define blt_table_unset_keys_DECLARED
/* 287 */
BLT_EXTERN void		blt_table_unset_keys(BLT_TABLE table);
#endif
#ifndef blt_table_get_keys_DECLARED
#define blt_table_get_keys_DECLARED
/* 288 */
BLT_EXTERN int		blt_table_get_keys(BLT_TABLE table,
				BLT_TABLE_COLUMN **keysPtr);
#endif
#ifndef blt_table_set_keys_DECLARED
#define blt_table_set_keys_DECLARED
/* 289 */
BLT_EXTERN int		blt_table_set_keys(BLT_TABLE table, int numKeys,
				BLT_TABLE_COLUMN *keys, int unique);
#endif
#ifndef blt_table_key_lookup_DECLARED
#define blt_table_key_lookup_DECLARED
/* 290 */
BLT_EXTERN int		blt_table_key_lookup(Tcl_Interp *interp,
				BLT_TABLE table, int objc,
				Tcl_Obj *const *objv, BLT_TABLE_ROW *rowPtr);
#endif
#ifndef blt_table_get_column_limits_DECLARED
#define blt_table_get_column_limits_DECLARED
/* 291 */
BLT_EXTERN int		blt_table_get_column_limits(Tcl_Interp *interp,
				BLT_TABLE table, BLT_TABLE_COLUMN col,
				Tcl_Obj **minObjPtrPtr,
				Tcl_Obj **maxObjPtrPtr);
#endif
#ifndef blt_table_row_DECLARED
#define blt_table_row_DECLARED
/* 292 */
BLT_EXTERN BLT_TABLE_ROW blt_table_row(BLT_TABLE table, long index);
#endif
#ifndef blt_table_column_DECLARED
#define blt_table_column_DECLARED
/* 293 */
BLT_EXTERN BLT_TABLE_COLUMN blt_table_column(BLT_TABLE table, long index);
#endif
#ifndef blt_table_row_index_DECLARED
#define blt_table_row_index_DECLARED
/* 294 */
BLT_EXTERN long		blt_table_row_index(BLT_TABLE table,
				BLT_TABLE_ROW row);
#endif
#ifndef blt_table_column_index_DECLARED
#define blt_table_column_index_DECLARED
/* 295 */
BLT_EXTERN long		blt_table_column_index(BLT_TABLE table,
				BLT_TABLE_COLUMN column);
#endif
#ifndef Blt_VecMin_DECLARED
#define Blt_VecMin_DECLARED
/* 296 */
BLT_EXTERN double	Blt_VecMin(Blt_Vector *vPtr);
#endif
#ifndef Blt_VecMax_DECLARED
#define Blt_VecMax_DECLARED
/* 297 */
BLT_EXTERN double	Blt_VecMax(Blt_Vector *vPtr);
#endif
#ifndef Blt_GetVectorToken_DECLARED
#define Blt_GetVectorToken_DECLARED
/* 298 */
BLT_EXTERN Blt_VectorToken	 Blt_GetVectorToken(Tcl_Interp *interp,
				const char *vecName);
#endif
#ifndef Blt_SetVectorChangedProc_DECLARED
#define Blt_SetVectorChangedProc_DECLARED
/* 299 */
BLT_EXTERN void		Blt_SetVectorChangedProc(Blt_VectorToken clientId,
				Blt_VectorChangedProc *proc,
				ClientData clientData);
#endif
#ifndef Blt_FreeVectorToken_DECLARED
#define Blt_FreeVectorToken_DECLARED
/* 300 */
BLT_EXTERN void		Blt_FreeVectorToken(Blt_VectorToken clientId);
#endif
#ifndef Blt_GetVectorFromToken_DECLARED
#define Blt_GetVectorFromToken_DECLARED
/* 301 */
BLT_EXTERN int		Blt_GetVectorFromToken(Tcl_Interp *interp,
				Blt_VectorToken clientId,
				Blt_Vector **vecPtrPtr);
#endif
#ifndef Blt_NameOfVectorFromToken_DECLARED
#define Blt_NameOfVectorFromToken_DECLARED
/* 302 */
BLT_EXTERN const char *	 Blt_NameOfVectorFromToken(Blt_VectorToken clientId);
#endif
#ifndef Blt_NameOfVector_DECLARED
#define Blt_NameOfVector_DECLARED
/* 303 */
BLT_EXTERN const char *	 Blt_NameOfVector(Blt_Vector *vecPtr);
#endif
#ifndef Blt_VectorNotifyPending_DECLARED
#define Blt_VectorNotifyPending_DECLARED
/* 304 */
BLT_EXTERN int		Blt_VectorNotifyPending(Blt_VectorToken clientId);
#endif
#ifndef Blt_CreateVector_DECLARED
#define Blt_CreateVector_DECLARED
/* 305 */
BLT_EXTERN int		Blt_CreateVector(Tcl_Interp *interp,
				const char *vecName, int size,
				Blt_Vector **vecPtrPtr);
#endif
#ifndef Blt_CreateVector2_DECLARED
#define Blt_CreateVector2_DECLARED
/* 306 */
BLT_EXTERN int		Blt_CreateVector2(Tcl_Interp *interp,
				const char *vecName, const char *cmdName,
				const char *varName, int initialSize,
				Blt_Vector **vecPtrPtr);
#endif
#ifndef Blt_GetVector_DECLARED
#define Blt_GetVector_DECLARED
/* 307 */
BLT_EXTERN int		Blt_GetVector(Tcl_Interp *interp,
				const char *vecName, Blt_Vector **vecPtrPtr);
#endif
#ifndef Blt_GetVectorFromObj_DECLARED
#define Blt_GetVectorFromObj_DECLARED
/* 308 */
BLT_EXTERN int		Blt_GetVectorFromObj(Tcl_Interp *interp,
				Tcl_Obj *objPtr, Blt_Vector **vecPtrPtr);
#endif
#ifndef Blt_VectorExists_DECLARED
#define Blt_VectorExists_DECLARED
/* 309 */
BLT_EXTERN int		Blt_VectorExists(Tcl_Interp *interp,
				const char *vecName);
#endif
#ifndef Blt_ResetVector_DECLARED
#define Blt_ResetVector_DECLARED
/* 310 */
BLT_EXTERN int		Blt_ResetVector(Blt_Vector *vecPtr, double *dataArr,
				int n, int arraySize, Tcl_FreeProc *freeProc);
#endif
#ifndef Blt_ResizeVector_DECLARED
#define Blt_ResizeVector_DECLARED
/* 311 */
BLT_EXTERN int		Blt_ResizeVector(Blt_Vector *vecPtr, int n);
#endif
#ifndef Blt_DeleteVectorByName_DECLARED
#define Blt_DeleteVectorByName_DECLARED
/* 312 */
BLT_EXTERN int		Blt_DeleteVectorByName(Tcl_Interp *interp,
				const char *vecName);
#endif
#ifndef Blt_DeleteVector_DECLARED
#define Blt_DeleteVector_DECLARED
/* 313 */
BLT_EXTERN int		Blt_DeleteVector(Blt_Vector *vecPtr);
#endif
#ifndef Blt_ExprVector_DECLARED
#define Blt_ExprVector_DECLARED
/* 314 */
BLT_EXTERN int		Blt_ExprVector(Tcl_Interp *interp, char *expr,
				Blt_Vector *vecPtr);
#endif
#ifndef Blt_InstallIndexProc_DECLARED
#define Blt_InstallIndexProc_DECLARED
/* 315 */
BLT_EXTERN void		Blt_InstallIndexProc(Tcl_Interp *interp,
				const char *indexName,
				Blt_VectorIndexProc *procPtr);
#endif
#ifndef Blt_VectorExists2_DECLARED
#define Blt_VectorExists2_DECLARED
/* 316 */
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
    int (*blt_ExprDoubleFromObj) (Tcl_Interp *interp, Tcl_Obj *objPtr, double *valuePtr); /* 69 */
    int (*blt_ExprIntFromObj) (Tcl_Interp *interp, Tcl_Obj *objPtr, int *valuePtr); /* 70 */
    int (*blt_GetStateFromObj) (Tcl_Interp *interp, Tcl_Obj *objPtr, int *statePtr); /* 71 */
    const char * (*blt_NameOfState) (int state); /* 72 */
    int (*blt_GetFillFromObj) (Tcl_Interp *interp, Tcl_Obj *objPtr, int *fillPtr); /* 73 */
    const char * (*blt_NameOfFill) (int fill); /* 74 */
    int (*blt_GetResizeFromObj) (Tcl_Interp *interp, Tcl_Obj *objPtr, int *fillPtr); /* 75 */
    const char * (*blt_NameOfResize) (int resize); /* 76 */
    int (*blt_GetSideFromObj) (Tcl_Interp *interp, Tcl_Obj *objPtr, int *sidePtr); /* 77 */
    const char * (*blt_NameOfSide) (int side); /* 78 */
    int (*blt_GetCount) (Tcl_Interp *interp, const char *string, int check, long *countPtr); /* 79 */
    int (*blt_GetCountFromObj) (Tcl_Interp *interp, Tcl_Obj *objPtr, int check, long *countPtr); /* 80 */
    int (*blt_ParseSwitches) (Tcl_Interp *interp, Blt_SwitchSpec *specPtr, int objc, Tcl_Obj *const *objv, VOID *rec, int flags); /* 81 */
    void (*blt_FreeSwitches) (Blt_SwitchSpec *specs, VOID *rec, int flags); /* 82 */
    int (*blt_SwitchChanged) (Blt_SwitchSpec *specs, ...); /* 83 */
    int (*blt_SwitchInfo) (Tcl_Interp *interp, Blt_SwitchSpec *specs, VOID *record, Tcl_Obj *objPtr, int flags); /* 84 */
    int (*blt_SwitchValue) (Tcl_Interp *interp, Blt_SwitchSpec *specs, VOID *record, Tcl_Obj *objPtr, int flags); /* 85 */
    Blt_TreeUid (*blt_Tree_GetUid) (Blt_Tree tree, const char *string); /* 86 */
    Blt_TreeUid (*blt_Tree_GetUidFromNode) (Blt_TreeNode node, const char *string); /* 87 */
    long (*blt_Tree_GetNextId) (Blt_Tree tree); /* 88 */
    Blt_TreeNode (*blt_Tree_CreateNode) (Blt_Tree tree, Blt_TreeNode parent, const char *name, Blt_TreeNode before); /* 89 */
    Blt_TreeNode (*blt_Tree_CreateNodeWithId) (Blt_Tree tree, Blt_TreeNode parent, const char *name, long inode, Blt_TreeNode before); /* 90 */
    int (*blt_Tree_DeleteNode) (Blt_Tree tree, Blt_TreeNode node); /* 91 */
    int (*blt_Tree_MoveNode) (Blt_Tree tree, Blt_TreeNode node, Blt_TreeNode parent, Blt_TreeNode before); /* 92 */
    Blt_TreeNode (*blt_Tree_GetNodeFromIndex) (Blt_Tree tree, long inode); /* 93 */
    Blt_TreeNode (*blt_Tree_FindChild) (Blt_TreeNode parent, const char *name); /* 94 */
    Blt_TreeNode (*blt_Tree_NextNode) (Blt_TreeNode root, Blt_TreeNode node); /* 95 */
    Blt_TreeNode (*blt_Tree_PrevNode) (Blt_TreeNode root, Blt_TreeNode node); /* 96 */
    Blt_TreeNode (*blt_Tree_FirstChild) (Blt_TreeNode parent); /* 97 */
    Blt_TreeNode (*blt_Tree_LastChild) (Blt_TreeNode parent); /* 98 */
    int (*blt_Tree_IsBefore) (Blt_TreeNode node1, Blt_TreeNode node2); /* 99 */
    int (*blt_Tree_IsAncestor) (Blt_TreeNode node1, Blt_TreeNode node2); /* 100 */
    int (*blt_Tree_PrivateVariable) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, Blt_TreeUid uid); /* 101 */
    int (*blt_Tree_PublicVariable) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, Blt_TreeUid uid); /* 102 */
    int (*blt_Tree_GetVariable) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, const char *varName, Tcl_Obj **valuePtr); /* 103 */
    int (*blt_Tree_VariableExists) (Blt_Tree tree, Blt_TreeNode node, const char *varName); /* 104 */
    int (*blt_Tree_SetVariable) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, const char *varName, Tcl_Obj *valuePtr); /* 105 */
    int (*blt_Tree_UnsetVariable) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, const char *varName); /* 106 */
    int (*blt_Tree_AppendVariable) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, const char *varName, Tcl_Obj *objPtr); /* 107 */
    int (*blt_Tree_ListAppendVariable) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, const char *varName, Tcl_Obj *objPtr); /* 108 */
    int (*blt_Tree_ListReplaceVariable) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, const char *varName, int firstIndex, int lastIndex, int objc, Tcl_Obj *const *objv); /* 109 */
    int (*blt_Tree_GetArrayVariable) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, const char *varName, const char *elemName, Tcl_Obj **valueObjPtrPtr); /* 110 */
    int (*blt_Tree_SetArrayVariable) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, const char *varName, const char *elemName, Tcl_Obj *valueObjPtr); /* 111 */
    int (*blt_Tree_UnsetArrayVariable) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, const char *varName, const char *elemName); /* 112 */
    int (*blt_Tree_AppendArrayVariable) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, const char *varName, const char *elemName, Tcl_Obj *objPtr); /* 113 */
    int (*blt_Tree_ListAppendArrayVariable) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, const char *varName, const char *elemName, Tcl_Obj *valueObjPtr); /* 114 */
    int (*blt_Tree_ListReplaceArrayVariable) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, const char *varName, const char *elemName, int firstIndex, int lastIndex, int objc, Tcl_Obj *const *objv); /* 115 */
    int (*blt_Tree_ArrayVariableExists) (Blt_Tree tree, Blt_TreeNode node, const char *varName, const char *elemName); /* 116 */
    int (*blt_Tree_ArrayNames) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, const char *varName, Tcl_Obj *listObjPtr); /* 117 */
    int (*blt_Tree_GetScalarVariableByUid) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, Blt_TreeUid uid, Tcl_Obj **valuePtr); /* 118 */
    int (*blt_Tree_SetScalarVariableByUid) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, Blt_TreeUid uid, Tcl_Obj *valuePtr); /* 119 */
    int (*blt_Tree_UnsetScalarVariableByUid) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, Blt_TreeUid uid); /* 120 */
    int (*blt_Tree_AppendScalarVariableByUid) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, Blt_TreeUid uid, Tcl_Obj *objPtr); /* 121 */
    int (*blt_Tree_ListAppendScalarVariableByUid) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, Blt_TreeUid uid, Tcl_Obj *objPtr); /* 122 */
    int (*blt_Tree_ListReplaceScalarVariableByUid) (Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode node, Blt_TreeUid uid, int firstIndex, int lastIndex, int objc, Tcl_Obj *const *objv); /* 123 */
    int (*blt_Tree_ScalarVariableExistsByUid) (Blt_Tree tree, Blt_TreeNode node, Blt_TreeUid uid); /* 124 */
    Blt_TreeUid (*blt_Tree_FirstVariable) (Blt_Tree tree, Blt_TreeNode node, Blt_TreeVariableIterator *iterPtr); /* 125 */
    Blt_TreeUid (*blt_Tree_NextVariable) (Blt_Tree tree, Blt_TreeVariableIterator *iterPtr); /* 126 */
    int (*blt_Tree_Apply) (Blt_TreeNode root, Blt_TreeApplyProc *proc, ClientData clientData); /* 127 */
    int (*blt_Tree_ApplyDFS) (Blt_TreeNode root, Blt_TreeApplyProc *proc, ClientData clientData, int order); /* 128 */
    int (*blt_Tree_ApplyBFS) (Blt_TreeNode root, Blt_TreeApplyProc *proc, ClientData clientData); /* 129 */
    int (*blt_Tree_SortNode) (Blt_Tree tree, Blt_TreeNode node, Blt_TreeCompareNodesProc *proc); /* 130 */
    int (*blt_Tree_Exists) (Tcl_Interp *interp, const char *name); /* 131 */
    Blt_Tree (*blt_Tree_Open) (Tcl_Interp *interp, const char *name, int flags); /* 132 */
    void (*blt_Tree_Close) (Blt_Tree tree); /* 133 */
    int (*blt_Tree_Attach) (Tcl_Interp *interp, Blt_Tree tree, const char *name); /* 134 */
    Blt_Tree (*blt_Tree_GetFromObj) (Tcl_Interp *interp, Tcl_Obj *objPtr); /* 135 */
    int (*blt_Tree_Size) (Blt_TreeNode node); /* 136 */
    Blt_TreeTrace (*blt_Tree_CreateTrace) (Blt_Tree tree, Blt_TreeNode node, const char *keyPattern, const char *tagName, unsigned int mask, Blt_TreeTraceProc *proc, ClientData clientData); /* 137 */
    void (*blt_Tree_DeleteTrace) (Blt_TreeTrace token); /* 138 */
    void (*blt_Tree_CreateEventHandler) (Blt_Tree tree, unsigned int mask, Blt_TreeNotifyEventProc *proc, ClientData clientData); /* 139 */
    void (*blt_Tree_DeleteEventHandler) (Blt_Tree tree, unsigned int mask, Blt_TreeNotifyEventProc *proc, ClientData clientData); /* 140 */
    void (*blt_Tree_RelabelNode) (Blt_Tree tree, Blt_TreeNode node, const char *string); /* 141 */
    void (*blt_Tree_RelabelNodeWithoutNotify) (Blt_TreeNode node, const char *string); /* 142 */
    const char * (*blt_Tree_NodeIdAscii) (Blt_TreeNode node); /* 143 */
    Tcl_Obj * (*blt_Tree_NodePathObj) (Blt_TreeNode node); /* 144 */
    const char * (*blt_Tree_NodePath) (Blt_TreeNode node); /* 145 */
    const char * (*blt_Tree_NodeRelativePath) (Blt_TreeNode root, Blt_TreeNode node, const char *separator, unsigned int flags, Tcl_Obj *resultPtr); /* 146 */
    long (*blt_Tree_NodePosition) (Blt_TreeNode node); /* 147 */
    void (*blt_Tree_ClearTags) (Blt_Tree tree, Blt_TreeNode node); /* 148 */
    int (*blt_Tree_HasTag) (Blt_Tree tree, Blt_TreeNode node, const char *tagName); /* 149 */
    void (*blt_Tree_AddTag) (Blt_Tree tree, Blt_TreeNode node, const char *tagName); /* 150 */
    void (*blt_Tree_RemoveTag) (Blt_Tree tree, Blt_TreeNode node, const char *tagName); /* 151 */
    void (*blt_Tree_ForgetTag) (Blt_Tree tree, const char *tagName); /* 152 */
    Blt_HashTable * (*blt_Tree_TagHashTable) (Blt_Tree tree, const char *tagName); /* 153 */
    int (*blt_Tree_TagTableIsShared) (Blt_Tree tree); /* 154 */
    void (*blt_Tree_NewTagTable) (Blt_Tree tree); /* 155 */
    Blt_HashEntry * (*blt_Tree_FirstTag) (Blt_Tree tree, Blt_HashSearch *searchPtr); /* 156 */
    long (*blt_Tree_Depth) (Blt_Tree tree); /* 157 */
    int (*blt_Tree_RegisterFormat) (Tcl_Interp *interp, const char *fmtName, Blt_TreeImportProc *importProc, Blt_TreeExportProc *exportProc); /* 158 */
    Blt_TreeTagEntry * (*blt_Tree_RememberTag) (Blt_Tree tree, const char *name); /* 159 */
    int (*blt_Tree_GetNodeFromObj) (Tcl_Interp *interp, Blt_Tree tree, Tcl_Obj *objPtr, Blt_TreeNode *nodePtr); /* 160 */
    int (*blt_Tree_GetNodeIterator) (Tcl_Interp *interp, Blt_Tree tree, Tcl_Obj *objPtr, Blt_TreeNodeIterator *iterPtr); /* 161 */
    Blt_TreeNode (*blt_Tree_FirstTaggedNode) (Blt_TreeNodeIterator *iterPtr); /* 162 */
    Blt_TreeNode (*blt_Tree_NextTaggedNode) (Blt_TreeNodeIterator *iterPtr); /* 163 */
    const char * (*blt_Tree_GetPathSeparator) (Blt_Tree tree); /* 164 */
    void (*blt_Tree_SetPathSeparator) (Blt_Tree tree, const char *sep); /* 165 */
    void (*blt_table_release_tags) (BLT_TABLE table); /* 166 */
    void (*blt_table_new_tags) (BLT_TABLE table); /* 167 */
    Blt_HashTable * (*blt_table_get_column_tag_table) (BLT_TABLE table); /* 168 */
    Blt_HashTable * (*blt_table_get_row_tag_table) (BLT_TABLE table); /* 169 */
    int (*blt_table_exists) (Tcl_Interp *interp, const char *name); /* 170 */
    int (*blt_table_create) (Tcl_Interp *interp, const char *name, BLT_TABLE *tablePtr); /* 171 */
    int (*blt_table_open) (Tcl_Interp *interp, const char *name, BLT_TABLE *tablePtr); /* 172 */
    void (*blt_table_close) (BLT_TABLE table); /* 173 */
    void (*blt_table_clear) (BLT_TABLE table); /* 174 */
    void (*blt_table_pack) (BLT_TABLE table); /* 175 */
    void (*blt_table_reset) (BLT_TABLE table); /* 176 */
    int (*blt_table_same_object) (BLT_TABLE table1, BLT_TABLE table2); /* 177 */
    Blt_HashTable * (*blt_table_row_get_label_table) (BLT_TABLE table, const char *label); /* 178 */
    Blt_HashTable * (*blt_table_column_get_label_table) (BLT_TABLE table, const char *label); /* 179 */
    BLT_TABLE_ROW (*blt_table_get_row) (Tcl_Interp *interp, BLT_TABLE table, Tcl_Obj *objPtr); /* 180 */
    BLT_TABLE_COLUMN (*blt_table_get_column) (Tcl_Interp *interp, BLT_TABLE table, Tcl_Obj *objPtr); /* 181 */
    BLT_TABLE_ROW (*blt_table_get_row_by_label) (BLT_TABLE table, const char *label); /* 182 */
    BLT_TABLE_COLUMN (*blt_table_get_column_by_label) (BLT_TABLE table, const char *label); /* 183 */
    BLT_TABLE_ROW (*blt_table_get_row_by_index) (BLT_TABLE table, long index); /* 184 */
    BLT_TABLE_COLUMN (*blt_table_get_column_by_index) (BLT_TABLE table, long index); /* 185 */
    int (*blt_table_set_row_label) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_ROW row, const char *label); /* 186 */
    int (*blt_table_set_column_label) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_COLUMN column, const char *label); /* 187 */
    BLT_TABLE_COLUMN_TYPE (*blt_table_name_to_column_type) (const char *typeName); /* 188 */
    int (*blt_table_set_column_type) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_COLUMN column, BLT_TABLE_COLUMN_TYPE type); /* 189 */
    const char * (*blt_table_column_type_to_name) (BLT_TABLE_COLUMN_TYPE type); /* 190 */
    int (*blt_table_set_column_tag) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_COLUMN column, const char *tag); /* 191 */
    int (*blt_table_set_row_tag) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_ROW row, const char *tag); /* 192 */
    BLT_TABLE_ROW (*blt_table_create_row) (Tcl_Interp *interp, BLT_TABLE table, const char *label); /* 193 */
    BLT_TABLE_COLUMN (*blt_table_create_column) (Tcl_Interp *interp, BLT_TABLE table, const char *label); /* 194 */
    int (*blt_table_extend_rows) (Tcl_Interp *interp, BLT_TABLE table, size_t n, BLT_TABLE_ROW *rows); /* 195 */
    int (*blt_table_extend_columns) (Tcl_Interp *interp, BLT_TABLE table, size_t n, BLT_TABLE_COLUMN *columms); /* 196 */
    int (*blt_table_delete_row) (BLT_TABLE table, BLT_TABLE_ROW row); /* 197 */
    int (*blt_table_delete_column) (BLT_TABLE table, BLT_TABLE_COLUMN column); /* 198 */
    int (*blt_table_move_rows) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_ROW destRow, BLT_TABLE_ROW firstRow, BLT_TABLE_ROW lastRow, int after); /* 199 */
    int (*blt_table_move_columns) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_COLUMN destColumn, BLT_TABLE_COLUMN firstColumn, BLT_TABLE_COLUMN lastColumn, int after); /* 200 */
    Tcl_Obj * (*blt_table_get_obj) (BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column); /* 201 */
    int (*blt_table_set_obj) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column, Tcl_Obj *objPtr); /* 202 */
    const char * (*blt_table_get_string) (BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column); /* 203 */
    int (*blt_table_set_string_rep) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column, const char *string, int length); /* 204 */
    int (*blt_table_set_string) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column, const char *string, int length); /* 205 */
    int (*blt_table_append_string) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column, const char *string, int length); /* 206 */
    int (*blt_table_set_bytes) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column, const unsigned char *string, int length); /* 207 */
    double (*blt_table_get_double) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column); /* 208 */
    int (*blt_table_set_double) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column, double value); /* 209 */
    long (*blt_table_get_long) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column, long defValue); /* 210 */
    int (*blt_table_set_long) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column, long value); /* 211 */
    int (*blt_table_get_boolean) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column, int defValue); /* 212 */
    int (*blt_table_set_boolean) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column, int value); /* 213 */
    int (*blt_table_set_int64) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column, int64_t value); /* 214 */
    int64_t (*blt_table_get_int64) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column, int64_t defValue); /* 215 */
    BLT_TABLE_VALUE (*blt_table_get_value) (BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column); /* 216 */
    int (*blt_table_set_value) (BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column, BLT_TABLE_VALUE value); /* 217 */
    int (*blt_table_unset_value) (BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column); /* 218 */
    int (*blt_table_value_exists) (BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column); /* 219 */
    const char * (*blt_table_value_string) (BLT_TABLE_VALUE value); /* 220 */
    const unsigned char * (*blt_table_value_bytes) (BLT_TABLE_VALUE value); /* 221 */
    unsigned int (*blt_table_value_length) (BLT_TABLE_VALUE value); /* 222 */
    int (*blt_table_tags_are_shared) (BLT_TABLE table); /* 223 */
    void (*blt_table_clear_row_tags) (BLT_TABLE table, BLT_TABLE_ROW row); /* 224 */
    void (*blt_table_clear_column_tags) (BLT_TABLE table, BLT_TABLE_COLUMN col); /* 225 */
    Blt_Chain (*blt_table_get_row_tags) (BLT_TABLE table, BLT_TABLE_ROW row); /* 226 */
    Blt_Chain (*blt_table_get_column_tags) (BLT_TABLE table, BLT_TABLE_COLUMN column); /* 227 */
    Blt_Chain (*blt_table_get_tagged_rows) (BLT_TABLE table, const char *tag); /* 228 */
    Blt_Chain (*blt_table_get_tagged_columns) (BLT_TABLE table, const char *tag); /* 229 */
    int (*blt_table_row_has_tag) (BLT_TABLE table, BLT_TABLE_ROW row, const char *tag); /* 230 */
    int (*blt_table_column_has_tag) (BLT_TABLE table, BLT_TABLE_COLUMN column, const char *tag); /* 231 */
    int (*blt_table_forget_row_tag) (Tcl_Interp *interp, BLT_TABLE table, const char *tag); /* 232 */
    int (*blt_table_forget_column_tag) (Tcl_Interp *interp, BLT_TABLE table, const char *tag); /* 233 */
    int (*blt_table_unset_row_tag) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_ROW row, const char *tag); /* 234 */
    int (*blt_table_unset_column_tag) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_COLUMN column, const char *tag); /* 235 */
    BLT_TABLE_COLUMN (*blt_table_first_column) (BLT_TABLE table); /* 236 */
    BLT_TABLE_COLUMN (*blt_table_last_column) (BLT_TABLE table); /* 237 */
    BLT_TABLE_COLUMN (*blt_table_next_column) (BLT_TABLE_COLUMN column); /* 238 */
    BLT_TABLE_COLUMN (*blt_table_previous_column) (BLT_TABLE_COLUMN column); /* 239 */
    BLT_TABLE_ROW (*blt_table_first_row) (BLT_TABLE table); /* 240 */
    BLT_TABLE_ROW (*blt_table_last_row) (BLT_TABLE table); /* 241 */
    BLT_TABLE_ROW (*blt_table_next_row) (BLT_TABLE_ROW row); /* 242 */
    BLT_TABLE_ROW (*blt_table_previous_row) (BLT_TABLE_ROW row); /* 243 */
    BLT_TABLE_ROWCOLUMN_SPEC (*blt_table_row_spec) (BLT_TABLE table, Tcl_Obj *objPtr, const char **sp); /* 244 */
    BLT_TABLE_ROWCOLUMN_SPEC (*blt_table_column_spec) (BLT_TABLE table, Tcl_Obj *objPtr, const char **sp); /* 245 */
    int (*blt_table_iterate_rows) (Tcl_Interp *interp, BLT_TABLE table, Tcl_Obj *objPtr, BLT_TABLE_ITERATOR *iter); /* 246 */
    int (*blt_table_iterate_columns) (Tcl_Interp *interp, BLT_TABLE table, Tcl_Obj *objPtr, BLT_TABLE_ITERATOR *iter); /* 247 */
    int (*blt_table_iterate_rows_objv) (Tcl_Interp *interp, BLT_TABLE table, int objc, Tcl_Obj *const *objv, BLT_TABLE_ITERATOR *iterPtr); /* 248 */
    int (*blt_table_iterate_columns_objv) (Tcl_Interp *interp, BLT_TABLE table, int objc, Tcl_Obj *const *objv, BLT_TABLE_ITERATOR *iterPtr); /* 249 */
    void (*blt_table_free_iterator_objv) (BLT_TABLE_ITERATOR *iterPtr); /* 250 */
    void (*blt_table_iterate_all_rows) (BLT_TABLE table, BLT_TABLE_ITERATOR *iterPtr); /* 251 */
    void (*blt_table_iterate_all_columns) (BLT_TABLE table, BLT_TABLE_ITERATOR *iterPtr); /* 252 */
    BLT_TABLE_ROW (*blt_table_first_tagged_row) (BLT_TABLE_ITERATOR *iter); /* 253 */
    BLT_TABLE_COLUMN (*blt_table_first_tagged_column) (BLT_TABLE_ITERATOR *iter); /* 254 */
    BLT_TABLE_ROW (*blt_table_next_tagged_row) (BLT_TABLE_ITERATOR *iter); /* 255 */
    BLT_TABLE_COLUMN (*blt_table_next_tagged_column) (BLT_TABLE_ITERATOR *iter); /* 256 */
    int (*blt_table_list_rows) (Tcl_Interp *interp, BLT_TABLE table, int objc, Tcl_Obj *const *objv, Blt_Chain chain); /* 257 */
    int (*blt_table_list_columns) (Tcl_Interp *interp, BLT_TABLE table, int objc, Tcl_Obj *const *objv, Blt_Chain chain); /* 258 */
    void (*blt_table_clear_row_traces) (BLT_TABLE table, BLT_TABLE_ROW row); /* 259 */
    void (*blt_table_clear_column_traces) (BLT_TABLE table, BLT_TABLE_COLUMN column); /* 260 */
    BLT_TABLE_TRACE (*blt_table_create_trace) (BLT_TABLE table, BLT_TABLE_ROW row, BLT_TABLE_COLUMN column, const char *rowTag, const char *columnTag, unsigned int mask, BLT_TABLE_TRACE_PROC *proc, BLT_TABLE_TRACE_DELETE_PROC *deleteProc, ClientData clientData); /* 261 */
    void (*blt_table_trace_column) (BLT_TABLE table, BLT_TABLE_COLUMN column, unsigned int mask, BLT_TABLE_TRACE_PROC *proc, BLT_TABLE_TRACE_DELETE_PROC *deleteProc, ClientData clientData); /* 262 */
    void (*blt_table_trace_row) (BLT_TABLE table, BLT_TABLE_ROW row, unsigned int mask, BLT_TABLE_TRACE_PROC *proc, BLT_TABLE_TRACE_DELETE_PROC *deleteProc, ClientData clientData); /* 263 */
    BLT_TABLE_TRACE (*blt_table_create_column_trace) (BLT_TABLE table, BLT_TABLE_COLUMN column, unsigned int mask, BLT_TABLE_TRACE_PROC *proc, BLT_TABLE_TRACE_DELETE_PROC *deleteProc, ClientData clientData); /* 264 */
    BLT_TABLE_TRACE (*blt_table_create_column_tag_trace) (BLT_TABLE table, const char *tag, unsigned int mask, BLT_TABLE_TRACE_PROC *proc, BLT_TABLE_TRACE_DELETE_PROC *deleteProc, ClientData clientData); /* 265 */
    BLT_TABLE_TRACE (*blt_table_create_row_trace) (BLT_TABLE table, BLT_TABLE_ROW row, unsigned int mask, BLT_TABLE_TRACE_PROC *proc, BLT_TABLE_TRACE_DELETE_PROC *deleteProc, ClientData clientData); /* 266 */
    BLT_TABLE_TRACE (*blt_table_create_row_tag_trace) (BLT_TABLE table, const char *tag, unsigned int mask, BLT_TABLE_TRACE_PROC *proc, BLT_TABLE_TRACE_DELETE_PROC *deleteProc, ClientData clientData); /* 267 */
    void (*blt_table_delete_trace) (BLT_TABLE table, BLT_TABLE_TRACE trace); /* 268 */
    BLT_TABLE_NOTIFIER (*blt_table_create_notifier) (Tcl_Interp *interp, BLT_TABLE table, unsigned int mask, BLT_TABLE_NOTIFY_EVENT_PROC *proc, BLT_TABLE_NOTIFIER_DELETE_PROC *deleteProc, ClientData clientData); /* 269 */
    BLT_TABLE_NOTIFIER (*blt_table_create_row_notifier) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_ROW row, unsigned int mask, BLT_TABLE_NOTIFY_EVENT_PROC *proc, BLT_TABLE_NOTIFIER_DELETE_PROC *deleteProc, ClientData clientData); /* 270 */
    BLT_TABLE_NOTIFIER (*blt_table_create_row_tag_notifier) (Tcl_Interp *interp, BLT_TABLE table, const char *tag, unsigned int mask, BLT_TABLE_NOTIFY_EVENT_PROC *proc, BLT_TABLE_NOTIFIER_DELETE_PROC *deleteProc, ClientData clientData); /* 271 */
    BLT_TABLE_NOTIFIER (*blt_table_create_column_notifier) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_COLUMN column, unsigned int mask, BLT_TABLE_NOTIFY_EVENT_PROC *proc, BLT_TABLE_NOTIFIER_DELETE_PROC *deleteProc, ClientData clientData); /* 272 */
    BLT_TABLE_NOTIFIER (*blt_table_create_column_tag_notifier) (Tcl_Interp *interp, BLT_TABLE table, const char *tag, unsigned int mask, BLT_TABLE_NOTIFY_EVENT_PROC *proc, BLT_TABLE_NOTIFIER_DELETE_PROC *deleteProc, ClientData clientData); /* 273 */
    void (*blt_table_delete_notifier) (BLT_TABLE table, BLT_TABLE_NOTIFIER notifier); /* 274 */
    void (*blt_table_sort_init) (BLT_TABLE table, BLT_TABLE_SORT_ORDER *order, long numCompares, unsigned int flags); /* 275 */
    BLT_TABLE_ROW * (*blt_table_sort_rows) (BLT_TABLE table); /* 276 */
    void (*blt_table_sort_row_map) (BLT_TABLE table, long numRows, BLT_TABLE_ROW *rows); /* 277 */
    void (*blt_table_sort_finish) (void); /* 278 */
    BLT_TABLE_COMPARE_PROC * (*blt_table_get_compare_proc) (BLT_TABLE table, BLT_TABLE_COLUMN column, unsigned int flags); /* 279 */
    BLT_TABLE_ROW * (*blt_table_get_row_map) (BLT_TABLE table); /* 280 */
    BLT_TABLE_COLUMN * (*blt_table_get_column_map) (BLT_TABLE table); /* 281 */
    void (*blt_table_set_row_map) (BLT_TABLE table, BLT_TABLE_ROW *map); /* 282 */
    void (*blt_table_set_column_map) (BLT_TABLE table, BLT_TABLE_COLUMN *map); /* 283 */
    int (*blt_table_restore) (Tcl_Interp *interp, BLT_TABLE table, char *string, unsigned int flags); /* 284 */
    int (*blt_table_file_restore) (Tcl_Interp *interp, BLT_TABLE table, const char *fileName, unsigned int flags); /* 285 */
    int (*blt_table_register_format) (Tcl_Interp *interp, const char *name, BLT_TABLE_IMPORT_PROC *importProc, BLT_TABLE_EXPORT_PROC *exportProc); /* 286 */
    void (*blt_table_unset_keys) (BLT_TABLE table); /* 287 */
    int (*blt_table_get_keys) (BLT_TABLE table, BLT_TABLE_COLUMN **keysPtr); /* 288 */
    int (*blt_table_set_keys) (BLT_TABLE table, int numKeys, BLT_TABLE_COLUMN *keys, int unique); /* 289 */
    int (*blt_table_key_lookup) (Tcl_Interp *interp, BLT_TABLE table, int objc, Tcl_Obj *const *objv, BLT_TABLE_ROW *rowPtr); /* 290 */
    int (*blt_table_get_column_limits) (Tcl_Interp *interp, BLT_TABLE table, BLT_TABLE_COLUMN col, Tcl_Obj **minObjPtrPtr, Tcl_Obj **maxObjPtrPtr); /* 291 */
    BLT_TABLE_ROW (*blt_table_row) (BLT_TABLE table, long index); /* 292 */
    BLT_TABLE_COLUMN (*blt_table_column) (BLT_TABLE table, long index); /* 293 */
    long (*blt_table_row_index) (BLT_TABLE table, BLT_TABLE_ROW row); /* 294 */
    long (*blt_table_column_index) (BLT_TABLE table, BLT_TABLE_COLUMN column); /* 295 */
    double (*blt_VecMin) (Blt_Vector *vPtr); /* 296 */
    double (*blt_VecMax) (Blt_Vector *vPtr); /* 297 */
    Blt_VectorToken (*blt_GetVectorToken) (Tcl_Interp *interp, const char *vecName); /* 298 */
    void (*blt_SetVectorChangedProc) (Blt_VectorToken clientId, Blt_VectorChangedProc *proc, ClientData clientData); /* 299 */
    void (*blt_FreeVectorToken) (Blt_VectorToken clientId); /* 300 */
    int (*blt_GetVectorFromToken) (Tcl_Interp *interp, Blt_VectorToken clientId, Blt_Vector **vecPtrPtr); /* 301 */
    const char * (*blt_NameOfVectorFromToken) (Blt_VectorToken clientId); /* 302 */
    const char * (*blt_NameOfVector) (Blt_Vector *vecPtr); /* 303 */
    int (*blt_VectorNotifyPending) (Blt_VectorToken clientId); /* 304 */
    int (*blt_CreateVector) (Tcl_Interp *interp, const char *vecName, int size, Blt_Vector **vecPtrPtr); /* 305 */
    int (*blt_CreateVector2) (Tcl_Interp *interp, const char *vecName, const char *cmdName, const char *varName, int initialSize, Blt_Vector **vecPtrPtr); /* 306 */
    int (*blt_GetVector) (Tcl_Interp *interp, const char *vecName, Blt_Vector **vecPtrPtr); /* 307 */
    int (*blt_GetVectorFromObj) (Tcl_Interp *interp, Tcl_Obj *objPtr, Blt_Vector **vecPtrPtr); /* 308 */
    int (*blt_VectorExists) (Tcl_Interp *interp, const char *vecName); /* 309 */
    int (*blt_ResetVector) (Blt_Vector *vecPtr, double *dataArr, int n, int arraySize, Tcl_FreeProc *freeProc); /* 310 */
    int (*blt_ResizeVector) (Blt_Vector *vecPtr, int n); /* 311 */
    int (*blt_DeleteVectorByName) (Tcl_Interp *interp, const char *vecName); /* 312 */
    int (*blt_DeleteVector) (Blt_Vector *vecPtr); /* 313 */
    int (*blt_ExprVector) (Tcl_Interp *interp, char *expr, Blt_Vector *vecPtr); /* 314 */
    void (*blt_InstallIndexProc) (Tcl_Interp *interp, const char *indexName, Blt_VectorIndexProc *procPtr); /* 315 */
    int (*blt_VectorExists2) (Tcl_Interp *interp, const char *vecName); /* 316 */
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
#ifndef Blt_ExprDoubleFromObj
#define Blt_ExprDoubleFromObj \
	(bltTclProcsPtr->blt_ExprDoubleFromObj) /* 69 */
#endif
#ifndef Blt_ExprIntFromObj
#define Blt_ExprIntFromObj \
	(bltTclProcsPtr->blt_ExprIntFromObj) /* 70 */
#endif
#ifndef Blt_GetStateFromObj
#define Blt_GetStateFromObj \
	(bltTclProcsPtr->blt_GetStateFromObj) /* 71 */
#endif
#ifndef Blt_NameOfState
#define Blt_NameOfState \
	(bltTclProcsPtr->blt_NameOfState) /* 72 */
#endif
#ifndef Blt_GetFillFromObj
#define Blt_GetFillFromObj \
	(bltTclProcsPtr->blt_GetFillFromObj) /* 73 */
#endif
#ifndef Blt_NameOfFill
#define Blt_NameOfFill \
	(bltTclProcsPtr->blt_NameOfFill) /* 74 */
#endif
#ifndef Blt_GetResizeFromObj
#define Blt_GetResizeFromObj \
	(bltTclProcsPtr->blt_GetResizeFromObj) /* 75 */
#endif
#ifndef Blt_NameOfResize
#define Blt_NameOfResize \
	(bltTclProcsPtr->blt_NameOfResize) /* 76 */
#endif
#ifndef Blt_GetSideFromObj
#define Blt_GetSideFromObj \
	(bltTclProcsPtr->blt_GetSideFromObj) /* 77 */
#endif
#ifndef Blt_NameOfSide
#define Blt_NameOfSide \
	(bltTclProcsPtr->blt_NameOfSide) /* 78 */
#endif
#ifndef Blt_GetCount
#define Blt_GetCount \
	(bltTclProcsPtr->blt_GetCount) /* 79 */
#endif
#ifndef Blt_GetCountFromObj
#define Blt_GetCountFromObj \
	(bltTclProcsPtr->blt_GetCountFromObj) /* 80 */
#endif
#ifndef Blt_ParseSwitches
#define Blt_ParseSwitches \
	(bltTclProcsPtr->blt_ParseSwitches) /* 81 */
#endif
#ifndef Blt_FreeSwitches
#define Blt_FreeSwitches \
	(bltTclProcsPtr->blt_FreeSwitches) /* 82 */
#endif
#ifndef Blt_SwitchChanged
#define Blt_SwitchChanged \
	(bltTclProcsPtr->blt_SwitchChanged) /* 83 */
#endif
#ifndef Blt_SwitchInfo
#define Blt_SwitchInfo \
	(bltTclProcsPtr->blt_SwitchInfo) /* 84 */
#endif
#ifndef Blt_SwitchValue
#define Blt_SwitchValue \
	(bltTclProcsPtr->blt_SwitchValue) /* 85 */
#endif
#ifndef Blt_Tree_GetUid
#define Blt_Tree_GetUid \
	(bltTclProcsPtr->blt_Tree_GetUid) /* 86 */
#endif
#ifndef Blt_Tree_GetUidFromNode
#define Blt_Tree_GetUidFromNode \
	(bltTclProcsPtr->blt_Tree_GetUidFromNode) /* 87 */
#endif
#ifndef Blt_Tree_GetNextId
#define Blt_Tree_GetNextId \
	(bltTclProcsPtr->blt_Tree_GetNextId) /* 88 */
#endif
#ifndef Blt_Tree_CreateNode
#define Blt_Tree_CreateNode \
	(bltTclProcsPtr->blt_Tree_CreateNode) /* 89 */
#endif
#ifndef Blt_Tree_CreateNodeWithId
#define Blt_Tree_CreateNodeWithId \
	(bltTclProcsPtr->blt_Tree_CreateNodeWithId) /* 90 */
#endif
#ifndef Blt_Tree_DeleteNode
#define Blt_Tree_DeleteNode \
	(bltTclProcsPtr->blt_Tree_DeleteNode) /* 91 */
#endif
#ifndef Blt_Tree_MoveNode
#define Blt_Tree_MoveNode \
	(bltTclProcsPtr->blt_Tree_MoveNode) /* 92 */
#endif
#ifndef Blt_Tree_GetNodeFromIndex
#define Blt_Tree_GetNodeFromIndex \
	(bltTclProcsPtr->blt_Tree_GetNodeFromIndex) /* 93 */
#endif
#ifndef Blt_Tree_FindChild
#define Blt_Tree_FindChild \
	(bltTclProcsPtr->blt_Tree_FindChild) /* 94 */
#endif
#ifndef Blt_Tree_NextNode
#define Blt_Tree_NextNode \
	(bltTclProcsPtr->blt_Tree_NextNode) /* 95 */
#endif
#ifndef Blt_Tree_PrevNode
#define Blt_Tree_PrevNode \
	(bltTclProcsPtr->blt_Tree_PrevNode) /* 96 */
#endif
#ifndef Blt_Tree_FirstChild
#define Blt_Tree_FirstChild \
	(bltTclProcsPtr->blt_Tree_FirstChild) /* 97 */
#endif
#ifndef Blt_Tree_LastChild
#define Blt_Tree_LastChild \
	(bltTclProcsPtr->blt_Tree_LastChild) /* 98 */
#endif
#ifndef Blt_Tree_IsBefore
#define Blt_Tree_IsBefore \
	(bltTclProcsPtr->blt_Tree_IsBefore) /* 99 */
#endif
#ifndef Blt_Tree_IsAncestor
#define Blt_Tree_IsAncestor \
	(bltTclProcsPtr->blt_Tree_IsAncestor) /* 100 */
#endif
#ifndef Blt_Tree_PrivateVariable
#define Blt_Tree_PrivateVariable \
	(bltTclProcsPtr->blt_Tree_PrivateVariable) /* 101 */
#endif
#ifndef Blt_Tree_PublicVariable
#define Blt_Tree_PublicVariable \
	(bltTclProcsPtr->blt_Tree_PublicVariable) /* 102 */
#endif
#ifndef Blt_Tree_GetVariable
#define Blt_Tree_GetVariable \
	(bltTclProcsPtr->blt_Tree_GetVariable) /* 103 */
#endif
#ifndef Blt_Tree_VariableExists
#define Blt_Tree_VariableExists \
	(bltTclProcsPtr->blt_Tree_VariableExists) /* 104 */
#endif
#ifndef Blt_Tree_SetVariable
#define Blt_Tree_SetVariable \
	(bltTclProcsPtr->blt_Tree_SetVariable) /* 105 */
#endif
#ifndef Blt_Tree_UnsetVariable
#define Blt_Tree_UnsetVariable \
	(bltTclProcsPtr->blt_Tree_UnsetVariable) /* 106 */
#endif
#ifndef Blt_Tree_AppendVariable
#define Blt_Tree_AppendVariable \
	(bltTclProcsPtr->blt_Tree_AppendVariable) /* 107 */
#endif
#ifndef Blt_Tree_ListAppendVariable
#define Blt_Tree_ListAppendVariable \
	(bltTclProcsPtr->blt_Tree_ListAppendVariable) /* 108 */
#endif
#ifndef Blt_Tree_ListReplaceVariable
#define Blt_Tree_ListReplaceVariable \
	(bltTclProcsPtr->blt_Tree_ListReplaceVariable) /* 109 */
#endif
#ifndef Blt_Tree_GetArrayVariable
#define Blt_Tree_GetArrayVariable \
	(bltTclProcsPtr->blt_Tree_GetArrayVariable) /* 110 */
#endif
#ifndef Blt_Tree_SetArrayVariable
#define Blt_Tree_SetArrayVariable \
	(bltTclProcsPtr->blt_Tree_SetArrayVariable) /* 111 */
#endif
#ifndef Blt_Tree_UnsetArrayVariable
#define Blt_Tree_UnsetArrayVariable \
	(bltTclProcsPtr->blt_Tree_UnsetArrayVariable) /* 112 */
#endif
#ifndef Blt_Tree_AppendArrayVariable
#define Blt_Tree_AppendArrayVariable \
	(bltTclProcsPtr->blt_Tree_AppendArrayVariable) /* 113 */
#endif
#ifndef Blt_Tree_ListAppendArrayVariable
#define Blt_Tree_ListAppendArrayVariable \
	(bltTclProcsPtr->blt_Tree_ListAppendArrayVariable) /* 114 */
#endif
#ifndef Blt_Tree_ListReplaceArrayVariable
#define Blt_Tree_ListReplaceArrayVariable \
	(bltTclProcsPtr->blt_Tree_ListReplaceArrayVariable) /* 115 */
#endif
#ifndef Blt_Tree_ArrayVariableExists
#define Blt_Tree_ArrayVariableExists \
	(bltTclProcsPtr->blt_Tree_ArrayVariableExists) /* 116 */
#endif
#ifndef Blt_Tree_ArrayNames
#define Blt_Tree_ArrayNames \
	(bltTclProcsPtr->blt_Tree_ArrayNames) /* 117 */
#endif
#ifndef Blt_Tree_GetScalarVariableByUid
#define Blt_Tree_GetScalarVariableByUid \
	(bltTclProcsPtr->blt_Tree_GetScalarVariableByUid) /* 118 */
#endif
#ifndef Blt_Tree_SetScalarVariableByUid
#define Blt_Tree_SetScalarVariableByUid \
	(bltTclProcsPtr->blt_Tree_SetScalarVariableByUid) /* 119 */
#endif
#ifndef Blt_Tree_UnsetScalarVariableByUid
#define Blt_Tree_UnsetScalarVariableByUid \
	(bltTclProcsPtr->blt_Tree_UnsetScalarVariableByUid) /* 120 */
#endif
#ifndef Blt_Tree_AppendScalarVariableByUid
#define Blt_Tree_AppendScalarVariableByUid \
	(bltTclProcsPtr->blt_Tree_AppendScalarVariableByUid) /* 121 */
#endif
#ifndef Blt_Tree_ListAppendScalarVariableByUid
#define Blt_Tree_ListAppendScalarVariableByUid \
	(bltTclProcsPtr->blt_Tree_ListAppendScalarVariableByUid) /* 122 */
#endif
#ifndef Blt_Tree_ListReplaceScalarVariableByUid
#define Blt_Tree_ListReplaceScalarVariableByUid \
	(bltTclProcsPtr->blt_Tree_ListReplaceScalarVariableByUid) /* 123 */
#endif
#ifndef Blt_Tree_ScalarVariableExistsByUid
#define Blt_Tree_ScalarVariableExistsByUid \
	(bltTclProcsPtr->blt_Tree_ScalarVariableExistsByUid) /* 124 */
#endif
#ifndef Blt_Tree_FirstVariable
#define Blt_Tree_FirstVariable \
	(bltTclProcsPtr->blt_Tree_FirstVariable) /* 125 */
#endif
#ifndef Blt_Tree_NextVariable
#define Blt_Tree_NextVariable \
	(bltTclProcsPtr->blt_Tree_NextVariable) /* 126 */
#endif
#ifndef Blt_Tree_Apply
#define Blt_Tree_Apply \
	(bltTclProcsPtr->blt_Tree_Apply) /* 127 */
#endif
#ifndef Blt_Tree_ApplyDFS
#define Blt_Tree_ApplyDFS \
	(bltTclProcsPtr->blt_Tree_ApplyDFS) /* 128 */
#endif
#ifndef Blt_Tree_ApplyBFS
#define Blt_Tree_ApplyBFS \
	(bltTclProcsPtr->blt_Tree_ApplyBFS) /* 129 */
#endif
#ifndef Blt_Tree_SortNode
#define Blt_Tree_SortNode \
	(bltTclProcsPtr->blt_Tree_SortNode) /* 130 */
#endif
#ifndef Blt_Tree_Exists
#define Blt_Tree_Exists \
	(bltTclProcsPtr->blt_Tree_Exists) /* 131 */
#endif
#ifndef Blt_Tree_Open
#define Blt_Tree_Open \
	(bltTclProcsPtr->blt_Tree_Open) /* 132 */
#endif
#ifndef Blt_Tree_Close
#define Blt_Tree_Close \
	(bltTclProcsPtr->blt_Tree_Close) /* 133 */
#endif
#ifndef Blt_Tree_Attach
#define Blt_Tree_Attach \
	(bltTclProcsPtr->blt_Tree_Attach) /* 134 */
#endif
#ifndef Blt_Tree_GetFromObj
#define Blt_Tree_GetFromObj \
	(bltTclProcsPtr->blt_Tree_GetFromObj) /* 135 */
#endif
#ifndef Blt_Tree_Size
#define Blt_Tree_Size \
	(bltTclProcsPtr->blt_Tree_Size) /* 136 */
#endif
#ifndef Blt_Tree_CreateTrace
#define Blt_Tree_CreateTrace \
	(bltTclProcsPtr->blt_Tree_CreateTrace) /* 137 */
#endif
#ifndef Blt_Tree_DeleteTrace
#define Blt_Tree_DeleteTrace \
	(bltTclProcsPtr->blt_Tree_DeleteTrace) /* 138 */
#endif
#ifndef Blt_Tree_CreateEventHandler
#define Blt_Tree_CreateEventHandler \
	(bltTclProcsPtr->blt_Tree_CreateEventHandler) /* 139 */
#endif
#ifndef Blt_Tree_DeleteEventHandler
#define Blt_Tree_DeleteEventHandler \
	(bltTclProcsPtr->blt_Tree_DeleteEventHandler) /* 140 */
#endif
#ifndef Blt_Tree_RelabelNode
#define Blt_Tree_RelabelNode \
	(bltTclProcsPtr->blt_Tree_RelabelNode) /* 141 */
#endif
#ifndef Blt_Tree_RelabelNodeWithoutNotify
#define Blt_Tree_RelabelNodeWithoutNotify \
	(bltTclProcsPtr->blt_Tree_RelabelNodeWithoutNotify) /* 142 */
#endif
#ifndef Blt_Tree_NodeIdAscii
#define Blt_Tree_NodeIdAscii \
	(bltTclProcsPtr->blt_Tree_NodeIdAscii) /* 143 */
#endif
#ifndef Blt_Tree_NodePathObj
#define Blt_Tree_NodePathObj \
	(bltTclProcsPtr->blt_Tree_NodePathObj) /* 144 */
#endif
#ifndef Blt_Tree_NodePath
#define Blt_Tree_NodePath \
	(bltTclProcsPtr->blt_Tree_NodePath) /* 145 */
#endif
#ifndef Blt_Tree_NodeRelativePath
#define Blt_Tree_NodeRelativePath \
	(bltTclProcsPtr->blt_Tree_NodeRelativePath) /* 146 */
#endif
#ifndef Blt_Tree_NodePosition
#define Blt_Tree_NodePosition \
	(bltTclProcsPtr->blt_Tree_NodePosition) /* 147 */
#endif
#ifndef Blt_Tree_ClearTags
#define Blt_Tree_ClearTags \
	(bltTclProcsPtr->blt_Tree_ClearTags) /* 148 */
#endif
#ifndef Blt_Tree_HasTag
#define Blt_Tree_HasTag \
	(bltTclProcsPtr->blt_Tree_HasTag) /* 149 */
#endif
#ifndef Blt_Tree_AddTag
#define Blt_Tree_AddTag \
	(bltTclProcsPtr->blt_Tree_AddTag) /* 150 */
#endif
#ifndef Blt_Tree_RemoveTag
#define Blt_Tree_RemoveTag \
	(bltTclProcsPtr->blt_Tree_RemoveTag) /* 151 */
#endif
#ifndef Blt_Tree_ForgetTag
#define Blt_Tree_ForgetTag \
	(bltTclProcsPtr->blt_Tree_ForgetTag) /* 152 */
#endif
#ifndef Blt_Tree_TagHashTable
#define Blt_Tree_TagHashTable \
	(bltTclProcsPtr->blt_Tree_TagHashTable) /* 153 */
#endif
#ifndef Blt_Tree_TagTableIsShared
#define Blt_Tree_TagTableIsShared \
	(bltTclProcsPtr->blt_Tree_TagTableIsShared) /* 154 */
#endif
#ifndef Blt_Tree_NewTagTable
#define Blt_Tree_NewTagTable \
	(bltTclProcsPtr->blt_Tree_NewTagTable) /* 155 */
#endif
#ifndef Blt_Tree_FirstTag
#define Blt_Tree_FirstTag \
	(bltTclProcsPtr->blt_Tree_FirstTag) /* 156 */
#endif
#ifndef Blt_Tree_Depth
#define Blt_Tree_Depth \
	(bltTclProcsPtr->blt_Tree_Depth) /* 157 */
#endif
#ifndef Blt_Tree_RegisterFormat
#define Blt_Tree_RegisterFormat \
	(bltTclProcsPtr->blt_Tree_RegisterFormat) /* 158 */
#endif
#ifndef Blt_Tree_RememberTag
#define Blt_Tree_RememberTag \
	(bltTclProcsPtr->blt_Tree_RememberTag) /* 159 */
#endif
#ifndef Blt_Tree_GetNodeFromObj
#define Blt_Tree_GetNodeFromObj \
	(bltTclProcsPtr->blt_Tree_GetNodeFromObj) /* 160 */
#endif
#ifndef Blt_Tree_GetNodeIterator
#define Blt_Tree_GetNodeIterator \
	(bltTclProcsPtr->blt_Tree_GetNodeIterator) /* 161 */
#endif
#ifndef Blt_Tree_FirstTaggedNode
#define Blt_Tree_FirstTaggedNode \
	(bltTclProcsPtr->blt_Tree_FirstTaggedNode) /* 162 */
#endif
#ifndef Blt_Tree_NextTaggedNode
#define Blt_Tree_NextTaggedNode \
	(bltTclProcsPtr->blt_Tree_NextTaggedNode) /* 163 */
#endif
#ifndef Blt_Tree_GetPathSeparator
#define Blt_Tree_GetPathSeparator \
	(bltTclProcsPtr->blt_Tree_GetPathSeparator) /* 164 */
#endif
#ifndef Blt_Tree_SetPathSeparator
#define Blt_Tree_SetPathSeparator \
	(bltTclProcsPtr->blt_Tree_SetPathSeparator) /* 165 */
#endif
#ifndef blt_table_release_tags
#define blt_table_release_tags \
	(bltTclProcsPtr->blt_table_release_tags) /* 166 */
#endif
#ifndef blt_table_new_tags
#define blt_table_new_tags \
	(bltTclProcsPtr->blt_table_new_tags) /* 167 */
#endif
#ifndef blt_table_get_column_tag_table
#define blt_table_get_column_tag_table \
	(bltTclProcsPtr->blt_table_get_column_tag_table) /* 168 */
#endif
#ifndef blt_table_get_row_tag_table
#define blt_table_get_row_tag_table \
	(bltTclProcsPtr->blt_table_get_row_tag_table) /* 169 */
#endif
#ifndef blt_table_exists
#define blt_table_exists \
	(bltTclProcsPtr->blt_table_exists) /* 170 */
#endif
#ifndef blt_table_create
#define blt_table_create \
	(bltTclProcsPtr->blt_table_create) /* 171 */
#endif
#ifndef blt_table_open
#define blt_table_open \
	(bltTclProcsPtr->blt_table_open) /* 172 */
#endif
#ifndef blt_table_close
#define blt_table_close \
	(bltTclProcsPtr->blt_table_close) /* 173 */
#endif
#ifndef blt_table_clear
#define blt_table_clear \
	(bltTclProcsPtr->blt_table_clear) /* 174 */
#endif
#ifndef blt_table_pack
#define blt_table_pack \
	(bltTclProcsPtr->blt_table_pack) /* 175 */
#endif
#ifndef blt_table_reset
#define blt_table_reset \
	(bltTclProcsPtr->blt_table_reset) /* 176 */
#endif
#ifndef blt_table_same_object
#define blt_table_same_object \
	(bltTclProcsPtr->blt_table_same_object) /* 177 */
#endif
#ifndef blt_table_row_get_label_table
#define blt_table_row_get_label_table \
	(bltTclProcsPtr->blt_table_row_get_label_table) /* 178 */
#endif
#ifndef blt_table_column_get_label_table
#define blt_table_column_get_label_table \
	(bltTclProcsPtr->blt_table_column_get_label_table) /* 179 */
#endif
#ifndef blt_table_get_row
#define blt_table_get_row \
	(bltTclProcsPtr->blt_table_get_row) /* 180 */
#endif
#ifndef blt_table_get_column
#define blt_table_get_column \
	(bltTclProcsPtr->blt_table_get_column) /* 181 */
#endif
#ifndef blt_table_get_row_by_label
#define blt_table_get_row_by_label \
	(bltTclProcsPtr->blt_table_get_row_by_label) /* 182 */
#endif
#ifndef blt_table_get_column_by_label
#define blt_table_get_column_by_label \
	(bltTclProcsPtr->blt_table_get_column_by_label) /* 183 */
#endif
#ifndef blt_table_get_row_by_index
#define blt_table_get_row_by_index \
	(bltTclProcsPtr->blt_table_get_row_by_index) /* 184 */
#endif
#ifndef blt_table_get_column_by_index
#define blt_table_get_column_by_index \
	(bltTclProcsPtr->blt_table_get_column_by_index) /* 185 */
#endif
#ifndef blt_table_set_row_label
#define blt_table_set_row_label \
	(bltTclProcsPtr->blt_table_set_row_label) /* 186 */
#endif
#ifndef blt_table_set_column_label
#define blt_table_set_column_label \
	(bltTclProcsPtr->blt_table_set_column_label) /* 187 */
#endif
#ifndef blt_table_name_to_column_type
#define blt_table_name_to_column_type \
	(bltTclProcsPtr->blt_table_name_to_column_type) /* 188 */
#endif
#ifndef blt_table_set_column_type
#define blt_table_set_column_type \
	(bltTclProcsPtr->blt_table_set_column_type) /* 189 */
#endif
#ifndef blt_table_column_type_to_name
#define blt_table_column_type_to_name \
	(bltTclProcsPtr->blt_table_column_type_to_name) /* 190 */
#endif
#ifndef blt_table_set_column_tag
#define blt_table_set_column_tag \
	(bltTclProcsPtr->blt_table_set_column_tag) /* 191 */
#endif
#ifndef blt_table_set_row_tag
#define blt_table_set_row_tag \
	(bltTclProcsPtr->blt_table_set_row_tag) /* 192 */
#endif
#ifndef blt_table_create_row
#define blt_table_create_row \
	(bltTclProcsPtr->blt_table_create_row) /* 193 */
#endif
#ifndef blt_table_create_column
#define blt_table_create_column \
	(bltTclProcsPtr->blt_table_create_column) /* 194 */
#endif
#ifndef blt_table_extend_rows
#define blt_table_extend_rows \
	(bltTclProcsPtr->blt_table_extend_rows) /* 195 */
#endif
#ifndef blt_table_extend_columns
#define blt_table_extend_columns \
	(bltTclProcsPtr->blt_table_extend_columns) /* 196 */
#endif
#ifndef blt_table_delete_row
#define blt_table_delete_row \
	(bltTclProcsPtr->blt_table_delete_row) /* 197 */
#endif
#ifndef blt_table_delete_column
#define blt_table_delete_column \
	(bltTclProcsPtr->blt_table_delete_column) /* 198 */
#endif
#ifndef blt_table_move_rows
#define blt_table_move_rows \
	(bltTclProcsPtr->blt_table_move_rows) /* 199 */
#endif
#ifndef blt_table_move_columns
#define blt_table_move_columns \
	(bltTclProcsPtr->blt_table_move_columns) /* 200 */
#endif
#ifndef blt_table_get_obj
#define blt_table_get_obj \
	(bltTclProcsPtr->blt_table_get_obj) /* 201 */
#endif
#ifndef blt_table_set_obj
#define blt_table_set_obj \
	(bltTclProcsPtr->blt_table_set_obj) /* 202 */
#endif
#ifndef blt_table_get_string
#define blt_table_get_string \
	(bltTclProcsPtr->blt_table_get_string) /* 203 */
#endif
#ifndef blt_table_set_string_rep
#define blt_table_set_string_rep \
	(bltTclProcsPtr->blt_table_set_string_rep) /* 204 */
#endif
#ifndef blt_table_set_string
#define blt_table_set_string \
	(bltTclProcsPtr->blt_table_set_string) /* 205 */
#endif
#ifndef blt_table_append_string
#define blt_table_append_string \
	(bltTclProcsPtr->blt_table_append_string) /* 206 */
#endif
#ifndef blt_table_set_bytes
#define blt_table_set_bytes \
	(bltTclProcsPtr->blt_table_set_bytes) /* 207 */
#endif
#ifndef blt_table_get_double
#define blt_table_get_double \
	(bltTclProcsPtr->blt_table_get_double) /* 208 */
#endif
#ifndef blt_table_set_double
#define blt_table_set_double \
	(bltTclProcsPtr->blt_table_set_double) /* 209 */
#endif
#ifndef blt_table_get_long
#define blt_table_get_long \
	(bltTclProcsPtr->blt_table_get_long) /* 210 */
#endif
#ifndef blt_table_set_long
#define blt_table_set_long \
	(bltTclProcsPtr->blt_table_set_long) /* 211 */
#endif
#ifndef blt_table_get_boolean
#define blt_table_get_boolean \
	(bltTclProcsPtr->blt_table_get_boolean) /* 212 */
#endif
#ifndef blt_table_set_boolean
#define blt_table_set_boolean \
	(bltTclProcsPtr->blt_table_set_boolean) /* 213 */
#endif
#ifndef blt_table_set_int64
#define blt_table_set_int64 \
	(bltTclProcsPtr->blt_table_set_int64) /* 214 */
#endif
#ifndef blt_table_get_int64
#define blt_table_get_int64 \
	(bltTclProcsPtr->blt_table_get_int64) /* 215 */
#endif
#ifndef blt_table_get_value
#define blt_table_get_value \
	(bltTclProcsPtr->blt_table_get_value) /* 216 */
#endif
#ifndef blt_table_set_value
#define blt_table_set_value \
	(bltTclProcsPtr->blt_table_set_value) /* 217 */
#endif
#ifndef blt_table_unset_value
#define blt_table_unset_value \
	(bltTclProcsPtr->blt_table_unset_value) /* 218 */
#endif
#ifndef blt_table_value_exists
#define blt_table_value_exists \
	(bltTclProcsPtr->blt_table_value_exists) /* 219 */
#endif
#ifndef blt_table_value_string
#define blt_table_value_string \
	(bltTclProcsPtr->blt_table_value_string) /* 220 */
#endif
#ifndef blt_table_value_bytes
#define blt_table_value_bytes \
	(bltTclProcsPtr->blt_table_value_bytes) /* 221 */
#endif
#ifndef blt_table_value_length
#define blt_table_value_length \
	(bltTclProcsPtr->blt_table_value_length) /* 222 */
#endif
#ifndef blt_table_tags_are_shared
#define blt_table_tags_are_shared \
	(bltTclProcsPtr->blt_table_tags_are_shared) /* 223 */
#endif
#ifndef blt_table_clear_row_tags
#define blt_table_clear_row_tags \
	(bltTclProcsPtr->blt_table_clear_row_tags) /* 224 */
#endif
#ifndef blt_table_clear_column_tags
#define blt_table_clear_column_tags \
	(bltTclProcsPtr->blt_table_clear_column_tags) /* 225 */
#endif
#ifndef blt_table_get_row_tags
#define blt_table_get_row_tags \
	(bltTclProcsPtr->blt_table_get_row_tags) /* 226 */
#endif
#ifndef blt_table_get_column_tags
#define blt_table_get_column_tags \
	(bltTclProcsPtr->blt_table_get_column_tags) /* 227 */
#endif
#ifndef blt_table_get_tagged_rows
#define blt_table_get_tagged_rows \
	(bltTclProcsPtr->blt_table_get_tagged_rows) /* 228 */
#endif
#ifndef blt_table_get_tagged_columns
#define blt_table_get_tagged_columns \
	(bltTclProcsPtr->blt_table_get_tagged_columns) /* 229 */
#endif
#ifndef blt_table_row_has_tag
#define blt_table_row_has_tag \
	(bltTclProcsPtr->blt_table_row_has_tag) /* 230 */
#endif
#ifndef blt_table_column_has_tag
#define blt_table_column_has_tag \
	(bltTclProcsPtr->blt_table_column_has_tag) /* 231 */
#endif
#ifndef blt_table_forget_row_tag
#define blt_table_forget_row_tag \
	(bltTclProcsPtr->blt_table_forget_row_tag) /* 232 */
#endif
#ifndef blt_table_forget_column_tag
#define blt_table_forget_column_tag \
	(bltTclProcsPtr->blt_table_forget_column_tag) /* 233 */
#endif
#ifndef blt_table_unset_row_tag
#define blt_table_unset_row_tag \
	(bltTclProcsPtr->blt_table_unset_row_tag) /* 234 */
#endif
#ifndef blt_table_unset_column_tag
#define blt_table_unset_column_tag \
	(bltTclProcsPtr->blt_table_unset_column_tag) /* 235 */
#endif
#ifndef blt_table_first_column
#define blt_table_first_column \
	(bltTclProcsPtr->blt_table_first_column) /* 236 */
#endif
#ifndef blt_table_last_column
#define blt_table_last_column \
	(bltTclProcsPtr->blt_table_last_column) /* 237 */
#endif
#ifndef blt_table_next_column
#define blt_table_next_column \
	(bltTclProcsPtr->blt_table_next_column) /* 238 */
#endif
#ifndef blt_table_previous_column
#define blt_table_previous_column \
	(bltTclProcsPtr->blt_table_previous_column) /* 239 */
#endif
#ifndef blt_table_first_row
#define blt_table_first_row \
	(bltTclProcsPtr->blt_table_first_row) /* 240 */
#endif
#ifndef blt_table_last_row
#define blt_table_last_row \
	(bltTclProcsPtr->blt_table_last_row) /* 241 */
#endif
#ifndef blt_table_next_row
#define blt_table_next_row \
	(bltTclProcsPtr->blt_table_next_row) /* 242 */
#endif
#ifndef blt_table_previous_row
#define blt_table_previous_row \
	(bltTclProcsPtr->blt_table_previous_row) /* 243 */
#endif
#ifndef blt_table_row_spec
#define blt_table_row_spec \
	(bltTclProcsPtr->blt_table_row_spec) /* 244 */
#endif
#ifndef blt_table_column_spec
#define blt_table_column_spec \
	(bltTclProcsPtr->blt_table_column_spec) /* 245 */
#endif
#ifndef blt_table_iterate_rows
#define blt_table_iterate_rows \
	(bltTclProcsPtr->blt_table_iterate_rows) /* 246 */
#endif
#ifndef blt_table_iterate_columns
#define blt_table_iterate_columns \
	(bltTclProcsPtr->blt_table_iterate_columns) /* 247 */
#endif
#ifndef blt_table_iterate_rows_objv
#define blt_table_iterate_rows_objv \
	(bltTclProcsPtr->blt_table_iterate_rows_objv) /* 248 */
#endif
#ifndef blt_table_iterate_columns_objv
#define blt_table_iterate_columns_objv \
	(bltTclProcsPtr->blt_table_iterate_columns_objv) /* 249 */
#endif
#ifndef blt_table_free_iterator_objv
#define blt_table_free_iterator_objv \
	(bltTclProcsPtr->blt_table_free_iterator_objv) /* 250 */
#endif
#ifndef blt_table_iterate_all_rows
#define blt_table_iterate_all_rows \
	(bltTclProcsPtr->blt_table_iterate_all_rows) /* 251 */
#endif
#ifndef blt_table_iterate_all_columns
#define blt_table_iterate_all_columns \
	(bltTclProcsPtr->blt_table_iterate_all_columns) /* 252 */
#endif
#ifndef blt_table_first_tagged_row
#define blt_table_first_tagged_row \
	(bltTclProcsPtr->blt_table_first_tagged_row) /* 253 */
#endif
#ifndef blt_table_first_tagged_column
#define blt_table_first_tagged_column \
	(bltTclProcsPtr->blt_table_first_tagged_column) /* 254 */
#endif
#ifndef blt_table_next_tagged_row
#define blt_table_next_tagged_row \
	(bltTclProcsPtr->blt_table_next_tagged_row) /* 255 */
#endif
#ifndef blt_table_next_tagged_column
#define blt_table_next_tagged_column \
	(bltTclProcsPtr->blt_table_next_tagged_column) /* 256 */
#endif
#ifndef blt_table_list_rows
#define blt_table_list_rows \
	(bltTclProcsPtr->blt_table_list_rows) /* 257 */
#endif
#ifndef blt_table_list_columns
#define blt_table_list_columns \
	(bltTclProcsPtr->blt_table_list_columns) /* 258 */
#endif
#ifndef blt_table_clear_row_traces
#define blt_table_clear_row_traces \
	(bltTclProcsPtr->blt_table_clear_row_traces) /* 259 */
#endif
#ifndef blt_table_clear_column_traces
#define blt_table_clear_column_traces \
	(bltTclProcsPtr->blt_table_clear_column_traces) /* 260 */
#endif
#ifndef blt_table_create_trace
#define blt_table_create_trace \
	(bltTclProcsPtr->blt_table_create_trace) /* 261 */
#endif
#ifndef blt_table_trace_column
#define blt_table_trace_column \
	(bltTclProcsPtr->blt_table_trace_column) /* 262 */
#endif
#ifndef blt_table_trace_row
#define blt_table_trace_row \
	(bltTclProcsPtr->blt_table_trace_row) /* 263 */
#endif
#ifndef blt_table_create_column_trace
#define blt_table_create_column_trace \
	(bltTclProcsPtr->blt_table_create_column_trace) /* 264 */
#endif
#ifndef blt_table_create_column_tag_trace
#define blt_table_create_column_tag_trace \
	(bltTclProcsPtr->blt_table_create_column_tag_trace) /* 265 */
#endif
#ifndef blt_table_create_row_trace
#define blt_table_create_row_trace \
	(bltTclProcsPtr->blt_table_create_row_trace) /* 266 */
#endif
#ifndef blt_table_create_row_tag_trace
#define blt_table_create_row_tag_trace \
	(bltTclProcsPtr->blt_table_create_row_tag_trace) /* 267 */
#endif
#ifndef blt_table_delete_trace
#define blt_table_delete_trace \
	(bltTclProcsPtr->blt_table_delete_trace) /* 268 */
#endif
#ifndef blt_table_create_notifier
#define blt_table_create_notifier \
	(bltTclProcsPtr->blt_table_create_notifier) /* 269 */
#endif
#ifndef blt_table_create_row_notifier
#define blt_table_create_row_notifier \
	(bltTclProcsPtr->blt_table_create_row_notifier) /* 270 */
#endif
#ifndef blt_table_create_row_tag_notifier
#define blt_table_create_row_tag_notifier \
	(bltTclProcsPtr->blt_table_create_row_tag_notifier) /* 271 */
#endif
#ifndef blt_table_create_column_notifier
#define blt_table_create_column_notifier \
	(bltTclProcsPtr->blt_table_create_column_notifier) /* 272 */
#endif
#ifndef blt_table_create_column_tag_notifier
#define blt_table_create_column_tag_notifier \
	(bltTclProcsPtr->blt_table_create_column_tag_notifier) /* 273 */
#endif
#ifndef blt_table_delete_notifier
#define blt_table_delete_notifier \
	(bltTclProcsPtr->blt_table_delete_notifier) /* 274 */
#endif
#ifndef blt_table_sort_init
#define blt_table_sort_init \
	(bltTclProcsPtr->blt_table_sort_init) /* 275 */
#endif
#ifndef blt_table_sort_rows
#define blt_table_sort_rows \
	(bltTclProcsPtr->blt_table_sort_rows) /* 276 */
#endif
#ifndef blt_table_sort_row_map
#define blt_table_sort_row_map \
	(bltTclProcsPtr->blt_table_sort_row_map) /* 277 */
#endif
#ifndef blt_table_sort_finish
#define blt_table_sort_finish \
	(bltTclProcsPtr->blt_table_sort_finish) /* 278 */
#endif
#ifndef blt_table_get_compare_proc
#define blt_table_get_compare_proc \
	(bltTclProcsPtr->blt_table_get_compare_proc) /* 279 */
#endif
#ifndef blt_table_get_row_map
#define blt_table_get_row_map \
	(bltTclProcsPtr->blt_table_get_row_map) /* 280 */
#endif
#ifndef blt_table_get_column_map
#define blt_table_get_column_map \
	(bltTclProcsPtr->blt_table_get_column_map) /* 281 */
#endif
#ifndef blt_table_set_row_map
#define blt_table_set_row_map \
	(bltTclProcsPtr->blt_table_set_row_map) /* 282 */
#endif
#ifndef blt_table_set_column_map
#define blt_table_set_column_map \
	(bltTclProcsPtr->blt_table_set_column_map) /* 283 */
#endif
#ifndef blt_table_restore
#define blt_table_restore \
	(bltTclProcsPtr->blt_table_restore) /* 284 */
#endif
#ifndef blt_table_file_restore
#define blt_table_file_restore \
	(bltTclProcsPtr->blt_table_file_restore) /* 285 */
#endif
#ifndef blt_table_register_format
#define blt_table_register_format \
	(bltTclProcsPtr->blt_table_register_format) /* 286 */
#endif
#ifndef blt_table_unset_keys
#define blt_table_unset_keys \
	(bltTclProcsPtr->blt_table_unset_keys) /* 287 */
#endif
#ifndef blt_table_get_keys
#define blt_table_get_keys \
	(bltTclProcsPtr->blt_table_get_keys) /* 288 */
#endif
#ifndef blt_table_set_keys
#define blt_table_set_keys \
	(bltTclProcsPtr->blt_table_set_keys) /* 289 */
#endif
#ifndef blt_table_key_lookup
#define blt_table_key_lookup \
	(bltTclProcsPtr->blt_table_key_lookup) /* 290 */
#endif
#ifndef blt_table_get_column_limits
#define blt_table_get_column_limits \
	(bltTclProcsPtr->blt_table_get_column_limits) /* 291 */
#endif
#ifndef blt_table_row
#define blt_table_row \
	(bltTclProcsPtr->blt_table_row) /* 292 */
#endif
#ifndef blt_table_column
#define blt_table_column \
	(bltTclProcsPtr->blt_table_column) /* 293 */
#endif
#ifndef blt_table_row_index
#define blt_table_row_index \
	(bltTclProcsPtr->blt_table_row_index) /* 294 */
#endif
#ifndef blt_table_column_index
#define blt_table_column_index \
	(bltTclProcsPtr->blt_table_column_index) /* 295 */
#endif
#ifndef Blt_VecMin
#define Blt_VecMin \
	(bltTclProcsPtr->blt_VecMin) /* 296 */
#endif
#ifndef Blt_VecMax
#define Blt_VecMax \
	(bltTclProcsPtr->blt_VecMax) /* 297 */
#endif
#ifndef Blt_GetVectorToken
#define Blt_GetVectorToken \
	(bltTclProcsPtr->blt_GetVectorToken) /* 298 */
#endif
#ifndef Blt_SetVectorChangedProc
#define Blt_SetVectorChangedProc \
	(bltTclProcsPtr->blt_SetVectorChangedProc) /* 299 */
#endif
#ifndef Blt_FreeVectorToken
#define Blt_FreeVectorToken \
	(bltTclProcsPtr->blt_FreeVectorToken) /* 300 */
#endif
#ifndef Blt_GetVectorFromToken
#define Blt_GetVectorFromToken \
	(bltTclProcsPtr->blt_GetVectorFromToken) /* 301 */
#endif
#ifndef Blt_NameOfVectorFromToken
#define Blt_NameOfVectorFromToken \
	(bltTclProcsPtr->blt_NameOfVectorFromToken) /* 302 */
#endif
#ifndef Blt_NameOfVector
#define Blt_NameOfVector \
	(bltTclProcsPtr->blt_NameOfVector) /* 303 */
#endif
#ifndef Blt_VectorNotifyPending
#define Blt_VectorNotifyPending \
	(bltTclProcsPtr->blt_VectorNotifyPending) /* 304 */
#endif
#ifndef Blt_CreateVector
#define Blt_CreateVector \
	(bltTclProcsPtr->blt_CreateVector) /* 305 */
#endif
#ifndef Blt_CreateVector2
#define Blt_CreateVector2 \
	(bltTclProcsPtr->blt_CreateVector2) /* 306 */
#endif
#ifndef Blt_GetVector
#define Blt_GetVector \
	(bltTclProcsPtr->blt_GetVector) /* 307 */
#endif
#ifndef Blt_GetVectorFromObj
#define Blt_GetVectorFromObj \
	(bltTclProcsPtr->blt_GetVectorFromObj) /* 308 */
#endif
#ifndef Blt_VectorExists
#define Blt_VectorExists \
	(bltTclProcsPtr->blt_VectorExists) /* 309 */
#endif
#ifndef Blt_ResetVector
#define Blt_ResetVector \
	(bltTclProcsPtr->blt_ResetVector) /* 310 */
#endif
#ifndef Blt_ResizeVector
#define Blt_ResizeVector \
	(bltTclProcsPtr->blt_ResizeVector) /* 311 */
#endif
#ifndef Blt_DeleteVectorByName
#define Blt_DeleteVectorByName \
	(bltTclProcsPtr->blt_DeleteVectorByName) /* 312 */
#endif
#ifndef Blt_DeleteVector
#define Blt_DeleteVector \
	(bltTclProcsPtr->blt_DeleteVector) /* 313 */
#endif
#ifndef Blt_ExprVector
#define Blt_ExprVector \
	(bltTclProcsPtr->blt_ExprVector) /* 314 */
#endif
#ifndef Blt_InstallIndexProc
#define Blt_InstallIndexProc \
	(bltTclProcsPtr->blt_InstallIndexProc) /* 315 */
#endif
#ifndef Blt_VectorExists2
#define Blt_VectorExists2 \
	(bltTclProcsPtr->blt_VectorExists2) /* 316 */
#endif

#endif /* defined(USE_BLT_STUBS) && !defined(BUILD_BLT_TCL_PROCS) */

/* !END!: Do not edit above this line. */
#endif /* _BLT_TCLPROCS_H */
