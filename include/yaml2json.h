// August 4, 2018

#ifndef YAML2JSON_H
#define YAML2JSON_H

#include <yaml.h>
#include <cJSON.h>

int yaml_parse_stdin(yaml_document_t * document);
int yaml_parse_file(const char * path, yaml_document_t * document);
cJSON * yaml2json(yaml_document_t * document);

#endif
