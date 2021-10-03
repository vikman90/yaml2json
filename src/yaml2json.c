// August 4, 2018

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <yaml2json.h>

#define merror(format, ...) fprintf(stderr, "ERROR: " format "\n", ##__VA_ARGS__)
#define minfo(format, ...) fprintf(stderr, "INFO: " format "\n", ##__VA_ARGS__)
#define mwarn(format, ...) fprintf(stderr, "WARNING: " format "\n", ##__VA_ARGS__)

static cJSON * yaml2json_node(yaml_document_t * document, yaml_node_t * node);

int yaml_parse_stdin(yaml_document_t * document) {
    yaml_parser_t parser;
    int error = -1;

    yaml_parser_initialize(&parser);
    yaml_parser_set_input_file(&parser, stdin);

    if (yaml_parser_load(&parser, document)) {
        error = 0;
    } else {
        mwarn("Failed to load YAML document at line %lu", parser.problem_mark.line);
    }

    yaml_parser_delete(&parser);

    return error;
}

int yaml_parse_file(const char * path, yaml_document_t * document) {
    yaml_parser_t parser;
    FILE * finput;
    int error = -1;

    if (finput = fopen(path, "rb"), finput) {
        yaml_parser_initialize(&parser);
        yaml_parser_set_input_file(&parser, finput);

        if (yaml_parser_load(&parser, document)) {
            error = 0;
        } else {
            mwarn("Failed to load YAML document in %s:%lu", path, parser.problem_mark.line);
        }

        yaml_parser_delete(&parser);
    } else {
        mwarn("Cannot open file '%s': %s (%d)", path, strerror(errno), errno);
    }

    fclose(finput);
    return error;
}

cJSON * yaml2json(yaml_document_t * document) {
    yaml_node_t * node;

    if (node = yaml_document_get_root_node(document), !node) {
        mwarn("No document defined.");
        return NULL;
    }

    return yaml2json_node(document, node);
}

cJSON * yaml2json_node(yaml_document_t * document, yaml_node_t * node) {
    yaml_node_t * key;
    yaml_node_t * value;
    yaml_node_item_t * item_i;
    yaml_node_pair_t * pair_i;
    double number;
    char * scalar;
    char * end;
    cJSON * object;

    switch (node->type) {
    case YAML_NO_NODE:
        object = cJSON_CreateObject();
        break;

    case YAML_SCALAR_NODE:
        scalar = (char *)node->data.scalar.value;
        number = strtod(scalar, &end);
        object = (end == scalar || *end) ? cJSON_CreateString(scalar) : cJSON_CreateNumber(number);
        break;

    case YAML_SEQUENCE_NODE:
        object = cJSON_CreateArray();

        for (item_i = node->data.sequence.items.start; item_i < node->data.sequence.items.top; ++item_i) {
            cJSON_AddItemToArray(object, yaml2json_node(document, yaml_document_get_node(document, *item_i)));
        }

        break;

    case YAML_MAPPING_NODE:
        object = cJSON_CreateObject();

        for (pair_i = node->data.mapping.pairs.start; pair_i < node->data.mapping.pairs.top; ++pair_i) {
            key = yaml_document_get_node(document, pair_i->key);
            value = yaml_document_get_node(document, pair_i->value);

            if (key->type != YAML_SCALAR_NODE) {
                mwarn("Mapping key is not scalar (line %lu).", key->start_mark.line);
                continue;
            }

            cJSON_AddItemToObject(object, (char *)key->data.scalar.value, yaml2json_node(document, value));
        }

        break;

    default:
        mwarn("Unknown node type (line %lu).", node->start_mark.line);
        object = NULL;
    }

    return object;
}
