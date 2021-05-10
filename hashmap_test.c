#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#else
#include <stdlib.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "hashmap.h"
#include "test.h"

_Bool TestSinglePutGet(void *pUserData) {
	HASHMAP hashMap;
	HASHMAP_STATUS hmStatus;

	hmStatus = HashMapInitialize(&hashMap, 10, NULL, NULL);
	TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMapInitialize failed.");

	hmStatus = HashMapPut(&hashMap, "foo", "bar");
	TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMapPut failed.");

	char *pszResult = HashMapGet(&hashMap, "foo");
	TEST_ASSERT(strcmp(pszResult, "bar") == 0, "HashMapGet failed.");

	HashMapDestroy(&hashMap);
	return TRUE;
}

_Bool TestMultiplePutGet(void *pUserData) {
	HASHMAP hashMap;
	HASHMAP_STATUS hmStatus;

	hmStatus = HashMapInitialize(&hashMap, 10, NULL, NULL);
	TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMapInitialize failed.");

	hmStatus = HashMapPut(&hashMap, "foo1", "bar1");
	TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMapPut1 failed.");

	hmStatus = HashMapPut(&hashMap, "foo2", "bar2");
	TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMapPut2 failed.");

	hmStatus = HashMapPut(&hashMap, "foo3", "bar3");
	TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMapPut3 failed.");

	char *pszResult;

	pszResult = HashMapGet(&hashMap, "foo1");
	TEST_ASSERT(strcmp(pszResult, "bar1") == 0, "HashMapGet1 failed.");

	pszResult = HashMapGet(&hashMap, "foo2");
	TEST_ASSERT(strcmp(pszResult, "bar2") == 0, "HashMapGet2 failed.");

	pszResult = HashMapGet(&hashMap, "foo3");
	TEST_ASSERT(strcmp(pszResult, "bar3") == 0, "HashMapGet3 failed.");

	HashMapDestroy(&hashMap);
	return TRUE;
}

_Bool TestOverwriteKey(void *pUserData) {

	HASHMAP hashMap;
	HASHMAP_STATUS hmStatus;

	hmStatus = HashMapInitialize(&hashMap, 10, NULL, NULL);
	TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMapInitialize failed.");

	hmStatus = HashMapPut(&hashMap, "foo", "bar");
	TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMapPut1 failed.");

	hmStatus = HashMapPut(&hashMap, "foo", "bar1");
	TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMapPut2 failed.");

	char *pszResult = HashMapGet(&hashMap, "foo");
	TEST_ASSERT(strcmp(pszResult, "bar1") == 0, "HashMapGet failed.");

	HashMapDestroy(&hashMap);
	return TRUE;
}

void __TestFreeFunction_Fn(void *pData, void *pUserData) {
	int *i = pData;
	*i = 0;
}

_Bool TestFreeFunction(void *pUserData) {

	HASHMAP hashMap;
	HASHMAP_STATUS hmStatus;

	hmStatus = HashMapInitialize(&hashMap, 10, __TestFreeFunction_Fn, NULL);
	TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMapInitialize failed.");

	int iTest1 = 1;
	int iTest2 = 2;

	hmStatus = HashMapPut(&hashMap, "foo", &iTest1);
	TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMapPut1 failed.");

	hmStatus = HashMapPut(&hashMap, "foo", &iTest2);
	TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMapPut2 failed.");

	int *iResult = HashMapGet(&hashMap, "foo");
	TEST_ASSERT(iResult == &iTest2, "HashMapGet failed.");

	TEST_ASSERT(iTest1 == 0, "Free function not called.");

	HashMapDestroy(&hashMap);
	return TRUE;
}

void __TestUserData_FreeFn(void *pData, void *pUserData) {
	*(int *)pUserData = 2;
}

