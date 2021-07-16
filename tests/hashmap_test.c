#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdlib.h>
#else
#include <stdlib.h>
#endif

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "hashmap.h"
#include "test.h"
#include "utility.h"

_Bool Test_SinglePutGet(void *pUserData) {
    UNUSED(pUserData);

    HASHMAP hashMap;
    HASHMAP_STATUS hmStatus;

    hmStatus = HashMap_Initialize(&hashMap, 10, NULL, NULL);
    TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Initialize failed.");

    hmStatus = HashMap_Put(&hashMap, "foo", "bar");
    TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Put failed.");

    void *pszResult;

    hmStatus = HashMap_Get(&hashMap, "foo", &pszResult);
    TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Get failed.");
    TEST_ASSERT(strcmp((char *)pszResult, "bar") == 0,
                "HashMap_Get wrong value.");

    HashMap_Destroy(&hashMap);
    return TRUE;
}

_Bool Test_MultiplePutGet(void *pUserData) {
    UNUSED(pUserData);

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

    void *pszResult;

    hmStatus = HashMap_Get(&hashMap, "foo1", &pszResult);
    TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Get1 failed.");
    TEST_ASSERT(strcmp(pszResult, "bar1") == 0, "HashMap_Get1 failed.");

    hmStatus = HashMap_Get(&hashMap, "foo2", &pszResult);
    TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Get2 failed.");
    TEST_ASSERT(strcmp(pszResult, "bar2") == 0, "HashMap_Get2 failed.");

    hmStatus = HashMap_Get(&hashMap, "foo3", &pszResult);
    TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Get2 failed.");
    TEST_ASSERT(strcmp(pszResult, "bar3") == 0, "HashMap_Get3 failed.");

    HashMap_Destroy(&hashMap);
    return TRUE;
}

_Bool Test_OverwriteKey(void *pUserData) {
    UNUSED(pUserData);

    HASHMAP hashMap;
    HASHMAP_STATUS hmStatus;

    hmStatus = HashMap_Initialize(&hashMap, 10, NULL, NULL);
    TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Initialize failed.");

    hmStatus = HashMap_Put(&hashMap, "foo", "bar");
    TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Put1 failed.");

    hmStatus = HashMap_Put(&hashMap, "foo", "bar1");
    TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Put2 failed.");

    void *pszResult;

    hmStatus = HashMap_Get(&hashMap, "foo", &pszResult);
    TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Get failed.");
    TEST_ASSERT(strcmp(pszResult, "bar1") == 0, "HashMap_Get failed.");

    HashMap_Destroy(&hashMap);
    return TRUE;
}

void __Test_FreeFunction_Fn(void *pData, void *pUserData) {
    UNUSED(pUserData);

    int *i = pData;
    *i = 0;
}

_Bool Test_FreeFunction(void *pUserData) {
    UNUSED(pUserData);

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

    void *piResult;

    hmStatus = HashMap_Get(&hashMap, "foo", &piResult);
    TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Get failed.");
    TEST_ASSERT(piResult == &iTest2, "HashMap_Get failed.");

    TEST_ASSERT(iTest1 == 0, "Free function not called.");

    HashMap_Destroy(&hashMap);
    return TRUE;
}

void __Test_UserData_FreeFn(void *pData, void *pUserData) {
    UNUSED(pData);

    *(int *)pUserData = 2;
}

_Bool Test_UserData_FreeFn(void *pUserData) {

    HASHMAP hashMap;
    HASHMAP_STATUS hmStatus;

    int iUserData = 1;
    int iTest1 = 1;
    int iTest2 = 2;

    hmStatus =
        HashMap_Initialize(&hashMap, 10, __Test_UserData_FreeFn, &iUserData);
    TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Initialize failed.");

    hmStatus = HashMap_Put(&hashMap, "foo", &iTest1);
    TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Put1 failed.");

    hmStatus = HashMap_Put(&hashMap, "foo", &iTest2);
    TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Put2 failed.");

    void *piResult;

    hmStatus = HashMap_Get(&hashMap, "foo", &piResult);
    TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Get failed.");
    TEST_ASSERT(piResult == &iTest2, "HashMap_Get failed.");

    TEST_ASSERT(iUserData == 2, "User data not passed to FreeFn.");

    HashMap_Destroy(&hashMap);
    return TRUE;
}

