def Settings( **kwargs ):
    return {
            'flags':['-x', 'c', '-Wall', '-Wextra', '-I./src/include', '-I./src/include/platform/lf_os',
                     '-I./src/inlcude/platform/linux', '-Wno-unused-function']
            }

