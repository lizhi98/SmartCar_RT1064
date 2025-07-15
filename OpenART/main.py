import sensor, time
import tf
from machine import UART

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_brightness(1000)
sensor.skip_frames(time=20)
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(True, (0, 0x80, 0))

clock = time.clock()

# 定义分类模型和数字识别模型标签文件的路径
model_object_path = '/sd/model_object_2.tflite' # 定义分类模型的路径
label_object_path = '/sd/label_object.txt' # 定义分类标签的路径
model_number_path = '/sd/model_number_1.tflite' # 定义数字识别模型的路径
label_number_path = '/sd/label_number.txt' # 定义数字识别标签的路径

# 先加载分类模型和数字识别模型的标签
labels_object = [line.rstrip() for line in open(label_object_path)] # 加载分类标签
labels_number = [line.rstrip() for line in open(label_number_path)] # 加载数字识别标签
model_object = tf.load(model_object_path, load_to_fb=True) # 加载分类模型
model_number = tf.load(model_number_path, load_to_fb=True) # 加载数字识别模型

# 延时5秒，等待RT1064
time.sleep(5)

# 打开串口
uart_rt1064 = UART(2, baudrate=9600)

def classify(model, img, labels):
    result_objs = tf.classify(model, img, min_scale=1.0, scale_mul=0.5, x_overlap=0.0, y_overlap=0.0)
    result_obj = result_objs[0]
    sorted_results = sorted(zip(labels, result_obj.output()), key=lambda x: x[1], reverse=True)
    return sorted_results

def print_results(results, n=3):
    for cat, prob in results[0:n]:
        print(f'{cat:13}: {prob:.2f}')

class Serial():
    def __init__(self, uart, debug=False):
        self.uart = uart
        self.debug = debug

    def set_object_result(self, result):
        cat, prob = result
        cat_id = labels_object.index(cat)
        self.object_cat_id = f'{cat_id:02}'
        self.object_prob = f'{prob:.2f}'

    def set_number_result(self, result):
        cat, prob = result
        self.number_cat_id = f'{cat:>02}'
        self.number_prob = f'{prob:.2f}'

    def set_number_placeholder(self):
        self.number_cat_id = '00'
        self.number_prob = '0.00'

    def send_frame(self):
        frame = f'o:{self.object_cat_id},p:{self.object_prob},n:{self.number_cat_id},p:{self.number_prob}'
        # self.uart.write(f'[{frame}]')
        self.uart.write('[')
        time.sleep(0.1)
        self.uart.write(f'{frame}')
        time.sleep(0.1)
        self.uart.write(']')
        if self.debug:
            print(frame)

serial = Serial(uart_rt1064)

while True:
    img = sensor.snapshot()
    img_area = img

    object_results = classify(model_object, img_area, labels=labels_object)
    first_result_object = object_results[0]
    serial.set_object_result(first_result_object)
    # print('=' * 20)
    # print_results(object_results, n=5)

    if first_result_object[0] == 'number':
        number_results = classify(model_number, img_area, labels=labels_number)
        serial.set_number_result(number_results[0])
        # print('-' * 20)
        # print_results(number_results, n=3)
    else:
        serial.set_number_placeholder()

    serial.send_frame()

'''
# 将矩形框内的图像使用训练好的模型进行分类
# tf.classify()将在图像的roi上运行网络(如果没有指定roi，则在整个图像上运行)
# 将为每个位置生成一个分类得分输出向量。
# 在每个比例下，检测窗口都以x_overlap（0-1）和y_overlap（0-1）为指导在ROI中移动。
# 如果将重叠设置为0.5，那么每个检测窗口将与前一个窗口重叠50%。
# 请注意，重叠越多，计算工作量就越大。因为每搜索/滑动一次都会运行一下模型。
# 最后，对于在网络沿x/y方向滑动后的多尺度匹配，检测窗口将由scale_mul（0-1）缩小到min_scale（0-1）。
# 下降到min_scale(0-1)。例如，如果scale_mul为0.5，则检测窗口将缩小50%。
# 请注意，如果x_overlap和y_overlap较小，则在较小的比例下可以搜索更多区域...
'''