int __Test_DataMapFn_PartialSum(int n) { return n * (n + 1) / 2; }

void __Test_DataMapFn_Sum(void *pData, void *pUserData) {
    int *piSum = pUserData;

    *piSum += *(int *)pData;
}

_Bool Test_DataMapFn(void *pUserData) {
    UNUSED(pUserData);

    HASHMAP hashMap;
    HASHMAP_STATUS hmStatus;

    int iMaxVal = 20;

    hmStatus = HashMap_Initialize(&hashMap, 10, NULL, NULL);
    TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Initialize failed.");

    int *pValues = malloc(sizeof(int) * iMaxVal);
    TEST_ASSERT(pValues != NULL, "malloc failed");

    for (int i = 0; i < iMaxVal; i++) {
        char pszKey[20];

        sprintf(pszKey, "foo%d", i);
        pValues[i] = i;
        hmStatus = HashMap_Put(&hashMap, pszKey, &pValues[i]);
        TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Put failed (%d).",
                    i);
    }

    int iSum = 0;
    HashMap_DataMapFn(&hashMap, __Test_DataMapFn_Sum, &iSum);
    int iExpectedSum = __Test_DataMapFn_PartialSum(iMaxVal - 1);

    TEST_ASSERT(iSum == iExpectedSum, "HashMap_DataMapFn failed. (%d != %d)",
                iSum, iExpectedSum);

    HashMap_Destroy(&hashMap);
    free(pValues);
    return TRUE;
}

_Bool Test_ManyPutGet(void *pUserData) {
    UNUSED(pUserData);

    HASHMAP hashMap;
    HASHMAP_STATUS hmStatus;

    hmStatus = HashMap_Initialize(&hashMap, 10, NULL, NULL);
    TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Initialize failed.");

    int iMaxVal = 20;
    int *pValues = malloc(sizeof(int) * iMaxVal);
    TEST_ASSERT(pValues != NULL, "malloc failed");

    for (int i = 0; i < iMaxVal; i++) {
        char pszKey[20];

        sprintf(pszKey, "foo%d", i);
        pValues[i] = i;
        hmStatus = HashMap_Put(&hashMap, pszKey, &pValues[i]);
        TEST_ASSERT(hmStatus == HASHMAP_STATUS_OK, "HashMap_Put failed (%d).",
                    i);
    }

    for (intptr_t i = 0; i < iMaxVal; i++) {
        char pszKey[20] = {0};

        sprintf(pszKey, "foo%lld", i);
        void *piValue;

        hmStatus = HashMap_Get(&hashMap, pszKey, &piValue);
        TEST_ASSERT(*(int *)piValue == i,
                    "HashMap_Get failed ((iValue)%lld != (i)%lld).",
                    *(int *)piValue, i);
    }

    HashMap_Destroy(&hashMap);
    free(pValues);
    return TRUE;
}

int main(int argc, char **argv) {
    TESTSUITE testSuite;

    TestSuite_Initialize(&testSuite);
    {
        TestSuite_AddTestCase(&testSuite, "HashMap Put/Get", Test_SinglePutGet,
                              NULL);
        TestSuite_AddTestCase(&testSuite, "HashMap Multiple Put/Get",
                              Test_MultiplePutGet, NULL);
        TestSuite_AddTestCase(&testSuite, "HashMap Overwrite",
                              Test_OverwriteKey, NULL);
        TestSuite_AddTestCase(&testSuite, "HashMap Free Function",
                              Test_FreeFunction, NULL);
        TestSuite_AddTestCase(&testSuite, "HashMap Free Function (User Data)",
                              Test_UserData_FreeFn, NULL);
        TestSuite_AddTestCase(&testSuite, "HashMap N > Capacity Put/Get",
                              Test_ManyPutGet, NULL);
        TestSuite_AddTestCase(&testSuite, "HashMap Map Data Function",
                              Test_DataMapFn, NULL);
    }

    TestSuite_Run(&testSuite);
    TestSuite_Destroy(&testSuite);

    if (_CrtDumpMemoryLeaks()) {
        printf("Memory leak found, check DbgView64 for more information.\n");
    }
    return 0;
}
