#ifdef _WIN32
  #include <windows.h>
  
  void timeout(unsigned int seconds) {
    Sleep(seconds * 1000);  // No Windows, Sleep é em milissegundos
  }
#else
  #include <unistd.h>
  
  void timeout(unsigned int seconds) {
    sleep(seconds);  // No POSIX, sleep é em segundos
  }
#endif
