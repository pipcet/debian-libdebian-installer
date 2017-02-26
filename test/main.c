#include <stdlib.h>

#include <check.h>

#include "test.h"

int main() {
  int number_failed;
  SRunner *sr;

  sr = srunner_create(make_test_hash_suite());
  srunner_set_tap(sr, "-");
  srunner_add_suite(sr, make_test_exec_suite());
  srunner_add_suite(sr, make_test_file_rfc822_suite());
  srunner_add_suite(sr, make_test_package_suite());
  srunner_add_suite(sr, make_test_packages_suite());
  srunner_add_suite(sr, make_test_release_suite());
  srunner_add_suite(sr, make_test_version_suite());
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
