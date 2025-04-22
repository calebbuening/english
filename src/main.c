#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/english.h"

#define MAX_INPUT_SIZE 4096
#define MAX_OUTPUT_SIZE 8192

static void print_usage(void) {
    printf("Usage: english <command> [options]\n\n");
    printf("Commands:\n");
    printf("  set model MODEL        Set the model to use (e.g., llama3, codellama, mistral, gemma)\n");
    printf("  set endpoint URL       Set the Ollama API endpoint URL\n");
    printf("  get model              Get the current model\n");
    printf("  get endpoint           Get the current Ollama API endpoint URL\n");
    printf("  compile LANGUAGE       Compile English to the specified programming language\n");
    printf("\n");
    printf("Options:\n");
    printf("  -v, --verbose          Enable verbose mode for debugging\n");
    printf("\n");
    printf("Options for 'compile':\n");
    printf("  -f, --file FILE        Read English description from a file\n");
    printf("  -o, --output FILE      Write output to a file (default: stdout)\n");
}

static int handle_set_endpoint(const char *endpoint) {
    if (!english_init()) {
        fprintf(stderr, "Error: Could not initialize English compiler\n");
        return 1;
    }
    
    english_set_ollama_endpoint(endpoint);
    printf("Ollama endpoint set to: %s\n", endpoint);
    english_cleanup();
    return 0;
}

static int handle_get_endpoint(void) {
    if (!english_init()) {
        fprintf(stderr, "Error: Could not initialize English compiler\n");
        return 1;
    }
    
    const char *endpoint = english_get_ollama_endpoint();
    printf("Current Ollama endpoint: %s\n", endpoint);
    
    english_cleanup();
    return 0;
}

static int handle_set_model(const char *model_value) {
    if (!english_init()) {
        fprintf(stderr, "Error: Could not initialize English compiler\n");
        return 1;
    }
    
    if (english_config_set_model(model_value)) {
        printf("Model set to '%s' successfully.\n", model_value);
        english_cleanup();
        return 0;
    } else {
        fprintf(stderr, "Error: Failed to set model\n");
        english_cleanup();
        return 1;
    }
}

static int handle_get_model(void) {
    if (!english_init()) {
        fprintf(stderr, "Error: Could not initialize English compiler\n");
        return 1;
    }
    
    const char *model = english_config_get_model();
    printf("Current model: %s\n", model);
    
    english_cleanup();
    return 0;
}

static int handle_compile(const char *target_language, const char *input_file, const char *output_file, bool verbose) {
    if (!english_init()) {
        fprintf(stderr, "Error: Could not initialize English compiler\n");
        return 1;
    }
    
    // Set verbose mode if requested
    english_set_verbose(verbose);
    
    if (verbose) {
        fprintf(stderr, "Verbose mode: Using Ollama for code generation\n");
    }
    
    // Read input from file or stdin
    char input_buffer[MAX_INPUT_SIZE];
    FILE *input_fp = stdin;
    
    if (input_file != NULL) {
        input_fp = fopen(input_file, "r");
        if (input_fp == NULL) {
            fprintf(stderr, "Error: Could not open input file %s\n", input_file);
            english_cleanup();
            return 1;
        }
    } else {
        printf("Enter English description (Ctrl+D to end):\n");
    }
    
    size_t input_size = 0;
    char line[1024];
    while (fgets(line, sizeof(line), input_fp) != NULL && input_size < MAX_INPUT_SIZE - 1) {
        size_t line_len = strlen(line);
        if (input_size + line_len >= MAX_INPUT_SIZE - 1) {
            break;
        }
        strcpy(input_buffer + input_size, line);
        input_size += line_len;
    }
    input_buffer[input_size] = '\0';
    
    if (input_file != NULL) {
        fclose(input_fp);
    }
    
    // Compile the English text to code
    char output_buffer[MAX_OUTPUT_SIZE];
    if (!english_compile(input_buffer, target_language, output_buffer, sizeof(output_buffer))) {
        fprintf(stderr, "Error: Failed to compile English to %s\n", target_language);
        english_cleanup();
        return 1;
    }
    
    // Write output to file or stdout
    FILE *output_fp = stdout;
    if (output_file != NULL) {
        output_fp = fopen(output_file, "w");
        if (output_fp == NULL) {
            fprintf(stderr, "Error: Could not open output file %s\n", output_file);
            english_cleanup();
            return 1;
        }
    }
    
    fprintf(output_fp, "%s\n", output_buffer);
    
    if (output_file != NULL) {
        fclose(output_fp);
    }
    
    english_cleanup();
    return 0;
}

int main(int argc, char *argv[]) {
    // Check if we have enough arguments
    if (argc < 2) {
        print_usage();
        return 1;
    }
    
    // Process global options first
    bool verbose = false;
    int i;
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose = true;
            // Remove this option from argv by shifting all subsequent elements
            for (int j = i; j < argc - 1; j++) {
                argv[j] = argv[j + 1];
            }
            argc--;
            i--; // Reprocess this position with the next argument
        }
    }
    
    // Handle set commands
    if (argc >= 2 && strcmp(argv[1], "set") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Error: Missing set parameter (key or model)\n");
            return 1;
        }
        
        // Handle 'set endpoint' command
        if (argc >= 3 && strcmp(argv[2], "endpoint") == 0) {
            if (argc < 4) {
                fprintf(stderr, "Error: Missing endpoint URL\n");
                return 1;
            }
            return handle_set_endpoint(argv[3]);
        }
        
        // Handle 'set model' command
        if (argc >= 3 && strcmp(argv[2], "model") == 0) {
            if (argc < 4) {
                fprintf(stderr, "Error: Missing model value\n");
                return 1;
            }
            return handle_set_model(argv[3]);
        }
        
        fprintf(stderr, "Error: Unknown set parameter '%s'\n", argv[2]);
        print_usage();
        return 1;
    }
    
    // Handle get commands
    if (argc >= 2 && strcmp(argv[1], "get") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Error: Missing get parameter (key or model)\n");
            return 1;
        }
        
        // Handle 'get endpoint' command
        if (strcmp(argv[2], "endpoint") == 0) {
            return handle_get_endpoint();
        }
        
        // Handle 'get model' command
        if (strcmp(argv[2], "model") == 0) {
            return handle_get_model();
        }
        
        fprintf(stderr, "Error: Unknown get parameter '%s'\n", argv[2]);
        print_usage();
        return 1;
    }
    
    // Handle 'compile' command
    if (strcmp(argv[1], "compile") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Error: Missing target language\n");
            return 1;
        }
        
        const char *target_language = argv[2];
        const char *input_file = NULL;
        const char *output_file = NULL;
        
        // Parse options
        for (int i = 3; i < argc; i++) {
            if ((strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--file") == 0) && i + 1 < argc) {
                input_file = argv[++i];
            } else if ((strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) && i + 1 < argc) {
                output_file = argv[++i];
            } else {
                fprintf(stderr, "Error: Unknown option %s\n", argv[i]);
                return 1;
            }
        }
        
        return handle_compile(target_language, input_file, output_file, verbose);
    }
    
    // Unknown command
    fprintf(stderr, "Error: Unknown command %s\n", argv[1]);
    print_usage();
    return 1;
}
