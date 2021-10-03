// August 4, 2018

#ifndef YAML2JSON_H
#define YAML2JSON_H

#include <yaml.h>
#include <cjson/cJSON.h>

cJSON * yaml2json(yaml_parser_t * parser);

#endif
