import re
import sys

def main():
    extensions_file = r'C:\dev\quackulous\src\renderergl2\tr_extensions.c'
    local_h_file = r'C:\dev\quackulous\src\renderergl2\tr_local.h'

    with open(extensions_file, 'r') as f:
        content = f.read()
    
    # Match all `void (APIENTRY * qgl...)` and similar declarations in tr_extensions.c
    # Also handle GLuint, GLboolean, GLenum, etc.
    # Exclude static functions like qglIsShaderCore
    
    declarations = []
    lines = content.split('\n')
    for line in lines:
        if '(APIENTRY * qgl' in line and not line.strip().startswith('static') and not line.strip().startswith('//'):
            decl = 'extern ' + line.strip()
            # If it's missing a semicolon at the end, it might be split across lines, but in this file most are single line
            if decl.endswith(';'):
                declarations.append(decl)
            elif decl.endswith(','): # wait some are multi line!
                pass # I'll just rely on a regex to be safer
    
    # Let's use regex
    # Match type (APIENTRY * name)(args);
    matches = re.finditer(r'^([A-Za-z_]+)\s*\(APIENTRY\s*\*\s*(qgl[A-Za-z0-9_]+)\)\s*\([^)]*\);', content, re.MULTILINE)
    
    decls = []
    for match in matches:
        decls.append(f"extern {match.group(1)} (APIENTRY * {match.group(2)}) (/* args omitted */);")
        # actually, I can just grab the exact substring
        pass
    
    # better yet, grab all lines that define the pointers (between `#include "tr_local.h"` and `static GLboolean (APIENTRY * qglIsShaderCore)`)
    
    start_idx = content.find('void (APIENTRY * qglActiveTexture)')
    if start_idx == -1:
        start_idx = content.find('void            (APIENTRY * qglActiveTexture)')
    end_idx = content.find('static GLboolean (APIENTRY * qglIsShaderCore)')
    
    if start_idx != -1 and end_idx != -1:
        decls_block = content[start_idx:end_idx]
        
        # prepend 'extern ' to every line that has (APIENTRY *
        final_lines = []
        for line in decls_block.split('\n'):
            line = line.strip()
            if '(APIENTRY *' in line and not line.startswith('extern') and not line.startswith('static'):
                final_lines.append('extern ' + line)
            elif line:
                # might be continuation line
                final_lines.append(line)
        
        append_content = "\n// --- AUTO EXTRACTED FROM tr_extensions.c ---\n" + "\n".join(final_lines) + "\n"
        
        with open(local_h_file, 'a') as f:
            f.write(append_content)
        print("Success")
    else:
        print("Could not find bounds")

if __name__ == '__main__':
    main()
