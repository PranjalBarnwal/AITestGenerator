# 🚀 Keploy: AI-Powered C++ Test Generator

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++: 17](https://img.shields.io/badge/C++-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![Platform: Linux/Win](https://img.shields.io/badge/Platform-Linux%2FWin-orange.svg)](https://github.com/yourusername/keploy)

> Supercharge your C++ testing workflow with AI-generated unit tests. Powered by Grok-3 LLM.

Keploy automatically generates comprehensive unit tests for your C++ codebase, focusing on controllers, models, and utilities. It leverages state-of-the-art AI to create meaningful tests while maintaining high code quality and coverage.

## ✨ Features

- 🤖 **AI-Powered Generation**: Uses Grok-3 LLM for intelligent test creation
- 🎯 **Smart Coverage**: Generates tests targeting critical code paths
- 🔄 **Automated Workflow**: Seamless integration with your build process
- 📊 **Coverage Reports**: Built-in support for code coverage analysis
- 🛠️ **Framework Support**: Works with Google Test and Drogon Framework
- 🔌 **Extensible**: Easy to customize for different testing needs

## 🚀 Quick Start

### Prerequisites

- C++17 compatible compiler
- CMake 3.10+
- Python 3.8+
- Git

### Installation

1. **Clone the Repository**
   ```bash
   git clone https://github.com/pranjalbarnwal/AITestGeneratorKeploy.git
   cd keploy
   ```

2. **Install Dependencies**

   **Ubuntu/Debian:**
   ```bash
   # C++ tools
   sudo apt-get update
   sudo apt-get install -y build-essential cmake git
   
   # Testing frameworks
   sudo apt-get install -y libgtest-dev libgmock-dev
   sudo apt-get install -y libdrogon-dev
   
   # Coverage tools
   sudo apt-get install -y lcov
   
   # Python dependencies
   pip install azure-ai-inference python-dotenv
   ```

   **Windows:**
   ```powershell
   # Using vcpkg
   vcpkg install gtest:x64-windows
   vcpkg install drogon:x64-windows
   
   # Python dependencies
   pip install azure-ai-inference python-dotenv
   ```

3. **Configure Environment**
   ```bash
   # Create .env file
   echo "GITHUB_TOKEN=your_token_here" > testgen/.env
   ```

## 💻 Usage

### Generate Tests

```bash
cd testgen
python main.py
```

### Build and Run Tests

```bash
cd ../orgChartApi/generated_tests
mkdir build && cd build
cmake ..
make
./runTests
```

### Generate Coverage Report

```bash
# From the build directory
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage_report
```

## 📁 Project Structure

```
keploy/
├── orgChartApi/               # Main C++ project
│   ├── controllers/           # API Controllers
│   ├── models/               # Data Models
│   ├── utils/                # Utility Functions
│   └── generated_tests/      # Generated Test Files
│       ├── test_*.cpp        # Individual Test Files
│       └── CMakeLists.txt    # Test Build Configuration
├── testgen/                  # Test Generator Tool
│   ├── main.py              # Generator Script
│   ├── prompts/             # LLM Prompt Templates
│   │   ├── initial.yaml     # Base Prompts
│   │   └── refine.yaml      # Refinement Prompts
│   └── .env                 # Environment Configuration
└── README.md                # This File
```

## 🔧 Configuration

### Prompt Templates

Customize test generation by modifying YAML templates in `testgen/prompts/`:

```yaml
# initial.yaml example
instruction: |
  Generate a comprehensive unit test suite for the following C++ code.
  Focus on:
  - Edge cases
  - Error conditions
  - Input validation
  ...
```

### Test Output

Generated tests are saved in `orgChartApi/generated_tests/` following the naming convention:
- Controllers: `test_*Controller.cpp`
- Models: `test_models_*.cpp`
- Utils: `test_*Utils.cpp`

## 📊 Test Coverage Report

### Overall Coverage Summary

| Component Type | Line Coverage | Branch Coverage | Function Coverage |
|---------------|---------------|-----------------|-------------------|
| Controllers   | 83%          | 78%            | 85%              |
| Models        | 87%          | 82%            | 89%              |
| Utils         | 79%          | 75%            | 82%              |
| **Overall**   | **84%**      | **79%**        | **86%**          |

### Detailed Coverage by Component

#### Controllers
```plaintext
AuthController.cc       ████████░░ 85% (256/301 lines)
DepartmentsController.cc ███████░░ 82% (164/200 lines)
LoginFilter.cc         ███████░░ 81% (97/120 lines)
```

#### Models
```plaintext
Department.cc          ████████░░ 88% (179/204 lines)
Job.cc                ████████░░ 87% (171/196 lines)
Person.cc             ████████░░ 89% (374/420 lines)
PersonInfo.cc         ████████░░ 86% (141/164 lines)
User.cc               ████████░░ 85% (165/194 lines)
```

#### Utils
```plaintext
utils.cc              ███████░░ 79% (138/175 lines)
```

### Coverage Highlights

- **High Coverage Areas**:
  - Model class core functionality: Strong coverage of essential operations
  - Basic authentication flows: Well-tested login/logout paths
  - Standard data validation: Good coverage of common cases
  - Basic error handling: Coverage of primary error paths

- **Areas for Improvement**:
  - Complex query methods in DepartmentsController
  - Advanced filtering scenarios in LoginFilter
  - Edge cases in utils.cc
  - Concurrent access patterns
  - Complex error handling scenarios
  - Performance edge cases
  - Resource cleanup paths

### Test Quality Metrics

- **Test Count**: 184 unit tests
- **Assertion Count**: 892 assertions
- **Mock Objects**: 12 mock classes
- **Test Categories**:
  - Basic functionality: 55%
  - Edge cases: 25%
  - Error conditions: 15%
  - Performance tests: 5%

## 🤝 Contributing

We welcome contributions! Please see our [Contributing Guide](CONTRIBUTING.md) for details.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📝 Best Practices

### Test Generation

- Keep prompt templates focused and specific
- Review generated tests for completeness
- Add custom test cases for complex scenarios
- Maintain test readability and maintainability

### Coverage Goals

- Aim for 80%+ line coverage
- Focus on critical business logic
- Include edge cases and error paths
- Document uncovered sections

## 🔍 Troubleshooting

### Common Issues

1. **Build Errors**
   ```bash
   # Regenerate specific test
   python testgen/main.py --file path/to/source.cpp
   ```

2. **Coverage Issues**
   ```bash
   # Generate detailed coverage report
   lcov --list coverage.info
   ```

3. **AI Generation Problems**
   - Check GitHub token validity
   - Verify prompt template syntax
   - Ensure source file readability

## 📚 API Reference

### Test Generator

```python
# Generate tests for specific component
process_component_tests(
    source_dir="path/to/component",
    prompt_template_file="template.yaml",
    component_type="models"
)
```

### Coverage Analysis

```bash
# Generate branch coverage report
lcov --capture --directory . \
     --output-file coverage.info \
     --rc lcov_branch_coverage=1
```
