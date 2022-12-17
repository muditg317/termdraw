template<typename ReturnType, int MAX_FUNCS, typename... ArgTypes>
void FunctionRegistry<ReturnType(ArgTypes...), MAX_FUNCS>::registerFunction(__func_type func) {
  if (locked) {
    throw std::runtime_error("Function registry is locked! Cannot register new function");
  }
  if (MAX_FUNCS != 0 && functions.size() >= MAX_FUNCS) {
    throw std::runtime_error("Function registry is full! Cannot register new function");
  }
  this->functions.push_back(func);
  if (MAX_FUNCS != 0 && functions.size() >= MAX_FUNCS) {
    this->lock();
  }
}

template<typename ReturnType, int MAX_FUNCS, typename... ArgTypes>
void FunctionRegistry<ReturnType(ArgTypes...), MAX_FUNCS>::lock(void) {
  this->locked = true;
}

template<typename ReturnType, int MAX_FUNCS, typename... ArgTypes>
std::vector<ReturnType> FunctionRegistry<ReturnType(ArgTypes...), MAX_FUNCS>::operator()(ArgTypes... args) {
  if (!locked) {
    throw std::runtime_error("Function registry is not locked! Cannot call functions\n");
  }
  std::vector<ReturnType> results;
  for (auto func : this->functions) {
    results.push_back(func(args...));
  }
  return results;
}

template<int MAX_FUNCS, typename... ArgTypes>
void FunctionRegistry<void(ArgTypes...), MAX_FUNCS>::registerFunction(__func_type func) {
  if (locked) {
    throw std::runtime_error("Function registry is locked! Cannot register new function");
  }
  if (MAX_FUNCS != 0 && functions.size() >= MAX_FUNCS) {
    throw std::runtime_error("Function registry is full! Cannot register new function");
  }
  this->functions.push_back(func);
  if (MAX_FUNCS != 0 && functions.size() >= MAX_FUNCS) {
    this->lock();
  }
}

template<int MAX_FUNCS, typename... ArgTypes>
void FunctionRegistry<void(ArgTypes...), MAX_FUNCS>::lock(void) {
  this->locked = true;
}

template<int MAX_FUNCS, typename... ArgTypes>
void FunctionRegistry<void(ArgTypes...), MAX_FUNCS>::operator()(ArgTypes... args) {
  if (!locked) {
    throw std::runtime_error("Function registry is not locked! Cannot call functions\n");
  }
  for (auto func : this->functions) {
   func(args...);
  }
}