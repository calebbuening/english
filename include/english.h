#ifndef ENGLISH_H
#define ENGLISH_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Initialize the English compiler
 * @return true if initialization was successful, false otherwise
 */
bool english_init(void);

/**
 * @brief Set the API key for the AI service
 * @param key_value The API key to use
 * @return true if the key was set successfully, false otherwise
 */
bool english_config_set_key(const char *key_value);

/**
 * @brief Get the API key for the AI service
 * @return The API key, or NULL if not set
 */
const char *english_config_get_key(void);

/**
 * @brief Set the model for the Ollama service
 * @param model_value The model to use (e.g., 'llama3', 'codellama', 'mistral')
 * @return true if the model was set successfully, false otherwise
 */
bool english_config_set_model(const char *model_value);

/**
 * @brief Get the model for the Ollama service
 * @return The model, or NULL if not set (will default to 'llama3')
 */
const char *english_config_get_model(void);

/**
 * @brief Set verbose mode for detailed output
 * @param verbose true to enable verbose mode, false to disable
 */
void english_set_verbose(bool verbose);

/**
 * @brief Check if verbose mode is enabled
 * @return true if verbose mode is enabled, false otherwise
 */
bool english_is_verbose(void);

/**
 * @brief Set the Ollama endpoint URL
 * @param endpoint The URL of the Ollama API endpoint
 */
void english_set_ollama_endpoint(const char *endpoint);

/**
 * @brief Get the current Ollama endpoint URL
 * @return The URL of the Ollama API endpoint
 */
const char *english_get_ollama_endpoint(void);

/**
 * @brief Compile English text to the target programming language
 * @param english_text The English description of the code to generate
 * @param target_language The target programming language (e.g., "python", "javascript")
 * @param output Buffer to store the generated code
 * @param output_size Size of the output buffer
 * @return true if compilation was successful, false otherwise
 */
bool english_compile(const char *english_text, const char *target_language, 
                     char *output, size_t output_size);

/**
 * @brief Clean up resources used by the English compiler
 */
void english_cleanup(void);

#endif /* ENGLISH_H */
