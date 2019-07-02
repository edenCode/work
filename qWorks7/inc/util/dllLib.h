#ifndef _DLL_LIB_H
#define _DLL_LIB_H

#ifdef __cplusplus
extern "C" {
#endif

/* Node of a linked list */
typedef struct dlnode 
{
    struct dlnode *next; /* Points at the next node in the list */
    struct dlnode *prev; /* Points at the previous node in the list */
} DL_NODE;

/* Header for a linked list */
typedef struct dl_list 
{
    DL_NODE *head;	/* header of list */
    DL_NODE *tail;	/* tail of list */
} DL_LIST;

/* Linked list macros */
#define DLL_FIRST(pList) (((DL_LIST *)(pList))->head)
#define DLL_LAST(pList)	 (((DL_LIST *)(pList))->tail)
#define DLL_NEXT(pNode)	 (((DL_NODE *)(pNode))->next)
#define DLL_PREV(pNode)	 (((DL_NODE *)(pNode))->prev)
#define DLL_EMPTY(pList) (((DL_LIST *)pList)->head == NULL)
#define DLL_INIT(pList) \
do {                    \
	((DL_LIST *)(pList))->head = NULL; \
	((DL_LIST *)(pList))->tail = NULL; \
}while(0)

/* function declarations */

DL_NODE* dllGet( DL_LIST *pList );
void dllAdd( DL_LIST *pList, DL_NODE *pNode );
void dllInsert( DL_LIST *pList, DL_NODE *pPrev, DL_NODE *pNode );
void dllRemove( DL_LIST *pList, DL_NODE *pNode );
int  dllCount( DL_LIST *pList );
DL_NODE* dllEach( DL_LIST *pList, int(*routine)(DL_NODE*,void*), void* arg );

#ifdef __cplusplus
}
#endif

#endif 

/* End of file */

