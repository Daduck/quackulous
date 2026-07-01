import os
import re

filepath = r"C:\dev\quackulous\src\renderergl2\tr_extensions.c"

with open(filepath, 'r', encoding='utf-8') as f:
    content = f.read()

def repl(match):
    core = match.group(1)
    arb = match.group(2)
    if core == arb and core.startswith("gl"):
        return f'GLimp_GetProcAddressWithFallback("{core}", "{core}ARB")'
    return match.group(0)

new_content = re.sub(r'GLimp_GetProcAddressWithFallback\("([^"]+)",\s*"([^"]+)"\)', repl, content)

with open(filepath, 'w', encoding='utf-8') as f:
    f.write(new_content)

print("Done")
