#ifndef FUNCTION_REGISTRY_HPP
#define FUNCTION_REGISTRY_HPP

#include <functional>
#include <memory>
#include <type_traits>
#include <vector>

template<typename ReturnType, int MAX_FUNCS = 0, typename... ArgTypes>
class FunctionRegistry;

template<typename ReturnType, int MAX_FUNCS, typename... ArgTypes>
class FunctionRegistry<ReturnType(ArgTypes...), MAX_FUNCS> {
 public:
  typedef std::function<ReturnType(ArgTypes...)> __func_type;

  FunctionRegistry(void) : locked(false) {}

  void registerFunction(__func_type f);

  void lock(void);

  std::vector<ReturnType> operator()(ArgTypes... args);

  inline bool isLocked(void) const { return locked; }

 private:
  std::vector<__func_type> functions;
  bool locked;
};


template<int MAX_FUNCS, typename... ArgTypes>
class FunctionRegistry<void(ArgTypes...), MAX_FUNCS> {
 public:
  typedef std::function<void(ArgTypes...)> __func_type;

  FunctionRegistry(void) : locked(false) {}

  void registerFunction(__func_type f);

  void lock(void);

  void operator()(ArgTypes... args);

  inline bool isLocked(void) const { return locked; }

 private:
  std::vector<__func_type> functions;
  bool locked;
};

#include <termdraw/function-registry_impl.hpp>

#endif
