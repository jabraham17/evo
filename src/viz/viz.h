#ifndef VIZ_VIZ_H_
#define VIZ_VIZ_H_

#include "dot/dot.h"
#include "img/img.h"
#include "simulator/environment.h"

img_t* viz_dump_environment(struct environment* env, size_t scale);

dgraph_t* viz_dump_creature(struct creature* c);

#endif
