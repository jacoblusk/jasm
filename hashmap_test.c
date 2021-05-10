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

_Bool Test_SinglePutGet(void *pUserData) {
	HASHMAP hashMap;
	HASHMAP_STATUS hmStatus;

	hmStatus = HashMap_Initialize(&hashMap, 10, NULL, NULL);
	TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Initialize failed.");

	hmStatus = HashMap_Put(&hashMap, "foo", "bar");
	TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Put failed.");

	char *pszResult = HashMap_Get(&hashMap, "foo");
	TEST_ASSERT(strcmp(pszResult, "bar") == 0, "HashMap_Get failed.");

	HashMapDestroy(&hashMap);
	return TRUE;
}

_Bool Test_MultiplePutGet(void *pUserData) {
	HASHMAP hashMap;
	HASHMAP_STATUS hmStatus;

	hmStatus = HashMap_Initialize(&hashMap, 10, NULL, NULL);
	TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Initialize failed.");

	hmStatus = HashMap_Put(&hashMap, "foo1", "bar1");
	TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Put1 failed.");

	hmStatus = HashMap_Put(&hashMap, "foo2", "bar2");
	TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Put2 failed.");

	hmStatus = HashMap_Put(&hashMap, "foo3", "bar3");
	TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Put3 failed.");

	char *pszResult;

	pszResult = HashMap_Get(&hashMap, "foo1");
	TEST_ASSERT(strcmp(pszResult, "bar1") == 0, "HashMap_Get1 failed.");

	pszResult = HashMap_Get(&hashMap, "foo2");
	TEST_ASSERT(strcmp(pszResult, "bar2") == 0, "HashMap_Get2 failed.");

	pszResult = HashMap_Get(&hashMap, "foo3");
	TEST_ASSERT(strcmp(pszResult, "bar3") == 0, "HashMap_Get3 failed.");

	HashMapDestroy(&hashMap);
	return TRUE;
}

_Bool Test_OverwriteKey(void *pUserData) {

	HASHMAP hashMap;
	HASHMAP_STATUS hmStatus;

	hmStatus = HashMap_Initialize(&hashMap, 10, NULL, NULL);
	TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Initialize failed.");

	hmStatus = HashMap_Put(&hashMap, "foo", "bar");
	TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Put1 failed.");

	hmStatus = HashMap_Put(&hashMap, "foo", "bar1");
	TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Put2 failed.");

	char *pszResult = HashMap_Get(&hashMap, "foo");
	TEST_ASSERT(strcmp(pszResult, "bar1") == 0, "HashMap_Get failed.");

	HashMapDestroy(&hashMap);
	return TRUE;
}

void __Test_FreeFunction_Fn(void *pData, void *pUserData) {
	int *i = pData;
	*i = 0;
}

_Bool Test_FreeFunction(void *pUserData) {

	HASHMAP hashMap;
	HASHMAP_STATUS hmStatus;

	hmStatus = HashMap_Initialize(&hashMap, 10, __Test_FreeFunction_Fn, NULL);
	TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Initialize failed.");

	int iTest1 = 1;
	int iTest2 = 2;

	hmStatus = HashMap_Put(&hashMap, "foo", &iTest1);
	TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Put1 failed.");

	hmStatus = HashMap_Put(&hashMap, "foo", &iTest2);
	TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Put2 failed.");

	int *iResult = HashMap_Get(&hashMap, "foo");
	TEST_ASSERT(iResult == &iTest2, "HashMap_Get failed.");

	TEST_ASSERT(iTest1 == 0, "Free function not called.");

	HashMapDestroy(&hashMap);
	return TRUE;
}

void __Test_UserData_FreeFn(void *pData, void *pUserData) {
	*(int *)pUserData = 2;
}

