'''
叠加 MNIST 数字图像、箱子图像和场地图像
'''

import os
import time
from os import path
from random import randint
from typing import cast
from itertools import product

from tensorflow.keras.datasets import mnist
from PIL import Image, ImageOps
import numpy as np
from numpy.typing import NDArray

from utils import log, load_images

# 常量
script_path = path.dirname(path.abspath(__file__))
fields_path = path.join(script_path, '../data/fields')
cubes_path = path.join(script_path, '../data/cubes_matted')
output_path = path.join(script_path, '../data/fields_numbers')

mnist_width = 28
mnist_size = (mnist_width,) * 2
mnist_target_width = mnist_width * 5
mnist_target_size = (mnist_target_width,) * 2
mnist_gap = -30

output_width = 320
output_height = 240
output_size = (output_width, output_height)

mnist_style_count = 10
mnist_angle_unit = 45

mnist_lux_threshold = 125

# 补全 MNIST 类型
type MnistBitmap = NDArray[np.uint8]
type MnistLabel = np.uint8
type MnistDataset = tuple[MnistBitmap, MnistLabel]
type MnistData = tuple[MnistDataset, MnistDataset]

# 工具函数
def generate_number_image(mnist_bitmap: MnistBitmap) -> Image.Image:
    '''从 MNIST 位图生成透明背景的 pillow 数字图像'''
    # 从 MNIST 位图创建 pillow 图像
    mnist_image = Image.fromarray(mnist_bitmap)
    # 放大到目标大小
    mnist_image = mnist_image.resize(mnist_target_size)
    # 反色
    mnist_image = ImageOps.invert(mnist_image).point(lambda l: 255 if l > mnist_lux_threshold else l)
    # 生成 alpha 通道
    mnist_alpha = mnist_image.point(lambda l: 0 if l == 255 else 255)
    return mnist_image, mnist_alpha

def random_mnist_offset(mnist_bitmaps: list[MnistBitmap]) -> int:
    return randint(0, len(mnist_bitmaps) - mnist_style_count)

# 加载 MNIST 数据并分组
(mnist_bitmaps, mnist_labels), _ = cast(MnistData, mnist.load_data())
mnist_bitmap_groups: dict[int, list[MnistBitmap]] = { label: [] for label in range(10) }
for mnist_bitmap, mnist_label in zip(mnist_bitmaps, mnist_labels):
    mnist_bitmap_groups[mnist_label].append(mnist_bitmap)
mnist_group_size = len(mnist_bitmap_groups[0])
log('Loaded', f'{len(mnist_bitmaps)} MNIST images')

# 加载所有场地图像
fields_images = load_images(fields_path)
log('Loaded', f'{len(fields_images)} field images')

# 加载所有箱子图像
cubes_images = load_images(cubes_path, extname='.png')
log('Loaded', f'{len(cubes_images)} cube images')

# 清空目标目录
removed_count = 0
for n in range(0, 100):
    output_n_path = path.join(output_path, str(n))
    if path.exists(output_n_path):
        for filename in os.listdir(output_n_path):
            os.remove(path.join(output_n_path, filename))
            removed_count += 1
    else:
        os.mkdir(output_n_path)
log('Removed', f'{removed_count} entries')

# 计算要生成的图像数量
n_fields = len(fields_images)
n_cubes = len(cubes_images)
n_number = 100
n_styles = mnist_style_count
n_angles = 360 // mnist_angle_unit
n_total = n_fields * n_cubes * n_number * n_styles * n_angles
total_images_digit = len(str(n_total))
log('Generating', f'{n_fields=} * {n_cubes=} * {n_number=} * {n_styles=} * {n_angles=} = {n_total} images')

# 为 0-99 的每个数生成
i_total = 0
start_time = time.time()
for n in range(100):
    # 获取数字的每一位与位数
    digits = [ int(d) for d in str(n) ]
    n_digits = len(digits)
    # 计算箱子+数字图像的大小
    cube_number_width = mnist_target_width * n_digits + mnist_gap * (n_digits - 1)
    cube_number_height = mnist_target_width
    number_size = (cube_number_width, cube_number_height)
    number_pos = ((output_width - cube_number_width) // 2, (output_height - cube_number_height) // 2)
    # 获取数字对应的 MNIST 位图组
    mnist_bitmaps_list = [ mnist_bitmap_groups[digit] for digit in digits ]
    # 生成随机偏移量以选择 MNIST 位图
    mnist_bitmap_offsets = [ random_mnist_offset(mnist_bitmaps) for mnist_bitmaps in mnist_bitmaps_list ]
    # 叠加数字+箱子+场地，为每个场地图像生成
    for (i_field, field_image), (i_cube, cube_image), i_style, angle in product(
        enumerate(fields_images),
        enumerate(cubes_images),
        range(mnist_style_count),
        range(0, 360, mnist_angle_unit)
    ):
        # 复制场地图像作为原图
        output_image = field_image.copy()
        # 新建用于叠放箱子+数字的空白图像
        cube_number_image = Image.new('RGBA', output_size)
        # 新建数字图像
        number_image = Image.new('RGBA', number_size)
        # 依次生成并粘贴每个数字的 MNIST 图像
        pos_x = 0
        for i_digit, mnist_bitmaps in enumerate(mnist_bitmaps_list):
            # 获取当前数字的 MNIST 位图
            mnist_bitmap = mnist_bitmaps[mnist_bitmap_offsets[i_digit] + i_style]
            # 生成数字图像
            digit_image, digit_alpha = generate_number_image(mnist_bitmap)
            # 粘贴到箱子+数字图像上
            number_image.paste(digit_image, (pos_x, 0), mask=digit_alpha)
            # 更新下一个数字的位置
            pos_x += mnist_target_width + mnist_gap
        # 将箱子图像粘贴到箱子+数字图像上
        cube_number_image.paste(cube_image, mask=cube_image)
        # 将数字图像粘贴到箱子+数字图像上
        cube_number_image.paste(number_image, number_pos, mask=number_image)
        # 随机旋转箱子+数字图像
        cube_number_image = cube_number_image.rotate(angle)
        # 将箱子+数字图像粘贴到场地图像上
        output_image.paste(cube_number_image, mask=cube_number_image)
        # 保存生成的图像
        target_image_path = f'{n}/F{i_field}_C{i_cube}_S{i_style}_A{angle}.jpg'
        output_image.save(path.join(output_path, target_image_path))
        # 计算速度与剩余时间
        curr_time = time.time()
        i_total += 1
        speed = i_total / (curr_time - start_time)
        eta = (n_total - i_total) / speed
        # 显示进度
        progress = i_total / n_total
        log('Generated', f'{target_image_path:20} {i_total:>{total_images_digit}}/{n_total} {progress:.2%} speed={speed:.2f}/s eta={eta:.2f}s     ')
        print('\x1B[1A', end='')

