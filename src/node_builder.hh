#ifndef NODE_BUILDER_HH
#define NODE_BUILDER_HH

#include "node.hh"
#include "document_builder.hh"
#include <stack>

namespace kyaml
{
  class node_builder : public document_builder
  {
  public:
    class unkown_alias : public std::exception
    {
    public:
      unkown_alias(std::string const &alias) :
        d_msg("unkown alias ")
      {
        d_msg += "'" + alias + "'\n";
      }

      char const *what() const throw() override
      {
        return d_msg.c_str();
      }

    private:
      std::string d_msg;
    };

    node_builder() :
      d_log("node builder")
    {}

    virtual void start_sequence() override;

    virtual void end_sequence() override;

    virtual void start_mapping() override;

    virtual void end_mapping() override;

    virtual void add_anchor(std::string const &anchor) override;

    virtual void add_alias(std::string const &alias) override;

    virtual void add_scalar(std::string const &val) override;

    virtual void add_atom(char32_t c) override;

    std::shared_ptr<node> build();

  private:
    typedef enum
    {
      SEQUENCE,
      MAPPING,
      MAPPING_KEY,
      ANCHOR,
      RESOLVED_NODE
    } token_t;

    struct item
    {
      token_t token;
      std::shared_ptr<node> value;

      item(token_t t, std::shared_ptr<node> v = std::shared_ptr<node>()) :
        token(t),
        value(v)
      {}
    };

    item pop();

    void resolve();
    void add_resolved_node(std::shared_ptr<node> s);

    std::unordered_map<std::string, std::weak_ptr<node> > d_anchors;

    std::stack<item> d_stack;

    logger<true> d_log;
  };
}

#endif // NODE_BUILDER_HH