#ifndef CHAR_STREAM_HH
#define CHAR_STREAM_HH

#include <istream>
#include <deque>
#include <string>

namespace kyaml
{
  typedef char32_t char_t;

  class char_stream
  {
  public:
    typedef size_t mark_t;

    char_stream(std::istream &base) : 
      d_base(base),
      d_pos(0),
      d_mark_valid(false)
    {}

    // get the next character, or EOF
    bool get(char_t &c);

    // peek the next character without modifying the stream
    bool peek(char_t &c);

    // peek one back
    bool rpeek(char_t &c);

    void advance(size_t n = 1);

    // mark the current position in the stream
    mark_t mark() const;

    // unwind the stream, setting the read pos back to m. 
    void unwind(mark_t m);

    // purge all buffers until pos(), will invalidate marks
    void ignore();
    // ignore until character is reached
    void ignore(char c);

    // eat all charactes to the current read pos. Returns all from m to the current read pos. Note
    // that this invalidates all marks previously returned by mark().
    std::string consume(mark_t m = 0);

    // mostly for diagnostic purposes
    mark_t pos() const
    {
      return d_pos;
    }

    bool good() const
    {
      return 
        d_pos < d_buffer.size() ||
        d_base.good();
    }

    bool eof() const
    {
      return 
        d_pos >= d_buffer.size() &&
        d_base.eof();
    }

    // return the indent level, that is the number of chars since the last newline (\r or \n). or start of file
    // returns 0 if this could not be established
    // hint can be used as a known lower bound.
    size_t indent_level(size_t hint = 0) const;

  private:
    bool underflow();

    std::istream      &d_base;
    std::deque<char_t> d_buffer;
    mark_t             d_pos;
    mutable bool       d_mark_valid; // only for additional run-time error checking
  };
}

#endif // CHAR_STREAM_HH
