#if defined(_WIN32)
	#define _CRT_SECURE_NO_WARNINGS
	#define _CRT_NONSTDC_NO_WARNINGS
#endif

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef enum tagHASHMAP_STATUS {
	HASHMAP_STATUS_OK,
	HASHMAP_STATUS_ENOMEM
} HASHMAP_STATUS;

typedef struct tagHASHMAP_ELEMENT HASHMAP_ELEMENT, *PHASHMAP_ELEMENT;

struct tagHASHMAP_ELEMENT {
	char const *pszKey;
	void *pData;
	PHASHMAP_ELEMENT pNext;
};

typedef void (*HASHMAP_MAP_ROUTINE)(PHASHMAP_ELEMENT, void *);
typedef HASHMAP_MAP_ROUTINE HASHMAP_FREE_ROUTINE; 

typedef struct tagHASHMAP {
	size_t cCapacity;
	size_t cSize;
	PHASHMAP_ELEMENT pElements;
	HASHMAP_FREE_ROUTINE pFreeFn;
	void *pFreeFnUserData;
} HASHMAP, *PHASHMAP;


size_t
__HashMapHash(char const *pszKey) {
	size_t hash = 5381;
	for(; *pszKey; pszKey++) {
		hash = ((hash << 5) + hash) + *pszKey;
	}

	return hash;
}

HASHMAP_STATUS
HashMapInitialize(
	PHASHMAP pHashMap,
	size_t cInitialCapacity,
	HASHMAP_FREE_ROUTINE pFreeFn,
	void *pFreeFnUserData) {

	pHashMap->cCapacity = cInitialCapacity;
	pHashMap->cSize = 0;
	pHashMap->pFreeFn = pFreeFn;
	pHashMap->pFreeFnUserData = pFreeFnUserData;
	pHashMap->pElements = malloc(
		sizeof(HASHMAP_ELEMENT) * pHashMap->cCapacity
	);

	return (pHashMap->pElements != NULL) \
		? HASHMAP_STATUS_OK : HASHMAP_STATUS_ENOMEM;
}

HASHMAP_STATUS
HashMapPut(PHASHMAP pHashMap, char const *pszKey, void *pValue) {
	size_t hash = __HashMapHash(pszKey);
	PHASHMAP_ELEMENT pElement = pHashMap->pElements \
		+ (hash % pHashMap->cCapacity);

	if(!pElement->pData) {
		char const *pszKeyCopy = strdup(pszKey);
		if(!pszKeyCopy) {
			return HASHMAP_STATUS_ENOMEM;			
		}

		pElement->pData = pValue;
		pElement->pszKey = pszKeyCopy;
	} else {
		if(strcmp(pszKey, pElement->pszKey) == 0) {
			if(pHashMap->pFreeFn) {
				pHashMap->pFreeFn(pElement, pHashMap->pFreeFnUserData);
			}

			pElement->pData = pValue;
			return HASHMAP_STATUS_OK;
		}

		bool bMatch = false;
		PHASHMAP_ELEMENT pCurrent = pElement;

		for(;
			pCurrent && (bMatch = strcmp(pCurrent->pszKey, pszKey) != 0);
			pCurrent = pCurrent->pNext
		);

		if(!bMatch) {
			if(pHashMap->pFreeFn) {
				pHashMap->pFreeFn(
					pCurrent->pData,
					pHashMap->pFreeFnUserData
				);
			}

			pCurrent->pData = pValue;
			return HASHMAP_STATUS_OK;
		}
		
		PHASHMAP_ELEMENT pNewElement = malloc(sizeof(HASHMAP_ELEMENT));
		if(!pNewElement) {
			return HASHMAP_STATUS_ENOMEM;
		}

		pNewElement->pszKey = strdup(pszKey);
		if(!pNewElement->pszKey) {
			free(pNewElement);
			return HASHMAP_STATUS_ENOMEM;
		}

		pNewElement->pData = pValue;
		if(pElement->pNext)
			pNewElement->pNext = pElement->pNext;

		pElement->pNext = pNewElement;
	}

	return HASHMAP_STATUS_OK;
}

void *
HashMapGet(PHASHMAP pHashMap, char const *pszKey) {
	size_t hash = __HashMapHash(pszKey);
	PHASHMAP_ELEMENT pElement = \
		pHashMap->pElements + (hash % pHashMap->cCapacity);
	PHASHMAP_ELEMENT pCurrent = pElement;

	for(;
		pCurrent && (bMatch = strcmp(pCurrent->pszKey, pszKey) != 0);
		pCurrent = pCurrent->pNext
	);

	if(pCurrent) {
		return pElement->pData;
	}

	return NULL;
}
