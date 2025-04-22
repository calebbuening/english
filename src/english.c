#include "../include/english.h"
#include "../include/config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <json-c/json.h>

// Structure to store response data from API calls
typedef struct {
    char *data;
    size_t size;
} response_data_t;

// Global verbose flag
static bool verbose_mode = false;

// Default Ollama endpoint
static char ollama_endpoint[1024] = "http://localhost:11434/api/generate";

// Callback function for CURL to handle response data
static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t real_size = size * nmemb;
    response_data_t *resp = (response_data_t *)userp;
    
    char *ptr = realloc(resp->data, resp->size + real_size + 1);
    if (ptr == NULL) {
        fprintf(stderr, "Error: Out of memory\n");
        return 0;
    }
    
    resp->data = ptr;
    memcpy(&(resp->data[resp->size]), contents, real_size);
    resp->size += real_size;
    resp->data[resp->size] = '\0';
    
    return real_size;
}

bool english_init(void) {
    // Initialize CURL
    curl_global_init(CURL_GLOBAL_DEFAULT);
    
    // Initialize configuration
    return config_init();
}

bool english_config_set_key(const char *key_value) {
    return config_set_api_key(key_value);
}

const char *english_config_get_key(void) {
    return config_get_api_key();
}

bool english_config_set_model(const char *model_value) {
    return config_set_model(model_value);
}

const char *english_config_get_model(void) {
    return config_get_model();
}

void english_set_verbose(bool verbose) {
    verbose_mode = verbose;
}

bool english_is_verbose(void) {
    return verbose_mode;
}

void english_set_ollama_endpoint(const char *endpoint) {
    if (endpoint != NULL) {
        strncpy(ollama_endpoint, endpoint, sizeof(ollama_endpoint) - 1);
        ollama_endpoint[sizeof(ollama_endpoint) - 1] = '\0';
    }
}

const char *english_get_ollama_endpoint(void) {
    return ollama_endpoint;
}

