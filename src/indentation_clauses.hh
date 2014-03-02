#ifdef COMPILE_GUARD

#ifndef INDENDATION_CLAUSES_HH
#define INDENDATION_CLAUSES_HH

#include "clauses_base.hh"

namespace kyaml
{
  namespace clauses
  {
    // [63] 	s-indent(n) 	::= 	s-space × n 
    class indent_clause_eq : public internal::void_clause
    {
    public:
      indent_clause_eq(context &c) : void_clause(c)
      {}

      bool try_clause();
    private:
      unsigned level() const
      {
        return ctx().indent_level();
      }
    };

    namespace internal
    {
      // special case of [63] needed to fulfill clause [69]
      class indent_clause_ge : public internal::void_clause
      {
      public:       
        indent_clause_ge(context &c) : void_clause(c)
        {}
        
        bool try_clause();
      private:
        unsigned level() const
        {
          return ctx().indent_level();
        }
      };  
    }

    // [64] 	s-indent(<n) 	::= 	s-space × m Where m < n 
    class indent_clause_lt : public internal::void_clause
    {
    public:
      indent_clause_lt(context &c) : 
        void_clause(c),
        d_m(0)
      {}

      bool try_clause();
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
    class indent_clause_le : public internal::void_clause
    {
    public:
      indent_clause_le(context &c) :
        void_clause(c),
        d_m(0)
      {}

      bool try_clause();
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

#endif // COMPILE_GUARD
