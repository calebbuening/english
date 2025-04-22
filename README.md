# ENGLISH: the programming language

*Compile .eng files into any other language*

## Prerequisites

Before building the compiler, you need to install the following dependencies:

- libcurl (for HTTP requests)
- libjson-c (for JSON parsing)
- Ollama (for local AI model execution)

On macOS, you can install these dependencies using Homebrew:

```bash
brew install curl json-c
brew install ollama
```

After installing Ollama, start the service and pull a model:

```bash
ollama serve
ollama pull codellama  # or any other model you prefer
```

## Installation

```bash
# Clone the repository
git clone <repository-url>
cd english

# Build the project
make

# Install the binary (optional)
sudo make install
```

## Usage

### Setting up your Ollama Model

Before using the compiler, you should set up your preferred Ollama model:

```bash
english set model codellama  # or llama3, mistral, etc.
```

By default, the compiler will connect to Ollama at http://localhost:11434/api/generate. If your Ollama instance is running elsewhere, you can set the endpoint:

```bash
english set endpoint http://your-server:11434/api/generate
```

### Compiling ENGLISH

You can compile English descriptions to code in several ways:

1. From a file:

```bash
english compile python --file input.txt --output output.py
```

2. From standard input:

```bash
echo "Create a function that calculates the factorial of a number" | english compile python
```

3. Interactive mode:

```bash
english compile javascript
# Then type your description and press Ctrl+D when done
```

The compiler will send your request to the local Ollama instance, which will generate code based on your description.

### Verbose Mode

For debugging purposes, you can enable verbose mode with the `-v` or `--verbose` flag:

```bash
english -v compile python --file input.txt
```

This will print detailed information about the compilation process, including API requests and responses.

## Supported Languages

The compiler supports various programming languages including:
- Python
- JavaScript
- Java
- C++
- Go
- Ruby
- And more...

## How It Works

The English-to-code compiler uses Ollama's local AI models to translate natural language descriptions into executable code. The quality of the generated code depends on the clarity and specificity of your English description and the capabilities of the model you've selected.

Ollama runs entirely on your local machine, so there's no need for API keys or internet connectivity once you've downloaded the models.
