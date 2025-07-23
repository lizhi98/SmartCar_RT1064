'''
工具函数
'''

import os
from os import path
from PIL import Image

def log(kind: str, message: str) -> None:
    '''打印日志'''
    print(f'\x1B[1;32m{kind:>10}\x1B[0m {message}')

def load_images(directory: str, extname: str = '.jpg') -> list[Image.Image]:
    '''加载指定目录中的所有图像'''
    return [ Image.open(path.join(directory, filename))
        for filename in os.listdir(directory)
        if filename.endswith(extname)
    ]