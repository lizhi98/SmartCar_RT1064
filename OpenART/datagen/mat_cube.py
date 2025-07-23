'''
从绿色背景中抠出箱子图像
'''

from os import path

from utils import log, load_images

# 常量
script_path = path.dirname(path.abspath(__file__))
cubes_path = path.join(script_path, '../data/cubes')
cubes_matted_path = path.join(script_path, '../data/cubes_matted')

# 加载有背景的箱子图像
cubes_images = load_images(cubes_path)
log('Loaded', f'{len(cubes_images)} cube images')

# 像素映射
k_green = 0.6
k_lux = -0.05
threshold = 0

type RGBA = tuple[int, int, int, int]
def mat_map(rgba: RGBA) -> RGBA:
    r, g, b, _ = rgba
    return (0, 0, 0, 0) if (g * 2 - r - b) * k_green + (r + g + b) * k_lux > threshold else rgba

# 抠出箱子图像
for i, image in enumerate(cubes_images):
    # 转化为 RGBA 模式
    image = image.convert('RGBA')
    # 应用像素映射
    image.putdata(list(map(mat_map, image.getdata())))

    # 保存抠出后的图像
    filename = path.join(cubes_matted_path, f'{i}.png')
    image.save(filename)
    log('Matted', filename)
