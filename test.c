#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#else
#include <stdlib.h>
#endif

#include <stdio.h>
#include <string.h>

#include "test.h"

void
TestSuite_Initialize(PTESTSUITE pTestSuite) {
	pTestSuite->pTestCases = NULL;
	pTestSuite->pLastTestCase = NULL;
	pTestSuite->cTests = 0;
}

void
TestSuite_Destroy(PTESTSUITE pTestSuite) {
	for(PTESTCASE pCurrentTestCase = pTestSuite->pTestCases;
		pCurrentTestCase;) {
		
		PTESTCASE pTemp = pCurrentTestCase->pNext;
		free(pCurrentTestCase);
		pCurrentTestCase = pTemp;
	}
}

_Bool
TestSuite_AddTestCase(
	PTESTSUITE pTestSuite,
	char const *pszName,
	TEST_ROUTINE pTestFn,
	void *pUserData) {
	
	PTESTCASE pTestCase = malloc(sizeof(TESTCASE));
	if(!pTestCase) {
		return FALSE;
	}

	pTestSuite->cTests++;

	pTestCase->pTestFn = pTestFn;
	pTestCase->pUserData = pUserData;
	pTestCase->pszName = pszName;
	pTestCase->pNext = NULL;

	if(!pTestSuite->pTestCases) {
		pTestSuite->pTestCases = pTestCase;
		pTestSuite->pLastTestCase = pTestCase;
		return TRUE;
	}

	pTestSuite->pLastTestCase->pNext = pTestCase;
	pTestSuite->pLastTestCase = pTestCase;

	return TRUE;
}

void
TestSuite_Run(PTESTSUITE pTestSuite) {
	size_t cSuccess = 0;
	size_t i = 1;
	int iMaxWidth = 0;

	for(PTESTCASE pCurrentTestCase = pTestSuite->pTestCases;
		pCurrentTestCase;
		pCurrentTestCase = pCurrentTestCase->pNext) {

		int iWidth = strlen(pCurrentTestCase->pszName);
		if(iMaxWidth < iWidth) {
			iMaxWidth = iWidth;
		}
	}

	for(PTESTCASE pCurrentTestCase = pTestSuite->pTestCases;
		pCurrentTestCase;
		pCurrentTestCase = pCurrentTestCase->pNext, i++) {

		printf("%zu. Test: %-*s", i, iMaxWidth, pCurrentTestCase->pszName);
		_Bool bResult = pCurrentTestCase->pTestFn(pCurrentTestCase->pUserData);

		if(bResult) {
			puts(" ... passed");
			cSuccess++;
		}
	}

	printf("\nSUCCEEDED: %zu, FAILED: %zu\n", cSuccess, pTestSuite->cTests - cSuccess); 
}