bool english_compile(const char *english_text, const char *target_language, 
                     char *output, size_t output_size) {
    if (english_text == NULL || target_language == NULL || output == NULL || output_size == 0) {
        return false;
    }
    
    // Get the model name (no API key needed for Ollama)
    const char *model_name = config_get_model();
    
    if (verbose_mode) {
        fprintf(stderr, "Verbose mode: Using Ollama model: %s\n", model_name);
        fprintf(stderr, "Verbose mode: Using Ollama endpoint: %s\n", ollama_endpoint);
    }
    
    // Initialize CURL
    CURL *curl = curl_easy_init();
    if (curl == NULL) {
        fprintf(stderr, "Error: Could not initialize CURL\n");
        return false;
    }
    
    // Set up the request
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    
    // Create the request payload for Ollama
    json_object *request = json_object_new_object();
    
    // Add model
    json_object *model = json_object_new_string(model_name);
    json_object_object_add(request, "model", model);
    
    // Create the prompt with system and user message combined
    char prompt[8192];
    snprintf(prompt, sizeof(prompt), 
             "You are a compiler that translates English to %s code. IMPORTANT: Generate ONLY code with NO explanations, comments, or any other text.\n\n"
             "Your response must ONLY contain valid %s code and nothing else. Do not include any explanations before or after the code.\n\n"
             "Translate the following English description into %s code:\n\n%s\n\nCode:", 
             target_language, target_language, target_language, english_text);
    
    json_object *prompt_obj = json_object_new_string(prompt);
    json_object_object_add(request, "prompt", prompt_obj);
    
    // Add temperature parameter
    json_object *temperature = json_object_new_double(0.1);
    json_object_object_add(request, "temperature", temperature);
    
    // Add stream parameter (false for non-streaming response)
    json_object *stream = json_object_new_boolean(0);
    json_object_object_add(request, "stream", stream);
    
    // Convert the request to a string
    const char *request_str = json_object_to_json_string(request);
    
    if (verbose_mode) {
        fprintf(stderr, "Verbose mode: Request payload: %s\n", request_str);
    }
    
    // Set up CURL options for Ollama
    curl_easy_setopt(curl, CURLOPT_URL, ollama_endpoint);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_str);
    
    // Set up the response data
    response_data_t response_data;
    response_data.data = malloc(1);
    response_data.size = 0;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response_data);
    
    // Perform the request
    if (verbose_mode) {
        fprintf(stderr, "Verbose mode: Sending request to Ollama API...\n");
    }
    
    CURLcode res = curl_easy_perform(curl);
    bool success = false;
    
    if (res != CURLE_OK) {
        fprintf(stderr, "Error: CURL request failed: %s\n", curl_easy_strerror(res));
    } else {
        if (verbose_mode) {
            fprintf(stderr, "Verbose mode: Received response from Ollama API\n");
        }
        if (verbose_mode) {
            fprintf(stderr, "Verbose mode: Raw response: %s\n", response_data.data);
        }
        
        // Parse the response from Ollama
        json_object *response = json_tokener_parse(response_data.data);
        if (response == NULL) {
            fprintf(stderr, "Error: Could not parse JSON response\n");
        } else {
            // Get the response content directly (Ollama format is different from OpenAI)
            json_object *response_content;
            if (json_object_object_get_ex(response, "response", &response_content)) {
                // Copy the content to the output buffer
                const char *content_str = json_object_get_string(response_content);
                
                // Extract code from markdown code blocks or after 'Code:' marker
                char *code_start = NULL;
                char *code_end = NULL;
                
                // Check for markdown code block format: ```language
                // followed by code and then closing ```
                char *markdown_start = strstr(content_str, "```");
                if (markdown_start) {
                    // Find the end of the language specifier line
                    char *newline = strchr(markdown_start + 3, '\n');
                    if (newline) {
                        // Start of actual code is after the newline
                        code_start = newline + 1;
                        
                        // Find the closing code block marker
                        code_end = strstr(code_start, "```");
                        if (code_end) {
                            // Create a temporary buffer to hold just the code
                            size_t code_length = code_end - code_start;
                            char *temp_code = malloc(code_length + 1);
                            if (temp_code) {
                                // Copy just the code part
                                strncpy(temp_code, code_start, code_length);
                                temp_code[code_length] = '\0';
                                
                                // Copy to output
                                strncpy(output, temp_code, output_size - 1);
                                free(temp_code);
                            } else {
                                // Memory allocation failed, fall back to using the whole content
                                strncpy(output, content_str, output_size - 1);
                            }
                        } else {
                            // No closing marker, use from code_start to the end
                            strncpy(output, code_start, output_size - 1);
                        }
                    } else {
                        // No newline after code block marker, fall back to whole content
                        strncpy(output, content_str, output_size - 1);
                    }
                } else {
                    // No markdown code block, check for 'Code:' marker
                    code_start = strstr(content_str, "Code:");
                    if (code_start) {
                        // Move past the 'Code:' prefix
                        code_start += 5;  // Length of 'Code:'
                        // Skip any leading whitespace
                        while (*code_start && (*code_start == ' ' || *code_start == '\n' || *code_start == '\t' || *code_start == '\r')) {
                            code_start++;
                        }
                        strncpy(output, code_start, output_size - 1);
                    } else {
                        // No code markers found, use the whole response
                        strncpy(output, content_str, output_size - 1);
                    }
                }
                
                output[output_size - 1] = '\0';
                success = true;
                
                if (verbose_mode) {
                    fprintf(stderr, "Verbose mode: Successfully parsed response\n");
                }
            } else {
                // Check for error message
                json_object *error;
                if (json_object_object_get_ex(response, "error", &error)) {
                    const char *error_str = json_object_get_string(error);
                    fprintf(stderr, "Error from Ollama: %s\n", error_str);
                    
                    // Provide more helpful error message for common errors
                    if (strstr(error_str, "model not found") != NULL) {
                        fprintf(stderr, "The model '%s' is not available in your Ollama installation.\n", model_name);
                        fprintf(stderr, "Try setting a different model with 'english set model MODEL_NAME'\n");
                        fprintf(stderr, "Common Ollama models include: llama3, codellama, mistral, gemma\n");
                    }
                } else {
                    fprintf(stderr, "Error: Unexpected response format from Ollama\n");
                }
            }
            
            json_object_put(response);
        }
    }
    
    // Clean up
    free(response_data.data);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    json_object_put(request);
    
    return success;
}

void english_cleanup(void) {
    // Clean up configuration
    config_cleanup();
    
    // Clean up CURL
    curl_global_cleanup();
}
