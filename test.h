#ifndef TEST_H
#define TEST_H

#include <stddef.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define TEST_ASSERT(expression, message, ...) do { \
	if(!(expression)) { \
		printf(" ... failed \n\t" #expression ", "); \
		printf(message "\n", __VA_ARGS__); \
		return FALSE; \
	} \
} while(0)

typedef struct tagTESTCASE TESTCASE, *PTESTCASE;

typedef struct tagTESTSUITE {
	size_t cTests;
	PTESTCASE pTestCases;
	PTESTCASE pLastTestCase;
} TESTSUITE, *PTESTSUITE;

typedef _Bool (*TEST_ROUTINE)(void *);
struct tagTESTCASE {
	char const *pszName;
	TEST_ROUTINE pTestFn;
	void *pUserData;
	PTESTCASE pNext;
};

void TestSuiteInitialize(PTESTSUITE pTestSuite);
void TestSuiteDestroy(PTESTSUITE pTestSuite);
_Bool TestSuiteAddTestCase(
	PTESTSUITE pTestSuite,
	char const *pszName,
	TEST_ROUTINE pTestFn,
	void *pUserData
);

void TestSuiteRun(PTESTSUITE pTestSuite);

#endif
