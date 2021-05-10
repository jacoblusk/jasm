#if defined(_WIN32)
	#define _CRT_SECURE_NO_WARNINGS
	#define _CRT_NONSTDC_NO_WARNINGS
#endif

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#else
#include <stdlib.h>
#endif

#include <string.h>

#include "hashmap.h"

size_t
__HashMap_Hash(char const *pszKey) {
	size_t hash = 5381;
	for(; *pszKey; pszKey++) {
		hash = ((hash << 5) + hash) + *pszKey;
	}

	return hash;
}

HASHMAP_STATUS
HashMap_Initialize(
	PHASHMAP pHashMap,
	size_t cInitialCapacity,
	HASHMAP_FREE_ROUTINE pFreeFn,
	void *pFreeFnUserData) {

	pHashMap->cCapacity = cInitialCapacity;
	pHashMap->cSize = 0;
	pHashMap->pFreeFn = pFreeFn;
	pHashMap->pFreeFnUserData = pFreeFnUserData;
	pHashMap->pElements = malloc(
		sizeof(HASHMAP_ELEMENT) * cInitialCapacity
	);

	memset(pHashMap->pElements, 0, sizeof(HASHMAP_ELEMENT) * cInitialCapacity);

	return (pHashMap->pElements != NULL) \
		? HASHMAP_STATUS_OK : HASHMAP_STATUS_ENOMEM;
}

HASHMAP_STATUS
HashMap_Put(PHASHMAP pHashMap, char const *pszKey, void *pValue) {
	size_t hash = __HashMap_Hash(pszKey);
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
				pHashMap->pFreeFn(pElement->pData, pHashMap->pFreeFnUserData);
			}

			pElement->pData = pValue;
			return HASHMAP_STATUS_OK;
		}

		_Bool bMatch = 0;
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
		pNewElement->pNext = NULL;
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

void
HashMap_DataMapFn(PHASHMAP pHashMap, HASHMAP_MAP_ROUTINE MapFn, void *pUserData) {
	for(size_t i = 0; i < pHashMap->cCapacity; i++) {
		PHASHMAP_ELEMENT pElement = pHashMap->pElements + i; 
		if(pElement->pData != NULL) {
			for(PHASHMAP_ELEMENT pCurrent = pElement;
				pCurrent;
				pCurrent = pCurrent->pNext) {
				MapFn(pCurrent->pData, pUserData);
			}
		}
	}
}

void
HashMap_ElementMapFn(PHASHMAP pHashMap, HASHMAP_MAP_ROUTINE MapFn, void *pUserData) {
	for(size_t i = 0; i < pHashMap->cCapacity; i++) {
		PHASHMAP_ELEMENT pElement = pHashMap->pElements + i; 
		if(pElement->pData != NULL) {
			for(PHASHMAP_ELEMENT pCurrent = pElement;
				pCurrent;) {

				PHASHMAP_ELEMENT pTemp = pCurrent->pNext;
				MapFn(pCurrent, pUserData);
				pCurrent = pTemp;
			}
		}
	}
}

void __HashMapDestroy_FreeFn(void *pData, void *pUserData) {
	PHASHMAP_ELEMENT pElement = pData;
	PHASHMAP pHashMap = pUserData;

	if(pHashMap->pFreeFn)
		pHashMap->pFreeFn(pElement->pData, pHashMap->pFreeFnUserData);
	
	free(pElement->pszKey);

	for(size_t i = 0; i < pHashMap->cCapacity; i++) {
		if(&pHashMap->pElements[i] == pElement)
			return;
	}

	free(pElement);
}

void
HashMapDestroy(PHASHMAP pHashMap) {
	HashMap_ElementMapFn(pHashMap, __HashMapDestroy_FreeFn, pHashMap);
	free(pHashMap->pElements);
}

void *
HashMap_Get(PHASHMAP pHashMap, char const *pszKey) {
	size_t hash = __HashMap_Hash(pszKey);
	PHASHMAP_ELEMENT pElement = \
		pHashMap->pElements + (hash % pHashMap->cCapacity);
	PHASHMAP_ELEMENT pCurrent = pElement;

	for(;
		pCurrent && (strcmp(pCurrent->pszKey, pszKey) != 0);
		pCurrent = pCurrent->pNext
	);

	if(pCurrent) {
		return pCurrent->pData;
	}

	return NULL;
}
