#include "node_clauses.hh"
#include "clause_test.hh"
#include <gmock/gmock.h>

using namespace std;
using namespace kyaml;
using namespace kyaml::test;
using namespace kyaml::clauses;
using namespace testing;

namespace
{
  class mock_builder : public document_builder
  {
  public:
    // not (yet?) interested in these:
    void add(char const *tag, void_item const &v) override
    {}
    void add(char const *tag, std::string const &v) override
    {}
  
    MOCK_METHOD1(add_anchor, void(string const &));
    MOCK_METHOD1(add_alias, void(string const &));
    MOCK_METHOD1(add_scalar, void(string const &));
  };
}

class properties_test : public Test
{
public:

  bool parse(std::string const &input)
  {
    context_wrap cw(input);
    return properties(cw.get()).parse(builder());
  }

  mock_builder &builder()
  {
    return d_builder;
  }

  void expect_anchor(std::string const &name)
  {
    EXPECT_CALL(builder(), add_anchor(name)).
      Times(1);
  }

  void expect_scalar(std::string const &value)
  {
    EXPECT_CALL(builder(), add_scalar(value)).
      Times(1);
  }
private:
  mock_builder d_builder;
};

TEST_F(properties_test, anchor)
{
  string const name = "anchor";
  string input = string("&") + name + " ";
  
  expect_anchor(name);
  EXPECT_TRUE(parse(input));
}

TEST_F(properties_test, verbatim_tag)
{
  string const value = "kl@2549#[]=0";
  string input = string("!<") + value + ">";
  
  expect_scalar(value);
  EXPECT_TRUE(parse(input));
}

TEST(alias_test, alias)
{
  string const name = "anchor";
  string input = string("*") + name + " ";
  
  context_wrap cw(input);
  alias_node an(cw.get());
  mock_builder mb;
  EXPECT_CALL(mb, add_alias(name)).
    Times(1);
  EXPECT_TRUE(an.parse(mb));
}

struct dquote_testcase
{
  string input;
  string expect;
  context::blockflow_t flow;
};

class dquote_test : public TestWithParam<dquote_testcase>
{};

TEST_P(dquote_test, extract)
{
  context_wrap cw(GetParam().input, 0, GetParam().flow);
  double_quoted dq(cw.get());

  string_builder sb;
  EXPECT_TRUE(dq.parse(sb));
  EXPECT_EQ(GetParam().expect, sb.build());
}

dquote_testcase dquote_testcases[] =
{
  {"\"klaas\"", "klaas", context::FLOW_IN},
//  {"\"klaas  \\\njacob\"", "klaas\njacob", context::FLOW_IN},
//  {"\"folded \nto a space,\t\n \nto a line feed, or \t\\\n   \tnon-content\"", "blah", context::FLOW_IN},
};

INSTANTIATE_TEST_CASE_P(dquote_tests,
                        dquote_test,
                        ValuesIn(dquote_testcases));

namespace
{
  clause_testcase qt(string const &input, string const &expect, context::blockflow_t bf = context::FLOW_IN)
  {
    return
      testcase_builder(input, true).
      with_consumed(input.size()).
      with_value(expect).
      with_blockflow(bf).
      build();
  }
}

CLAUSE_TEST(nonbreak_nonspace_double_inline,
            cases({qt("aap noot mies", "aap noot mies"),
                   qt("  klaas", "  klaas")}))
