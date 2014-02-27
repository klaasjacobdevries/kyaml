#include "comment_clauses.hh"
#include "clause_test.hh"
#include <gtest/gtest.h>

using namespace std;
using namespace kyaml::test;
using namespace kyaml::clauses;

namespace
{
  clause_testcase<string_result> tc(string const &input, bool result, string const &val)
  {
    return 
      testcase_builder<string_result>(input, result).
      with_value(val).
      with_consumed(result ? val.size() : 0).
      build();
  }
}

CLAUSE_TEST(non_break_comment_text,
            cases({tc("# klaas", true, "# klaas"),
                   tc("## klaas", true, "## klaas"),
                   tc("#", true, "#"),
                   tc("klaas", false, ""),
                   tc("# klaas\n", true, "# klaas")}))

CLAUSE_TEST(sbreak_comment,
            cases({tc("#klaas\n", true, "#klaas\n")}))