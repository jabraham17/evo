#ifndef VIZ_VIZ_H_
#define VIZ_VIZ_H_

#include "simulator/environment.h"
#include "img/img.h"

img_t* viz_dump_environment(struct environment* env, size_t scale);

#endif
