

#include <assert.h>

#ifdef __cplusplus
extern "C"
#endif
char pselect();

int main() {
#if defined (__stub_pselect) || defined (__stub___pselect)
  fail fail fail
#else
  pselect();
#endif

  return 0;
}
