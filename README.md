# yaml2json
YAML to JSON converter

This project aims to easily convert any YAML document to JSON, using the [cJSON library](https://github.com/DaveGamble/cJSON) by Dave Gamble.

## How to compile

```sh
make deps
make
```

## Running project

- Parsing a YAML file: `bin/yaml2json example.yml`
- Parsing standard input: `cat example.yml | bin/yaml2json -`
