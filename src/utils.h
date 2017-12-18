#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <unistd.h>

// A callback for deleting objects on the main thread using later(). This is
// needed when the object is an Rcpp object or contains one, because deleting
// such objects invoke R's memory management functions.
template <typename T>
void delete_cb_main(void* obj) {
  ASSERT_MAIN_THREAD()
  // later() expects a void*, so we have to cast it.
  T typed_obj = reinterpret_cast<T>(obj);
  delete typed_obj;
}

// Does the same as delete_cb_main, but checks that it's running on the
// background thread (when thread debugging is enabled).
template <typename T>
void delete_cb_bg(void* obj) {
  ASSERT_BACKGROUND_THREAD()
  T typed_obj = reinterpret_cast<T>(obj);
  delete typed_obj;
}


// It's not safe to call REprintf from the background thread but we need some
// way to output error messages. R CMD check does not it if the code uses the
// symbols stdout, stderr, and printf, so this function is a way to avoid
// those. It's to calling `fprintf(stderr, ...)`.
inline void err_printf(const char *fmt, ...) {
  size_t max_size = 4096;
  char buf[max_size];

  va_list args;
  va_start(args, fmt);
  int n = vsnprintf(buf, max_size, fmt, args);
  va_end(args);

  if (n == -1)
    return;

  write(STDERR_FILENO, buf, n);
}

#endif