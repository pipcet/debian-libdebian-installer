#ifndef TEST_H
#define TEST_H

#include <check.h>

Suite* make_test_exec_suite();
Suite* make_test_file_rfc822_suite();
Suite* make_test_hash_suite();
Suite* make_test_release_suite();
Suite* make_test_system_packages_suite();

#endif
