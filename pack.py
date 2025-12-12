import os

# Configuration
EXTENSIONS = {'.h', '.hpp', '.cpp', '.c', '.cc'}
IGNORE_DIRS = {'cmake-build-debug', '.git', '.vscode', 'build', 'bin' , "cmake-build-debug-event-trace","cmake-build-debug", "test" }
OUTPUT_FILE = 'codebase.txt'

def pack_files():
    with open(OUTPUT_FILE, 'w', encoding='utf-8') as outfile:
        for root, dirs, files in os.walk("."):
            # Filter out ignored directories
            dirs[:] = [d for d in dirs if d not in IGNORE_DIRS]
            
            for file in files:
                if any(file.endswith(ext) for ext in EXTENSIONS):
                    filepath = os.path.join(root, file)
                    
                    # Write Header
                    outfile.write(f"\n{'='*50}\n")
                    outfile.write(f"FILE PATH: {filepath}\n")
                    outfile.write(f"{'='*50}\n\n")
                    
                    # Write Content
                    try:
                        with open(filepath, 'r', encoding='utf-8') as infile:
                            outfile.write(infile.read())
                    except Exception as e:
                        outfile.write(f"[Error reading file: {e}]")
                    
                    outfile.write("\n")

if __name__ == "__main__":
    pack_files()
    print(f"Done! Code saved to {OUTPUT_FILE}")