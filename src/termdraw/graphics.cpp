#include <termdraw/graphics.hpp>

#include <cstdio>
// #include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cassert>
#include <iostream>
#include <cstring>
#include <chrono>
#include <thread>
#include <cmath>
#include <string>
#include <csignal>
#include <cstdarg>
#include <vector>

// #include <fmt/format.h>
#include <Eigen/Eigen>
// #include <boost/algorithm/string/join.hpp>

static double _frameRate = DEFAULT_FRAME_RATE;
double getFrameRate() {
  return _frameRate;
}

static int _width;
int getWidth() {
  return _width;
}

static int _height;
int getHeight() {
  return _height;
}

static PixelBuffer pixelBuffer;
static char *consoleBuffer;

static bool pixelBufferAllocated = false;
PixelBuffer& getPixelBuffer() {
  assert(pixelBufferAllocated && "Must allocate buffer before reading or writing!");
  // printf("accessed buffer!\n");
  return pixelBuffer;
}

void allocate_buffers(void) {
  pixelBuffer = PixelBuffer(WIDTH,HEIGHT);
  pixelBufferAllocated = true;

  consoleBuffer = (char *) calloc(CONSOLE_HEIGHT, sizeof(char) * CONSOLE_LINE_SIZE);
  assert(consoleBuffer != nullptr && "Failed to allocate console buffer for terminal drawing session!");
}

static bool dims_set = false;
void display_size(int width, int height) {
  assert(!dims_set && "Display cannot be set twice!");
  _width = width;
  _height = height;
  dims_set = true;

  allocate_buffers();
}


inline struct winsize &getConsoleSize() {
  static struct winsize w;
  static bool windowSized = false;
  if (!windowSized) {
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    windowSized = true;
  }
  return w;
}
void display_size_based_on_console(int rowsToSave) {
  struct winsize w = getConsoleSize();
  display_size(w.ws_col * WIDTH_SCALE, (w.ws_row - rowsToSave - 1) * HEIGHT_SCALE);
}

void frameRate(double fr) {
  assert(fr > 0 && "Cannot use non-positive framerate!");
  _frameRate = fr;
  if (fr > MAX_FR_RECOMMENDATION) {
    printf("The provided frame rate %.2f may be too high! Recommended max: %.2f\n", fr, MAX_FR_RECOMMENDATION);
  }
}

std::vector<std::string> splitByDelimToCstr(std::string strToSplit, std::string delim) {
  size_t pos_start = 0, pos_end, delim_len = delim.length();
  std::string token;
  std::vector<std::string> res;

  while ((pos_end = strToSplit.find (delim, pos_start)) != std::string::npos) {
    token = strToSplit.substr (pos_start, pos_end - pos_start);
    pos_start = pos_end + delim_len;
    res.push_back(token);
  }

  res.push_back(strToSplit.substr(pos_start));
  return res;
}

void _graphics_print_string(std::string result) {
  // std::cout << "internal printer -- ==========" << result << "=========" << std::endl;
  std::string formatString = fmt::format("%-{}s\n", std::to_string(getConsoleSize().ws_col));
  // std::cout << "internal printer -- format: " << formatString << std::endl;

  std::vector<std::string> resLines = splitByDelimToCstr(result, "\n");
  // std::cout << "internal printer -- lines: " << resLines.size() << std::endl;
  std::string lastLine = resLines.back();
  for (std::string line : resLines) {
    // std::cout << "internal printer -- line: " << line << std::endl;
    if (line == lastLine) {
      if (line.size() == 0) {
        return;
      }
      formatString = formatString.substr(0, formatString.size()-1); // remove \n
    }
    // std::cout << "internal printer -- format: " << formatString << std::endl;
    std::printf(formatString.c_str(), line.c_str());
  }
}

inline char pixelsToBraille_offset1(PixBuf<WIDTH_SCALE,HEIGHT_SCALE> pixelGroup) {
  return pixelGroup(0,3) + (pixelGroup(1,3) << 1);
}

