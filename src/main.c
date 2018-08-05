// August 4, 2018

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <yaml2json.h>

int main(int argc, char ** argv) {
    yaml_document_t document;
    cJSON * object;
    char * json;

    if (argc < 2) {
        fprintf(stderr, "Syntax: %s <file> | -", argv[0]);
        return EXIT_FAILURE;
    }

    if (!strcmp(argv[1], "-")) {
        if (yaml_parse_stdin(&document)) {
            return EXIT_FAILURE;
        }
    } else {
        if (yaml_parse_file(argv[1], &document)) {
            return EXIT_FAILURE;
        }
    }

    if (object = yaml2json(&document), !object) {
        return EXIT_FAILURE;
    }

    yaml_document_delete(&document);
    json = cJSON_Print(object);
    cJSON_Delete(object);
    puts(json);
    free(json);

    return EXIT_SUCCESS;
}
