import os
import glob
from collections import defaultdict
from azure.ai.inference import ChatCompletionsClient
from azure.ai.inference.models import SystemMessage, UserMessage
from azure.core.credentials import AzureKeyCredential
from dotenv import load_dotenv

load_dotenv()

# API Configuration
API_ENDPOINT = "https://models.github.ai/inference"
AI_MODEL_NAME = "xai/grok-3"
API_TOKEN = os.environ["GITHUB_TOKEN"]

# Project Paths
PROJECT_ROOT = "../orgChartApi"
TEST_OUTPUT_DIR = "../orgChartApi/generated_tests"

def load_prompt_template(template_filename: str) -> str:
    """Load a YAML prompt template from the prompts directory."""
    prompt_path = os.path.join("prompts", template_filename)
    with open(prompt_path, "r") as prompt_file:
        return prompt_file.read()

def generate_test_with_ai(prompt_template: str, source_code: str) -> str:
    """Generate test code using the AI model."""
    print(f"Generating tests for code block of length {len(source_code)}...")
    
    ai_client = ChatCompletionsClient(
        endpoint=API_ENDPOINT,
        credential=AzureKeyCredential(API_TOKEN),
    )
    
    ai_response = ai_client.complete(
        messages=[
            SystemMessage("You are an expert C++ developer and test writer."),
            UserMessage(f"{prompt_template}\n\n---\n\n{source_code}\n\n---\n\n# Output only the test code.")
        ],
        temperature=0.2,
        top_p=1.0,
        model=AI_MODEL_NAME
    )
    
    print("Test generation completed successfully.")
    return ai_response.choices[0].message.content

def collect_source_files(source_dir: str) -> dict:
    """Group source files by their base name without extension."""
    source_files = []
    file_extensions = ("*.cpp", "*.cc", "*.h", "*.hpp")
    
    for extension in file_extensions:
        source_files.extend(glob.glob(os.path.join(source_dir, extension)))
    
    file_groups = defaultdict(list)
    for file_path in source_files:
        base_name = os.path.splitext(os.path.basename(file_path))[0]
        file_groups[base_name].append(file_path)
    
    print(f"Found {len(file_groups)} unique file(s) in {source_dir}: {list(file_groups.keys())}")
    return file_groups

def save_test_file(component_name: str, test_content: str) -> str:
    """Save generated test code to a file."""
    test_file_path = os.path.join(TEST_OUTPUT_DIR, f"test_{component_name}.cpp")
    with open(test_file_path, "w", encoding="utf-8") as test_file:
        test_file.write(test_content)
    print(f"Test file written: {test_file_path}")
    return test_file_path

def process_component_tests(source_dir: str, prompt_template_file: str, component_type: str) -> None:
    """Generate tests for a specific component type (controllers, models, utils)."""
    print(f"\n[Step] Generating unit tests for {component_type}...")
    
    prompt_template = load_prompt_template(prompt_template_file)
    source_file_groups = collect_source_files(source_dir)
    
    for base_name, source_files in source_file_groups.items():
        print(f"Generating test for: {base_name} (from {source_files})")
        
        combined_source = ""
        for source_file in source_files:
            with open(source_file, "r", encoding="utf-8", errors="replace") as file:
                combined_source += f"\n// File: {os.path.basename(source_file)}\n"
                combined_source += file.read()
        
        test_code = generate_test_with_ai(prompt_template, combined_source)
        save_test_file(base_name, test_code)
    
    print(f"[Step] Test generation for {component_type} complete.\n")

def main() -> None:
    """Main entry point for the test generator."""
    print("=== Unit Test Generator: Controllers, Models, Utils (Grok-3, grouped) ===")
    os.makedirs(TEST_OUTPUT_DIR, exist_ok=True)

    # Generate tests for each component type
    components = [
        {
            "dir": os.path.join(PROJECT_ROOT, "controllers"),
            "template": "initial.yaml",
            "type": "controllers"
        },
        {
            "dir": os.path.join(PROJECT_ROOT, "models"),
            "template": "initial.yaml",
            "type": "models"
        },
        {
            "dir": os.path.join(PROJECT_ROOT, "utils"),
            "template": "initial.yaml",
            "type": "utils"
        }
    ]

    for component in components:
        process_component_tests(
            source_dir=component["dir"],
            prompt_template_file=component["template"],
            component_type=component["type"]
        )

if __name__ == "__main__":
    main()