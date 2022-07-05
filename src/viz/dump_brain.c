#include "viz.h"
#include <stdbool.h>
#include <stdio.h>

static float scale_to_float(
    int16_t value,
    int16_t oldMin,
    int16_t oldMax,
    float floatMin,
    float floatMax) {

    float result = (floatMax - floatMin) * ((float)value - (float)oldMin) /
                       ((float)oldMax - (float)oldMin) +
                   floatMin;
    return result;
}

static void connection_to_dot(dgraph_t* dg, connection_t c) {

    uint16_t source = genome_connection_source_unique_id(c);
    char* source_label = genome_connection_source_str(c);
    uint16_t sink = genome_connection_sink_unique_id(c);
    char* sink_label = genome_connection_sink_str(c);

    dot_add_vertex(dg, source);
    dot_set_attribute_on_vertex(dg, source, "label", source_label);

    dot_add_vertex(dg, sink);
    dot_set_attribute_on_vertex(dg, sink, "label", sink_label);

    dot_add_edge(dg, source, sink);
    char buf[64];
    int16_t weight = genome_connection_weight(c);
    float weight_f = scale_to_float(weight, INT16_MIN, INT16_MAX, -4.0, 4.0);
    sprintf(buf, "%4.3f", weight_f);
    dot_set_attribute_on_edge(dg, source, sink, "xlabel", buf);
    float h = weight >= 0 ? 0.333 : 0.0;
    float s = scale_to_float(weight >= 0 ? weight : -weight, 0, INT16_MAX, 0.5, 1.0);
    float v = 1.0;
    sprintf(buf, "\"%4.3f,%4.3f,%4.3f\"", h, s, v);
    dot_set_attribute_on_edge(dg, source, sink, "color", buf);

    float penwidth = scale_to_float(weight >= 0 ? weight : -weight, 0, INT16_MAX, 1.0, 8.0);
    sprintf(buf, "%4.3f", penwidth);
    dot_set_attribute_on_edge(dg, source, sink, "penwidth", buf);
}

dgraph_t* viz_dump_creature(struct creature* c) {
    dgraph_t* dg = dot_create("creature brain");
    dot_set_directed(dg, true);
    dot_set_attribute(dg, "splines", "curved");
    for(size_t i = 0; i < c->genome.n_connections; i++) {
        connection_to_dot(dg, c->genome.connections[i]);
    }
    return dg;
}
