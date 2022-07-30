import getopt
import os
import sys

from lib.mysign import my_sign


def show_help():
    print("-h                       查看帮助")
    print("-s <path>                bin文件路径         例: E:\\folk.bin")
    print("-o <path>                dfu文件输出路径     例: D:\\")
    print("-v <version num>         版本号              例: 0.0.1.0")
    print("-t <type>                类型                例: factory或user或engineer")
    print("-k                       查看秘钥            例: ")
    print("-c                       创建秘钥            例: ")
    print("完整例子: xxx.exe -s E:\\folk.bin -o D:\\ -v 0.0.1.0 -t factory")


if __name__ == "__main__":
    # print(sys.argv)
    py_path = os.path.dirname(sys.argv[0])

    mysign = my_sign(py_path)

    sour_path = None
    out_path = None
    pack_version = None
    pack_type = None

    try:
        opts, args = getopt.getopt(sys.argv[1:], '-h-s:-o:-v:-t:-k-c',
                                   ['help', 'source=', 'out=', 'version=', 'type=', 'key', 'newkey'])
    except Exception:
        print("参数错误!")
        sys.exit()

    for opt_name, opt_value in opts:
        if opt_name in ('-h', '--help'):
            show_help()
            sys.exit()
        elif(opt_name in ('-c', '--newkey')):
            mysign.create_key()
        elif(opt_name in ('-s', '--source')):
            sour_path = opt_value
        elif(opt_name in ('-o', '--out')):
            out_path = opt_value
        elif(opt_name in ('-v', '--version')):
            pack_version = opt_value
        elif(opt_name in ('-t', '--type')):
            pack_type = opt_value
        elif(opt_name in ('-k', '--key')):
            mysign.show_key()
            sys.exit()

    # print(sour_path)
    if sour_path is not None:
        data = mysign.get_data(sour_path)
    else:
        sys.exit()
    # print("sour_path:", sour_path)

    if len(data) != 0:
        size = os.path.getsize(sour_path)
        md5_val = mysign.md5_opt1(data)
        # 得到输出路径
        out_path = mysign.get_out_path(sour_path, out_path)
        print("out path:", out_path)
        # 得到头信息
        head_info = mysign.get_head_info(
            md5_val, size, pack_version, pack_type)
        # 写出dfu文件
        with open(out_path, 'wb') as f:
            f.write(head_info + data)
            f.close()

    else:
        print("err: Invalid parameter")
