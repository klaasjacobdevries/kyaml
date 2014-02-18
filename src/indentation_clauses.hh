#ifndef INDENDATION_CLAUSES_HH
#define INDENDATION_CLAUSES_HH

#include "clauses_base.hh"

namespace kyaml
{
  namespace clauses
  {
    // [63] 	s-indent(n) 	::= 	s-space × n 
    class indent_clause_eq : public clause
    {
    public:
      typedef unsigned value_t;

      indent_clause_eq(context &c) : clause(c)
      {}

      bool try_clause();

      value_t value() const
      {
        return level();
      }
    private:
      unsigned level() const
      {
        return ctx().indent_level();
      }
    };

    // [64] 	s-indent(<n) 	::= 	s-space × m Where m < n 
    class indent_clause_lt : public clause
    {
    public:
      typedef unsigned value_t;

      indent_clause_lt(context &c) : 
        clause(c),
        d_m(0)
      {}

      bool try_clause();

      value_t value() const
      {
        return lower_level();
      }
    private:
      unsigned lower_level() const
      {
        return d_m;
      }
      unsigned higher_level() const
      {
        return ctx().indent_level();
      }
      
      unsigned d_m;
    };

    // [65] 	s-indent(≤n) 	::= 	s-space × m /* Where m ≤ n */ 
    class indent_clause_le : public clause
    {
    public:
      typedef unsigned value_t;

      indent_clause_le(context &c) :
        clause(c),
        d_m(0)
      {}

      bool try_clause();

      value_t value() const
      {
        return lower_level();
      }
    private:
      unsigned lower_level() const
      {
        return d_m;
      }
      unsigned higher_level() const
      {
        return ctx().indent_level();
      }
      unsigned d_m;
    };
  }
}

#endif // INDENDATION_CLAUSES_HH