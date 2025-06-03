#ifndef CONFIG_H_
#define CONFIG_H_

/** Maximum of 1024 entries in the config file */
#define CONFIG_MAX_ENTRIES 1024

/**
 * Contains a key/value pair from the configuration file
 */
typedef struct config_node {
    const char* key;
    void* value;
} config_node;

/**
 * @brief Parses a config file and creates the config array
 * @param filename The path to the configuration file (must not be NULL)
 * @return Pointer to array of config_node structures, or NULL on error
 * @note Caller is responsible for freeing the returned memory
 */
config_node*
config_parse(const char* filename);

#endif