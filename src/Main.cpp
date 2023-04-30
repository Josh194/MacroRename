#include <iostream>

/*
#define TEST

int i = TEST_A;

#ifdef TEST
	#error "msg"
#endif

#define FOO 1
#define BAR FOO

int i = BAR;
*/

#define TEST2 #include

TEST2 "test.h"

#define TEST3 "test.h"

#include TEST3

int main() {

}