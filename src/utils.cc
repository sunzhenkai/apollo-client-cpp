#include <sys/time.h>
// self
#include "apollo/utils.h"

namespace apollo {
long CurrentMilliseconds() {
  struct timeval now{};
  gettimeofday(&now, nullptr);
  return now.tv_sec * 1000 + now.tv_usec / 1000;
}
} // namespace apollo
