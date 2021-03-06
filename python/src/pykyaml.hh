#ifndef PYKYAML_HH
#define PYKYAML_HH

#include <Python.h>
#include <kyaml.hh>
#include <stdexcept>

namespace pykyaml
{
  // wrapper object for PyObject*
  class py_object
  {
  public:
    py_object(PyObject *self, bool incref) :
      d_self(self)
    {
      if(incref && d_self)
        Py_INCREF(d_self);
    }

    py_object(py_object const &other)
    {
      copy(other);
    }

    py_object(py_object &&other)
    {
      move(std::move(other));
    }

    ~py_object()
    {
      destroy();
    }

    py_object &operator=(py_object const &other)
    {
      if(this != &other)
      {
        destroy();
        copy(other);
      }
      return *this;
    }

    py_object &operator=(py_object &&other)
    {
      if(this != &other)
      {
        destroy();
        move(std::move(other));
      }
      return *this;
    }

    PyObject const *get() const
    {
      return d_self;
    }

    PyObject *get()
    {
      return d_self;
    }

    PyObject *release()
    {
      PyObject *self = d_self;
      d_self = nullptr;
      return self;
    }

  private:
    void copy(py_object const &other)
    {
      d_self = other.d_self;
      if(d_self)
        Py_INCREF(d_self);
    }

    void move(py_object &&other)
    {
      d_self = other.d_self;
      other.d_self = nullptr;
    }

    void destroy()
    {
      if(d_self)
        Py_DECREF(d_self);
    }

    PyObject *d_self;
  };

  class call_checker
  {
  public:
    static void initialize(std::string const &name)
    {
      if(!s_exception)
        s_exception = PyErr_NewException((char *)name.c_str(), nullptr, nullptr);
    }

    template <typename callable_t, typename... args_t>
    py_object call(callable_t &callable, args_t... args)
    {
      assert(s_exception);

      try
      {
        return callable(args...);
      }
      catch(std::exception const &e)
      {
        PyErr_SetString(s_exception, e.what());
        return py_object(nullptr, true);
      }
    }

    static PyObject *get()
    {
      return s_exception;
    }

  private:
    static PyObject *s_exception;
  };

  py_object build_leaf(std::shared_ptr<const kyaml::scalar> value);

  py_object build_tree(kyaml::document const &root, bool convert);

  void init_parser(PyObject *module);
  void init_leaf(PyObject *module);
}

#endif // PYKYAML_HH
