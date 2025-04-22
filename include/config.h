#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

/**
 * @brief Initialize the configuration system
 * @return true if initialization was successful, false otherwise
 */
bool config_init(void);

/**
 * @brief Set the API key for the AI service
 * @param key_value The API key to use
 * @return true if the key was set successfully, false otherwise
 */
bool config_set_api_key(const char *key_value);

/**
 * @brief Get the API key for the AI service
 * @return The API key, or NULL if not set
 */
const char *config_get_api_key(void);

/**
 * @brief Set the model for the Ollama service
 * @param model_value The model to use (e.g., 'llama3', 'codellama', 'mistral')
 * @return true if the model was set successfully, false otherwise
 */
bool config_set_model(const char *model_value);

/**
 * @brief Get the model for the Ollama service
 * @return The model, or NULL if not set (will default to 'llama3')
 */
const char *config_get_model(void);

/**
 * @brief Clean up resources used by the configuration system
 */
void config_cleanup(void);

#endif /* CONFIG_H */
