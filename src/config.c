#include "../include/config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define CONFIG_DIR_NAME ".english"
#define CONFIG_FILE_NAME "config.txt"
#define MAX_PATH_LENGTH 1024
#define MAX_KEY_LENGTH 1024
#define MAX_MODEL_LENGTH 256
#define DEFAULT_MODEL "llama3"

static char config_dir[MAX_PATH_LENGTH];
static char config_file[MAX_PATH_LENGTH];
static char api_key[MAX_KEY_LENGTH];
static char model[MAX_MODEL_LENGTH];

static bool ensure_config_dir(void);
static bool load_config(void);
static bool save_config(void);

bool config_init(void) {
    // Get the home directory
    const char *home_dir = getenv("HOME");
    if (home_dir == NULL) {
        fprintf(stderr, "Error: Could not get home directory\n");
        return false;
    }
    
    // Construct the config directory path
    snprintf(config_dir, sizeof(config_dir), "%s/%s", home_dir, CONFIG_DIR_NAME);
    
    // Construct the config file path
    snprintf(config_file, sizeof(config_file), "%s/%s", config_dir, CONFIG_FILE_NAME);
    
    // Ensure the config directory exists
    if (!ensure_config_dir()) {
        return false;
    }
    
    // Load the configuration
    return load_config();
}

bool config_set_api_key(const char *key_value) {
    if (key_value == NULL) {
        return false;
    }
    
    // Copy the key value
    strncpy(api_key, key_value, sizeof(api_key) - 1);
    api_key[sizeof(api_key) - 1] = '\0';
    
    // Save the configuration
    return save_config();
}

const char *config_get_api_key(void) {
    return api_key[0] != '\0' ? api_key : NULL;
}

bool config_set_model(const char *model_value) {
    if (model_value == NULL) {
        return false;
    }
    
    // Copy the model value
    strncpy(model, model_value, sizeof(model) - 1);
    model[sizeof(model) - 1] = '\0';
    
    // Save the configuration
    return save_config();
}

const char *config_get_model(void) {
    return model[0] != '\0' ? model : DEFAULT_MODEL;
}

void config_cleanup(void) {
    // Nothing to clean up for now
}

static bool ensure_config_dir(void) {
    struct stat st;
    
    // Check if the directory exists
    if (stat(config_dir, &st) == 0) {
        // Check if it's a directory
        if (S_ISDIR(st.st_mode)) {
            return true;
        }
        
        fprintf(stderr, "Error: %s exists but is not a directory\n", config_dir);
        return false;
    }
    
    // Create the directory
    if (mkdir(config_dir, 0700) != 0) {
        fprintf(stderr, "Error: Could not create directory %s\n", config_dir);
        return false;
    }
    
    return true;
}

static bool load_config(void) {
    FILE *file = fopen(config_file, "r");
    if (file == NULL) {
        // It's okay if the file doesn't exist yet
        api_key[0] = '\0';
        model[0] = '\0';  // Default model will be used
        return true;
    }
    
    char line[MAX_KEY_LENGTH];
    char key[64];
    char value[MAX_KEY_LENGTH];
    
    // Initialize with empty values
    api_key[0] = '\0';
    model[0] = '\0';
    
    // Read each line of the config file
    while (fgets(line, sizeof(line), file) != NULL) {
        // Remove trailing newline if present
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
            len--;
        }
        
        // Skip empty lines
        if (len == 0) {
            continue;
        }
        
        // Parse key=value pairs
        if (sscanf(line, "%63[^=]=%[^\n]", key, value) == 2) {
            if (strcmp(key, "api_key") == 0) {
                strncpy(api_key, value, sizeof(api_key) - 1);
                api_key[sizeof(api_key) - 1] = '\0';
            } else if (strcmp(key, "model") == 0) {
                strncpy(model, value, sizeof(model) - 1);
                model[sizeof(model) - 1] = '\0';
            }
        }
    }
    
    fclose(file);
    return true;
}

static bool save_config(void) {
    FILE *file = fopen(config_file, "w");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open %s for writing\n", config_file);
        return false;
    }
    
    // Write the API key and model in key=value format
    fprintf(file, "api_key=%s\n", api_key);
    
    // Only write the model if it's set (non-empty)
    if (model[0] != '\0') {
        fprintf(file, "model=%s\n", model);
    }
    
    fclose(file);
    return true;
}
