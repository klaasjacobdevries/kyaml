#include "kyaml.hh"
#include "sample_docs.hh"
#include "utils.hh"
#include <gtest/gtest.h>

using namespace std;
using namespace kyaml;
using namespace kyaml::test;

class multidoc_base : public testing::Test
{
public:
  void check_sync(std::string const &head, unsigned linenumber)
  {
    ASSERT_TRUE((bool)d_parser);
    EXPECT_EQ(head, d_parser->peek(head.size()));
    EXPECT_EQ(linenumber, d_parser->linenumber());
  }

  unique_ptr<const document> parse(unsigned n = 0)
  {
    assert(d_parser);

    skip(n);
    return d_parser->parse();
  }

  stringstream const &stream() const
  {
    return d_stream;
  }

protected:
  void construct(std::string const &input)
  {
    d_stream.str(input);
    d_parser.reset(new kyaml::parser(d_stream));
  }

private:
  void skip(unsigned n)
  {
    assert(d_parser);

    for(unsigned i = 0; i < n; ++i)
    {
      try
      {
        d_parser->parse();
      }
      catch(parser::error const &e)
      {}
    }
  }

  stringstream d_stream;
  unique_ptr<kyaml::parser> d_parser;
};

class multidoc : public multidoc_base
{
public:
  void SetUp() override
  {
    multidoc_base::SetUp();
    construct(g_multi_yaml);
  }
};

TEST_F(multidoc, bare)
{
  // stream 1

  unique_ptr<const document> root = parse();
  ASSERT_TRUE((bool)root);

  EXPECT_EQ("bare document", root->leaf_value());


  check_sync("%YAML 1.2\n", 6);
}

TEST_F(multidoc, sequence)
{
  // stream 2
  unique_ptr<const document> root = parse(1);
  ASSERT_TRUE((bool)root);

  EXPECT_EQ("item 1", root->leaf_value("sequence", 0));
  EXPECT_EQ("item 2", root->leaf_value("sequence", 1));

  check_sync("---\n", 13);
}

TEST_F(multidoc, mapping)
{
  // stream 3
  unique_ptr<const document> root = parse(2);
  ASSERT_TRUE((bool)root);

  EXPECT_EQ("value 1", root->leaf_value("mapping", "key1"));
  EXPECT_EQ("value 2", root->leaf_value("mapping", "key2"));

  // eof
  EXPECT_TRUE(stream().eof());
}

// the purpose of the unhappy stream test is not as such to lay down the
// one and only rules for what happens on invalid input, but rather
// that (1) something reasonable happends and (2) the stream is synced
// for the next document.
class unhappy : public multidoc_base
{
public:
  unhappy() :
    d_log("unhappy")
  {}

  void SetUp() override
  {
    multidoc_base::SetUp();

    // by putting the unhappy scenarios in the same stream we implicitely
    // test for stream synchronization on errors
    construct(g_unhappy_stream_yaml);
  }

  void parse_error(unsigned n, unsigned linenumber)
  {
    try
    {
      unique_ptr<const document> root = parse(n);

      // if we reach this, no exception was thrown
      FAIL() << "no exception was thrown in " << *root;
    }
    catch(parser::parse_error const &e)
    {
      d_log("parse error", e.what());
      EXPECT_EQ(linenumber, e.linenumber());
    }
  }

  void content_error(unsigned n, unsigned linenumber)
  {
    try
    {
      unique_ptr<const document> root = parse(n);

      // if we reach this, no exception was thrown
      FAIL() << "no exception was thrown in " << *root;
    }
    catch(parser::content_error const &e)
    {
      d_log("content error", e.what());
      EXPECT_EQ(linenumber, e.linenumber());
    }
  }

private:
  logger<false> d_log;
};

TEST_F(unhappy, unbalanced_quote)
{
  parse_error(0, 3);
  check_sync("---\n# eos 1", 6);
}

TEST_F(unhappy, empty)
{
  unique_ptr<const document> root = parse(1);
  ASSERT_TRUE((bool)root);

  EXPECT_EQ("", root->leaf_value());

  check_sync("---\n# eos 2", 9);
}

TEST_F(unhappy, invalid_string)
{
  unique_ptr<const document> root = parse(2);
  ASSERT_TRUE((bool)root);

  EXPECT_EQ("|", root->leaf_value("string"));

  check_sync("---\n# eos 3", 14);
}

TEST_F(unhappy, unbalanced)
{
  parse_error(3, 17);
  check_sync("---\n# eos 4", 19);
}

TEST_F(unhappy, indent)
{
  parse_error(4, 25);
  check_sync("---\n# eos 5", 26);
}

TEST_F(unhappy, invalid_anchor)
{
  content_error(5, 29);
  check_sync("---\n# eos 6", 30);
}

