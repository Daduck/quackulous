import os
import re

directory = r"C:\dev\quackulous\src\renderergl2"

def process_file(filepath):
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()

    # Types
    content = content.replace("GLhandleARB", "GLuint")
    content = content.replace("GLcharARB", "GLchar")
    content = content.replace("GLsizeiptrARB", "GLsizeiptr")
    content = content.replace("GLintptrARB", "GLintptr")

    # Constants like GL_TEXTURE0_ARB -> GL_TEXTURE0
    content = re.sub(r'\b(GL_[A-Z0-9_]+)_ARB\b', r'\1', content)
    
    # Types like PFNGL...ARBPROC -> PFNGL...PROC
    content = re.sub(r'\b(PFNGL[A-Z0-9_]+)ARB(PROC)\b', r'\1\2', content)

    # Function pointers / functions ending in ARB
    # Specific ones with Object
    content = content.replace("CreateShaderObjectARB", "CreateShader")
    content = content.replace("CreateProgramObjectARB", "CreateProgram")
    content = content.replace("UseProgramObjectARB", "UseProgram")
    content = content.replace("DeleteObjectARB", "DeleteObject")
    content = content.replace("AttachObjectARB", "AttachObject")
    content = content.replace("DetachObjectARB", "DetachObject")
    content = content.replace("GetObjectParameterivARB", "GetObjectParameteriv")
    content = content.replace("GetAttachedObjectsARB", "GetAttachedObjects")
    content = content.replace("GetInfoLogARB", "GetInfoLog")

    # Catch-all for qgl.*ARB
    def qgl_repl(match):
        return match.group(1)
    content = re.sub(r'\b(qgl[A-Za-z0-9_]+?)ARB\b', qgl_repl, content)

    # We also need to fix strings passed to SDL_GL_GetProcAddress.
    # Like "glActiveTextureARB" -> "glActiveTexture"
    def str_repl(match):
        return '"' + match.group(1) + '"'
    content = re.sub(r'"(gl[A-Za-z0-9_]+?)ARB"', str_repl, content)

    # Same for Object replacements in strings
    content = content.replace('"glCreateShaderObject"', '"glCreateShader"')
    content = content.replace('"glCreateProgramObject"', '"glCreateProgram"')
    content = content.replace('"glUseProgramObject"', '"glUseProgram"')
    
    # And types with Object
    content = content.replace('PFNGLCREATESHADEROBJECTPROC', 'PFNGLCREATESHADERPROC')
    content = content.replace('PFNGLCREATEPROGRAMOBJECTPROC', 'PFNGLCREATEPROGRAMPROC')
    content = content.replace('PFNGLUSEPROGRAMOBJECTPROC', 'PFNGLUSEPROGRAMPROC')

    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(content)

for root, _, files in os.walk(directory):
    for file in files:
        if file.endswith('.c') or file.endswith('.h'):
            process_file(os.path.join(root, file))
print("Done!")
