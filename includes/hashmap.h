#ifndef HASHMAP_H
#define HASHMAP_H

typedef enum tagHASHMAP_STATUS {
	HASHMAP_STATUS_OK,
	HASHMAP_STATUS_ENOMEM
} HASHMAP_STATUS;

typedef struct tagHASHMAP_ELEMENT HASHMAP_ELEMENT, *PHASHMAP_ELEMENT;

struct tagHASHMAP_ELEMENT {
	char *pszKey;
	void *pData;
	PHASHMAP_ELEMENT pNext;
};

typedef void (*HASHMAP_MAP_ROUTINE)(void *, void *);
typedef HASHMAP_MAP_ROUTINE HASHMAP_FREE_ROUTINE; 

typedef struct tagHASHMAP {
	size_t cCapacity;
	size_t cSize;
	PHASHMAP_ELEMENT pElements;
	HASHMAP_FREE_ROUTINE pFreeFn;
	void *pFreeFnUserData;
} HASHMAP, *PHASHMAP;

size_t __HashMap_Hash(char const *pszKey);

HASHMAP_STATUS HashMap_Initialize(
	PHASHMAP pHashMap,
	size_t cInitialCapacity,
	HASHMAP_FREE_ROUTINE pFreeFn,
	void *pFreeFnUserData
);

HASHMAP_STATUS HashMap_Put(PHASHMAP pHashMap, char const *pszKey, void *pValue);
void *HashMap_Get(PHASHMAP pHashMap, char const *pszKey);
void HashMap_ElementMapFn(PHASHMAP pHashMap, HASHMAP_MAP_ROUTINE MapFn, void *pUserData);
void HashMap_DataMapFn(PHASHMAP pHashMap, HASHMAP_MAP_ROUTINE MapFn, void *pUserData);
void HashMap_Destroy(PHASHMAP pHashMap);
#endif
