import os
import shutil
import random

source_path = r'D:\WorkSpaces\eIQ\Train_OpenMV'
target_path = r'D:\WorkSpaces\eIQ\Train_OpenMV\image\train\number'
n = 476

def extract_and_mix_files():
    """
    从source_path中的0-99目录中各抽取n个文件，混合起来复制到target_path中
    """
    # 确保目标路径存在
    os.makedirs(target_path, exist_ok=True)
    
    all_selected_files = []
    
    # 遍历0-99目录
    for i in range(100):
        dir_name = str(i)
        source_dir = os.path.join(source_path, dir_name)
        
        # 检查目录是否存在
        if not os.path.exists(source_dir):
            print(f"目录 {source_dir} 不存在")
            exit(1)
        
        # 获取目录中的所有文件
        files = []
        for file in os.listdir(source_dir):
            file_path = os.path.join(source_dir, file)
            if os.path.isfile(file_path):
                files.append(file_path)

        # 检查文件数量是否足
        if len(files) < n:
            print(f"目录 {source_dir} 中的文件数量不足：{len(files)} < {n}")
            exit(1)

        # 抽取n个文件
        selected_files = random.sample(files, n)

        # 添加到总列表中，同时记录原始目录信息
        for file_path in selected_files:
            all_selected_files.append((file_path, dir_name))
    
    # 复制文件到目标目录
    print(f"开始复制 {len(all_selected_files)} 个文件到 {target_path}")
    
    for file_path, original_dir in all_selected_files:
        file_name = os.path.basename(file_path)
        
        new_file_name = f"{original_dir}_{file_name}"
        target_file_path = os.path.join(target_path, new_file_name)
        shutil.copy2(file_path, target_file_path)
    
    print(f"复制完成！总共复制了 {len(all_selected_files)} 个文件")

if __name__ == "__main__":
    print(f"源路径: {source_path}")
    print(f"目标路径: {target_path}")
    print(f"每个目录抽取文件数: {n}")
    
    extract_and_mix_files()