_Bool Test_UserData_FreeFn(void *pUserData) {

	HASHMAP hashMap;
	HASHMAP_STATUS hmStatus;

	int iUserData = 1;
	int iTest1 = 1;
	int iTest2 = 2;
	
	hmStatus = HashMap_Initialize(&hashMap, 10, __Test_UserData_FreeFn, &iUserData);
	TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Initialize failed.");

	hmStatus = HashMap_Put(&hashMap, "foo", &iTest1);
	TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Put1 failed.");

	hmStatus = HashMap_Put(&hashMap, "foo", &iTest2);
	TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Put2 failed.");

	int *iResult = HashMap_Get(&hashMap, "foo");
	TEST_ASSERT(iResult == &iTest2, "HashMap_Get failed.");

	TEST_ASSERT(iUserData == 2, "User data not passed to FreeFn.");

	HashMapDestroy(&hashMap);
	return TRUE;
}

int __Test_DataMapFn_PartialSum(int n) {
	return n * (n + 1) / 2;
}

void __Test_DataMapFn_Sum(void *pData, void *pUserData) {
	int *piSum = pUserData;

	*piSum += (uintptr_t)pData;
}

_Bool Test_DataMapFn(void *pUserData) {
	HASHMAP hashMap;
	HASHMAP_STATUS hmStatus;

	uintptr_t iMaxVal = 20;
	
	hmStatus = HashMap_Initialize(&hashMap, 10, NULL, NULL);
	TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Initialize failed.");

	for(intptr_t i = 1; i <= iMaxVal; i++) {
		char pszKey[20];

		sprintf(pszKey, "foo%lld", i);
		hmStatus = HashMap_Put(&hashMap, pszKey, (void *)i);
		TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Put failed (%lld).", i);
	}

	int iSum = 0;
	HashMap_DataMapFn(&hashMap, __Test_DataMapFn_Sum, &iSum);
	int iExpectedSum = __Test_DataMapFn_PartialSum(iMaxVal);

	TEST_ASSERT(iSum == iExpectedSum, "HashMap_DataMapFn failed. (%d != %d)", iSum, iExpectedSum);

	HashMapDestroy(&hashMap);
	return TRUE;
}

_Bool Test_ManyPutGet(void *pUserData) {

	HASHMAP hashMap;
	HASHMAP_STATUS hmStatus;
	
	hmStatus = HashMap_Initialize(&hashMap, 10, NULL, NULL);
	TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Initialize failed.");

	for(intptr_t i = 1; i < 20; i++) {
		char pszKey[20];

		sprintf(pszKey, "foo%lld", i);
		hmStatus = HashMap_Put(&hashMap, pszKey, (void *)i);
		TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Put failed (%lld).", i);
	}

	for(intptr_t i = 1; i < 20; i++) {
		char pszKey[20] = { 0 };

		sprintf(pszKey, "foo%lld", i);
		uintptr_t iValue = (uintptr_t)HashMap_Get(&hashMap, pszKey);
		TEST_ASSERT(iValue == i, "HashMap_Get failed ((iValue)%lld != (i)%lld).", iValue, i);
	}

	HashMapDestroy(&hashMap);
	return TRUE;
}

int main(int argc, char **argv) {
	TESTSUITE testSuite;

	TestSuite_Initialize(&testSuite);
	{
		TestSuite_AddTestCase(&testSuite, "HashMap Put/Get", Test_SinglePutGet, NULL); 
		TestSuite_AddTestCase(&testSuite, "HashMap Multiple Put/Get", Test_MultiplePutGet, NULL); 
		TestSuite_AddTestCase(&testSuite, "HashMap Overwrite", Test_OverwriteKey, NULL); 
		TestSuite_AddTestCase(&testSuite, "HashMap Free Function", Test_FreeFunction, NULL); 
		TestSuite_AddTestCase(&testSuite, "HashMap Free Function (User Data)", Test_UserData_FreeFn, NULL); 
		TestSuite_AddTestCase(&testSuite, "HashMap N > Capacity Put/Get", Test_ManyPutGet, NULL); 
		TestSuite_AddTestCase(&testSuite, "HashMap Map Data Function", Test_DataMapFn, NULL); 
	}

	TestSuite_Run(&testSuite);
	TestSuite_Destroy(&testSuite);

	if(_CrtDumpMemoryLeaks()) {
		printf("Memory leak found, check DbgView64 for more information.\n");
	}
	return 0;
}
