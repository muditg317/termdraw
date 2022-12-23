#pragma once

namespace termdraw {

typedef void preloopFunc(int argc, char *argv[]);
typedef void loopFunc(void);
typedef void finishFunc(int signum);

} // namespace termdraw