_Bool TestUserData_FreeFn(void *pUserData) {

	HASHMAP hashMap;
	HASHMAP_STATUS hmStatus;

	int iUserData = 1;
	int iTest1 = 1;
	int iTest2 = 2;
	
	hmStatus = HashMapInitialize(&hashMap, 10, __TestUserData_FreeFn, &iUserData);
	TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMapInitialize failed.");

	hmStatus = HashMapPut(&hashMap, "foo", &iTest1);
	TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMapPut1 failed.");

	hmStatus = HashMapPut(&hashMap, "foo", &iTest2);
	TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMapPut2 failed.");

	int *iResult = HashMapGet(&hashMap, "foo");
	TEST_ASSERT(iResult == &iTest2, "HashMapGet failed.");

	TEST_ASSERT(iUserData == 2, "User data not passed to FreeFn.");

	HashMapDestroy(&hashMap);
	return TRUE;
}

int __TestDataMapFn_PartialSum(int n) {
	return n * (n + 1) / 2;
}

void __TestDataMapFn_Sum(void *pData, void *pUserData) {
	int *piSum = pUserData;

	*piSum += (uintptr_t)pData;
}

_Bool TestDataMapFn(void *pUserData) {
	HASHMAP hashMap;
	HASHMAP_STATUS hmStatus;

	uintptr_t iMaxVal = 20;
	
	hmStatus = HashMapInitialize(&hashMap, 10, NULL, NULL);
	TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMapInitialize failed.");

	for(intptr_t i = 1; i <= iMaxVal; i++) {
		char pszKey[20];

		sprintf(pszKey, "foo%lld", i);
		hmStatus = HashMapPut(&hashMap, pszKey, (void *)i);
		TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMapPut failed (%lld).", i);
	}

	int iSum = 0;
	HashMapDataMapFn(&hashMap, __TestDataMapFn_Sum, &iSum);
	int iExpectedSum = __TestDataMapFn_PartialSum(iMaxVal);

	TEST_ASSERT(iSum == iExpectedSum, "HashMapDataMapFn failed. (%d != %d)", iSum, iExpectedSum);

	HashMapDestroy(&hashMap);
	return TRUE;
}

_Bool TestManyPutGet(void *pUserData) {

	HASHMAP hashMap;
	HASHMAP_STATUS hmStatus;
	
	hmStatus = HashMapInitialize(&hashMap, 10, NULL, NULL);
	TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMapInitialize failed.");

	for(intptr_t i = 1; i < 20; i++) {
		char pszKey[20];

		sprintf(pszKey, "foo%lld", i);
		hmStatus = HashMapPut(&hashMap, pszKey, (void *)i);
		TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMapPut failed (%lld).", i);
	}

	for(intptr_t i = 1; i < 20; i++) {
		char pszKey[20] = { 0 };

		sprintf(pszKey, "foo%lld", i);
		uintptr_t iValue = (uintptr_t)HashMapGet(&hashMap, pszKey);
		TEST_ASSERT(iValue == i, "HashMapGet failed ((iValue)%lld != (i)%lld).", iValue, i);
	}

	HashMapDestroy(&hashMap);
	return TRUE;
}

int main(int argc, char **argv) {
	TESTSUITE testSuite;

	TestSuiteInitialize(&testSuite);
	{
		TestSuiteAddTestCase(&testSuite, "HashMap Put/Get", TestSinglePutGet, NULL); 
		TestSuiteAddTestCase(&testSuite, "HashMap Multiple Put/Get", TestMultiplePutGet, NULL); 
		TestSuiteAddTestCase(&testSuite, "HashMap Overwrite", TestOverwriteKey, NULL); 
		TestSuiteAddTestCase(&testSuite, "HashMap Free Function", TestFreeFunction, NULL); 
		TestSuiteAddTestCase(&testSuite, "HashMap Free Function (User Data)", TestUserData_FreeFn, NULL); 
		TestSuiteAddTestCase(&testSuite, "HashMap N > Capacity Put/Get", TestManyPutGet, NULL); 
		TestSuiteAddTestCase(&testSuite, "HashMap Map Data Function", TestDataMapFn, NULL); 
	}

	TestSuiteRun(&testSuite);
	TestSuiteDestroy(&testSuite);

	if(_CrtDumpMemoryLeaks()) {
		printf("Memory leak found, check DbgView64 for more information.\n");
	}
	return 0;
}
