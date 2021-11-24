#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP

typedef bool keyPressHandler(char);

/**
 * @brief register a key pres handler for the graphics session
 * should return boolean (true to terminate program, false otherwise)
 */
void onKeyPress(keyPressHandler *);

int keyboard_main(int argc, char *argv[]);

#ifdef KEYBOARD_MAIN
int main(int argc, char *argv[]) {
  keyboard_main(argc, argv);
}
#endif


#endif