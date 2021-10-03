// August 4, 2018

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <yaml2json.h>

int main(int argc, char ** argv) {
    yaml_parser_t parser;
    cJSON * object;

    if (argc < 2) {
        fprintf(stderr, "Syntax: %s <file> | -", argv[0]);
        return EXIT_FAILURE;
    }

    yaml_parser_initialize(&parser);
    FILE * finput = strcmp(argv[1], "-") == 0 ? stdin : fopen(argv[1], "rb");

    if (finput == NULL) {
        fprintf(stderr, "ERROR: Cannot open file '%s': %s (%d)", argv[1], strerror(errno), errno);
        yaml_parser_delete(&parser);
        return EXIT_FAILURE;
    }

    yaml_parser_set_input_file(&parser, finput);

    if (object = yaml2json(&parser), !object) {
        return EXIT_FAILURE;
    }

    char * json = cJSON_Print(object);
    puts(json);

    free(json);
    cJSON_Delete(object);
    yaml_parser_delete(&parser);

    if (finput != stdin) {
        fclose(finput);
    }

    return EXIT_SUCCESS;
}
