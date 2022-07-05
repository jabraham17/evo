#ifndef VIZ_VIZ_H_
#define VIZ_VIZ_H_

#include "img/img.h"
#include "simulator/environment.h"
#include "dot/dot.h"

img_t* viz_dump_environment(struct environment* env, size_t scale);

dgraph_t* viz_dump_creature(struct creature* c);

#endif
