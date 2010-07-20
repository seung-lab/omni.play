/*
 * AUTHORS:
 *   Aleksandar Zlateski <zlateski@mit.edu>
 *
 * Do not share without authors permission.
 */

#include <iostream>
#include <iomanip>
#include "zunit.h"
#include "zi/base/time.h"
#include "zi/base/bash.h"

namespace zi { namespace Test {

UnitTestSuite* UnitTestSuite::suite_ = 0;

UnitTestSuite& UnitTestSuite::get() {
  if (suite_ == NULL) {
    suite_ = new UnitTestSuite();
  }
  return *(suite_);
}

int UnitTestSuite::runTests() {
  ZTIMER timer;
  double totTime = 0;
  int total = 0, passed = 0;
  for (int i=0;i<factories_.size();i++) {
    UnitTest *test = factories_[i]->getTest();
    std::cout << "-------------------------------------------------------"
              << std::endl
              << "EXECUTING TEST: "
              << test->getName()
              << std::endl
              << "-------------------------------------------------------"
              << std::endl;
    double t = timer.dLap();
    test->run();
    t = timer.dLap();
    totTime += t;
    total++;
    if (!test->getPassed()) {
      std::cout << Bash::RED << "FAILED" << Bash::GRAY
                << " : " << test->getRet() << std::endl;
    } else {
      passed++;
      std::cout << Bash::LITEGREEN << "PASSED" << Bash::GRAY
                << " (" << std::setprecision(3)
                << std::fixed << t << " secs)"
                << std::endl;
    }
  }
  std::cout << "======================================================="
            << std::endl
            << "PASSED " << ((passed == total) ? Bash::GREEN : Bash::RED)
            << passed << "/" << total << Bash::WHITE << " TEST ("
            << totTime << " : " << timer.dTotal() << " secs)" << std::endl;
  return 0;
}

} }