inline char pixelsToBraille_offset2(PixBuf<WIDTH_SCALE,HEIGHT_SCALE> pixelGroup) {
  return pixelGroup(0,0) + (pixelGroup(0,1) << 1) + (pixelGroup(0,2) << 2) + (pixelGroup(1,0) << 3) + (pixelGroup(1,1) << 4) + (pixelGroup(1,2) << 5);
}
inline void render(void) {
  static char baseBraille[] = "\u2800";
  for (int y = 0; y < CONSOLE_HEIGHT; ++y) {
    for (int x = 0; x < CONSOLE_WIDTH; ++x) {
      PixBuf<WIDTH_SCALE,HEIGHT_SCALE> pixelGroup = pixelBuffer.block<WIDTH_SCALE,HEIGHT_SCALE>(x*WIDTH_SCALE,y*HEIGHT_SCALE);
      // validate pixel values
      assert((((pixelGroup.array() == 0).count() + (pixelGroup.array() == 1).count()) == WIDTH_SCALE*HEIGHT_SCALE) && "Matrix must have only 1s and 0s!");

      char offset1 = pixelsToBraille_offset1(pixelGroup);
      char offset2 = pixelsToBraille_offset2(pixelGroup);

      // bool top = pixelGroup(0,0);
      // bool bottom = pixelGroup(0,0);

      // wchar_t choices[2][2] = {{' ','▄',},{'▀','█'}};
      // char choices[2][2] = {{' ','o',},{'*','0'}};

      // consoleBuffer[CONSOLE_LINE_SIZE*y + BYTES_PER_CONSOLE_CHAR*x+0] = choices[top][bottom];
      consoleBuffer[CONSOLE_LINE_SIZE*y + BYTES_PER_CONSOLE_CHAR*x+0] = baseBraille[0];
      consoleBuffer[CONSOLE_LINE_SIZE*y + BYTES_PER_CONSOLE_CHAR*x+1] = baseBraille[1] + offset1;
      consoleBuffer[CONSOLE_LINE_SIZE*y + BYTES_PER_CONSOLE_CHAR*x+2] = baseBraille[2] + offset2;
    }
    consoleBuffer[CONSOLE_LINE_SIZE*(y+1) - 1] = '\n';
  }
  fwrite(consoleBuffer, CONSOLE_LINE_SIZE, CONSOLE_HEIGHT, stdout);
}

void clean(void) {
  pixelBuffer.setZero();
}
/**
 * @brief move the cursor to the top of the drawing region
 * Clear the rest of any printed lines so the screen isn't duplicated
 */
inline void resetCursor(void) {
  printf("\x1b[%dD", CONSOLE_WIDTH);
  printf("\x1b[%dA", CONSOLE_HEIGHT);
  // getCursorPos(&lastCursorX,&lastCursorY);
}


std::function<preloopFunc> preloopFuncs[MAX_FUNCTION_REGISTRATIONS];
int numPreloopFuncs = 0;
void registerPreloop(preloopFunc func) {
  assert(numPreloopFuncs < MAX_FUNCTION_REGISTRATIONS && "Too many preloop functions registered!");
  preloopFuncs[numPreloopFuncs++] = func;
}

std::function<loopFunc> loopFuncs[MAX_FUNCTION_REGISTRATIONS];
int numLoopFuncs = 0;
void registerLoop(loopFunc func) {
  assert(numLoopFuncs < MAX_FUNCTION_REGISTRATIONS && "Too many loop functions registered!");
  loopFuncs[numLoopFuncs++] = func;
}

std::function<finishFunc> finishFuncs[MAX_FUNCTION_REGISTRATIONS];
int numFinishFuncs = 0;
void registerFinish(finishFunc func) {
  assert(numFinishFuncs < MAX_FUNCTION_REGISTRATIONS && "Too many finish functions registered!");
  finishFuncs[numFinishFuncs++] = func;
}

