#include "node.hh"
#include <sstream>

using namespace std;
using namespace kyaml;

namespace
{
  ostream &operator<<(ostream &o, node::type_t t)
  {
    switch(t)
    {
    case node::MAPPING:
      o << "mapping";
      break;
    case node::SEQUENCE:
      o << "sequence";
      break;
    case node::SCALAR:
      o << "scalar";
      break;
    default:
      o << "(invalid " << (int)t << ")";
    }
    return o;
  }
}

node::wrong_type::wrong_type(type_t expect, type_t actual)
{
  stringstream str;
  str << "node type mismatch: expected " << expect << " but is " << actual << '\n';
  d_msg = str.str();
}



sequence const &node::as_sequence() const
{
  if(type() != SEQUENCE)
    throw wrong_type(SEQUENCE, type());

  return dynamic_cast<sequence const &>(*this);
}

scalar const &node::as_scalar() const
{
  if(type() != SCALAR)
    throw wrong_type(SCALAR, type());

  return dynamic_cast<scalar const &>(*this);
}

mapping const &node::as_mapping() const
{
  if(type() != MAPPING)
    throw wrong_type(MAPPING, type());

  return dynamic_cast<mapping const &>(*this);
}

string const &node::get() const
{
  assert(type() != SCALAR);
  throw wrong_type(SCALAR, type());
}

node const &node::get(size_t i) const
{
  assert(type() != SEQUENCE);
  throw wrong_type(SEQUENCE, type());
}

node const &node::get(const string &key) const
{
  assert(type() != MAPPING);
  throw wrong_type(MAPPING, type());
}

void node::add(std::shared_ptr<node> val)
{
  if(type() != SEQUENCE)
    throw wrong_type(SEQUENCE, type());

  dynamic_cast<sequence &>(*this).add(val);
}

void node::add(string const &key, std::shared_ptr<node> val)
{
  if(type() != MAPPING)
    throw wrong_type(MAPPING, type());

  return dynamic_cast<mapping &>(*this).add(key, val);
}

void sequence::print(ostream &o) const
{
  o << "[";
  for(auto it : *this)
  {
    it->print(o);
    o << ", ";
  }
  o << "]";
}

void mapping::print(ostream &o) const
{
  o << "{";
  for(auto it : *this)
  {
    o << it.first << " : ";
    it.second->print(o);
    o << ", ";
  }
  o << "}";
}

node const &mapping::get(const string &key) const
{
  container_t::const_iterator it = d_items.find(key);
  assert(it != d_items.end());
  assert(it->second);
  return *it->second;
}


ostream &std::operator<<(ostream &o, std::shared_ptr<const node> sp)
{
  if(sp)
    sp->print(o);
  else
    o << "(nullptr)";
  return o;
}
