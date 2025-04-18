import os

def Settings(**kwargs):
    return{
            'errorformat': '%f:%l:%c:%m',
            'flags':['-x', 'c++', '-std=c++20', '-g', '-Wall', '-Wextra', '-Wpedantic', '-Werror', '-Wno-unused-parameter', '-Wno-unused-private-field', '-Wno-unused-variable', '-Iinclude/', '-Ivendor/include', '-I/usr/include/bullet'],
            }
