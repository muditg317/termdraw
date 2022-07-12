#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <memory>

class Application {
 public:
  int run(int argc, char *argv[]);

 private:
  void preloop(int argc, char *argv[]);
  void loop(void);
  void finish(int signum = 0);

};

extern std::shared_ptr<Application> app;

#endif