static void preloop(int argc, char *argv[]) {
  // graphics_printf("run preloops: %d\n", numPreloopFuncs);
  for (int8_t i = numPreloopFuncs-1; i >= 0; --i) {
    // graphics_printf("run preloop %d\n", i);
    preloopFuncs[i](argc, argv);
  }
}

static void loop(void) {
  for (int8_t i = numLoopFuncs-1; i >= 0; --i) {
    loopFuncs[i]();
  }
}

static void finish(int signum = 0) {
  // clean();
  render();
  for (int8_t i = numFinishFuncs-1; i >= 0; --i) {
    finishFuncs[i](signum);
  }
}

static std::chrono::time_point<std::chrono::system_clock> start;

static std::chrono::time_point<std::chrono::system_clock> t;
static std::chrono::microseconds microSecDelay;

static uint32_t framesComputed = 0;

void graphics_preloop(int argc, char *argv[]) {
  // graphics_printf("run graphics preloop\n");
  // signal(SIGINT, finish);
  // signal(SIGTERM, finish);

  struct sigaction signal_handler = {
    .__sigaction_handler = finish,
    .sa_flags = 0
  };
  sigemptyset(&signal_handler.sa_mask);

  sigaction(SIGINT, &signal_handler, nullptr);
  sigaction(SIGTERM, &signal_handler, nullptr);

  setup(argc, argv);
  assert(dims_set && "Must set dimensions for terminal drawing session! Call `display_size(width,height)`");
  
  printf("Finished termdraw setup!\n\tScreen:    \t%-3dx%3d\n\tConsole:\t%-3dx%3d\n\tFrame Rate:\t%.2f\n", WIDTH, HEIGHT, CONSOLE_WIDTH, CONSOLE_HEIGHT, FRAME_RATE);

  // printf("&pixelBuffer = %p\n", &pixelBuffer);
  // printf("&consoleBuffer = %p\n", &consoleBuffer);
  // printf("&argc = %p\n", &argc);

  int64_t microsDelay = std::floor(1000000.0/FRAME_RATE);
  microSecDelay = std::chrono::microseconds(microsDelay);
  
  start = t = std::chrono::system_clock::now();

  render();
  resetCursor();
}

void graphics_loop() {
  // graphics_printf("run graphics loop\n");

  // printf("call update!\n");
  update();
  // printf("call render!\n");
  render();
  // printf("call reset!\n");
  resetCursor();

  framesComputed++;

  t += microSecDelay;
  std::this_thread::sleep_until(t);
}

void graphics_finish(int signum) {
  // graphics_printf("run graphics finish\n");
  // if (signum) {
  //   std::cout << "Interrupt signal (" << signum << ") received.\n";
  // }


  std::chrono::time_point<std::chrono::system_clock> sim_end = std::chrono::system_clock::now();
  auto timeMsComputing = (std::chrono::duration_cast<std::chrono::milliseconds>(sim_end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::milliseconds>(start.time_since_epoch())).count();
  long int timeMsComputed = (int) (framesComputed * DELTA_T_MS);
  // auto timeMsComputing = std::chrono::duration_cast<std::chrono::milliseconds>(timeComputing.time_since_epoch());

  double ratio = ((float)timeMsComputed)/((float)timeMsComputing);

  printf("\nRendering stats:\n\ttime computed:   \t|%9ld ms\n\ttime computing:   \t|%9ld ms\n\trendering ratio:\t|%9.3f\n", timeMsComputed, timeMsComputing, ratio);

  if (signum) {
    exit(signum);
  }
}

bool quit_application = false;
int graphics_main(int argc, char *argv[]) {
  // graphics_printf("Run graphics main!\n");

  registerPreloop(graphics_preloop);
  registerLoop(graphics_loop);
  registerFinish(graphics_finish);

  preloop(argc, argv);

  while (!quit_application) {
    loop();
  }

  finish();
  return 0;
}