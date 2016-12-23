#ifndef TEST_H
#define TEST_H

#ifdef _DEBUG
#define RUN_TESTS() tests_run()
#else
#define RUN_TESTS()
#endif

void tests_run();

#endif // TEST_H