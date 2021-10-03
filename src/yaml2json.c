// August 4, 2018

#include <stdlib.h>
#include <string.h>
#include <yaml2json.h>

static yaml_event_type_t _parse_get_event_type(yaml_parser_t * parser);
static cJSON * _parse_document(yaml_parser_t * parser);
static cJSON * _parse_node(yaml_parser_t * parser);
static cJSON * _parse_scalar(const yaml_event_t * event);
static cJSON * _parse_sequence(yaml_parser_t * parser);
static cJSON * _parse_mapping(yaml_parser_t * parser);

cJSON * yaml2json(yaml_parser_t * parser) {
    if (_parse_get_event_type(parser) != YAML_STREAM_START_EVENT) {
        return NULL;
    }

    cJSON * object = _parse_document(parser);

    if (_parse_get_event_type(parser) != YAML_STREAM_END_EVENT) {
        cJSON_Delete(object);
        return NULL;
    }

    return object;
}

yaml_event_type_t _parse_get_event_type(yaml_parser_t * parser) {
    yaml_event_t event;

    if (!yaml_parser_parse(parser, &event)) {
        return YAML_NO_EVENT;
    }

    yaml_event_type_t type = event.type;
    yaml_event_delete(&event);

    return type;
}

cJSON * _parse_document(yaml_parser_t * parser) {
    if (_parse_get_event_type(parser) != YAML_DOCUMENT_START_EVENT) {
        return NULL;
    }

    cJSON * object = _parse_node(parser);

    if (_parse_get_event_type(parser) != YAML_DOCUMENT_END_EVENT) {
        cJSON_Delete(object);
        return NULL;
    }

    return object;
}

cJSON * _parse_node(yaml_parser_t * parser) {
    yaml_event_t event;
    cJSON * object = NULL;

    if (!yaml_parser_parse(parser, &event)) {
        return NULL;
    }

    switch (event.type) {
    case YAML_ALIAS_EVENT:
        // TODO: Alias unsupported.
        break;

    case YAML_SCALAR_EVENT:
        object = _parse_scalar(&event);
        break;

    case YAML_SEQUENCE_START_EVENT:
        object = _parse_sequence(parser);
        break;

    case YAML_MAPPING_START_EVENT:
        object = _parse_mapping(parser);

    default:
        break;
    }

    yaml_event_delete(&event);
    return object;
}

cJSON * _parse_scalar(const yaml_event_t * event) {
    const char * value = (const char *)event->data.scalar.value;

    if (strcmp(value, "true") == 0) {
        return cJSON_CreateTrue();
    } else if (strcmp(value, "false") == 0) {
        return cJSON_CreateFalse();
    } else if (strcmp(value, "null") == 0) {
        return cJSON_CreateNull();
    } else {
        char * end;
        double number = strtod(value, &end);

        if (*end == '\0') {
            return cJSON_CreateNumber(number);
        } else {
            return cJSON_CreateString(value);
        }
    }
}

cJSON * _parse_sequence(yaml_parser_t * parser) {
    cJSON * array = cJSON_CreateArray();
    cJSON * object = _parse_node(parser);

    while (object != NULL) {
        cJSON_AddItemToArray(array, object);
        object = _parse_node(parser);
    }

    return array;
}

cJSON * _parse_mapping(yaml_parser_t * parser) {
    yaml_event_t event;

    if (!yaml_parser_parse(parser, &event)) {
        return NULL;
    }

    cJSON * object = cJSON_CreateObject();

    while (event.type != YAML_MAPPING_END_EVENT) {
        if (event.type != YAML_SCALAR_EVENT) {
            break;
        }

        cJSON_AddItemToObject(object, (const char *)event.data.scalar.value, _parse_node(parser));
        yaml_event_delete(&event);

        if (!yaml_parser_parse(parser, &event)) {
            break;
        }
    }

    yaml_event_delete(&event);
    return object;
}
