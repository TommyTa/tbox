import getopt
import os
import sys


def show_help():
    print("-h                查看帮助")
    print("-s <path>         文件路径1               例: E:\\folk.bin")
    print("-t <path>         文件路径2               例: E:\\folk.bin")
    print("-o <path>         合成文件输出路径        例: D:\\xxx.xxx(如果不填该项，默认选择文件1路径下)")
    print("-u <hex>          文件1起始位置           例: 0x800")
    print("-p <hex>          文件2起始位置           例: 0x40000(如果不填该项，会自动追加到文件1末尾)")
    print("-i <hex>          文件1和文件2间隔长度     例: 0x10")
    print("完整例子: xxx.exe -s E:\\folk1.bin -u 0x10 -t E:\\folk2.bin -p 0x80000000 -o D:\\xxx.bin")
    print("如果 [文件1起始位置 + 文件1大小 > 文件2起始位置] 则 会发生文件1被截断且被覆盖")


def get_file_data(path):
    data = bytes()
    if(path.find(".") > -1):
        with open(path, 'rb') as f:
            data = f.read()
            f.close()
    return data


if __name__ == "__main__":
    # print(sys.argv)
    py_path = os.path.dirname(sys.argv[0])

    sour1_path = None
    sour2_path = None
    data = None
    data1 = None
    addr1 = "0"
    addr2 = "-1"
    out_path = None
    interval = None

    try:
        opts, args = getopt.getopt(sys.argv[1:], '-h-s:-t:-o:-u:-p:-i:',
                                   ['help', 'source1=', 'source2=', 'out=', 'addr1=', 'addr2=', 'interval='])
    except Exception:
        print("参数错误!")
        sys.exit()

    for opt_name, opt_value in opts:
        if opt_name in ('-h', '--help'):
            show_help()
            sys.exit()
        elif(opt_name in ('-s', '--source1')):
            sour1_path = opt_value
        elif(opt_name in ('-t', '--source2')):
            sour2_path = opt_value
        elif(opt_name in ('-o', '--out')):
            out_path = opt_value
        elif(opt_name in ('-u', '--addr1')):
            addr1 = opt_value
        elif(opt_name in ('-p', '--addr2')):
            addr2 = opt_value
        elif(opt_name in ('-i', '--interval')):
            interval = opt_value

    # 获取文件数据
    if sour1_path is not None:
        data = get_file_data(sour1_path)
    else:
        sys.exit()

    if sour2_path is not None:
        data1 = get_file_data(sour2_path)
    else:
        data1 = bytes()

    # 转换偏移地址
    if addr1 is not None:
        if addr1.find("0x") >= 0:
            addr1 = int(addr1, 16)
        else:
            addr1 = int(addr1)
        file_head_bytes = bytes(addr1)
    else:
        file_head_bytes = bytes()

    if addr2 is not None:
        if addr2.find("0x") >= 0:
            addr2 = int(addr2, 16)
        else:
            addr2 = int(addr2)

    # 生成两个文件之间的填充
    if interval is not None:
        if interval.find("0x") >= 0:
            interval = int(interval, 16)
        else:
            interval = int(interval)
        file_fill_bytes = bytes(interval)
    else:
        file_fill_bytes = bytes()

    # 组合文件数据
    out_data = bytes()
    # 写出文件
    if out_path is None:
        out_path = os.path.split(sour1_path)[0] + "\\new"

    with open(out_path, 'wb+') as f:
        f.write(file_head_bytes)
        f.write(data)
        f.close()
    # 判断是否有文件2
    if data1 is not None:
        if(len(data1) > 0):
            # 判断文件2写入位置
            if(addr2 > 0):
                with open(out_path, 'r+b') as f:
                    # 文件写入指针偏移
                    ret = f.seek(addr2, 0)
                    f.write(data1)
                    f.close()
            else:
                with open(out_path, 'a+b') as f:
                    # 判断是否填充东西
                    if(file_fill_bytes is not None):
                        f.write(file_fill_bytes)
                    f.write(data1)
                    f.close()
    print("out path:", out_path)
