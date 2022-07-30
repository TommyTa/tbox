import hashlib
import os
import sys
import time
from datetime import datetime

import rsa


class my_sign():

    pubkey = ""
    privkey = ""

    def __init__(self, path):
        self.py_path = path
        self.pubkey_path = self.py_path + '\\key\\public.pem'
        self.privkey_path = self.py_path + '\\key\\private.pem'
        self.keys_path = self.py_path + '\\key\\keys.h'
        self.read_key()
        rsa.key.find_p_q = self.find_p_q  # 替换库函数

    # 重写RSA库，使得秘钥长度为256或512
    def find_p_q(self, nbits, getprime_func, accurate):
        total_bits = nbits * 2
        shift = 0  # 取消偏移，使长度为256
        pbits = nbits + shift
        qbits = nbits - shift
        # Choose the two initial primes
        p = getprime_func(pbits)
        q = getprime_func(qbits)

        def is_acceptable(p: int, q: int):
            if p == q:
                return False

            if not accurate:
                return True

            # Make sure we have just the right amount of bits
            found_size = rsa.common.bit_size(p * q)
            return total_bits == found_size

        # Keep choosing other primes until they match our requirements.
        change_p = False
        while not is_acceptable(p, q):
            # Change p on one iteration and q on the other
            if change_p:
                p = getprime_func(pbits)
            else:
                q = getprime_func(qbits)

            change_p = not change_p

        # We want p > q as described on
        return max(p, q), min(p, q)

    def get_data(self, path):
        data = []
        if(path.find(".bin") > -1):
            with open(path, 'rb') as f:
                data = f.read()
                f.close()
        return data

    def hex_output(self, bytes_s):
        i = 0
        s = ""
        for data in bytes_s:
            if(i >= 16):
                i = 0
                s = s[:-1] + "\n"
            i = i + 1
            s = s + '0x' + ('%02X' % data) + ", "
        return s[:-2]

    def buff_output(self, input_s, buf_head):
        input_s = input_s.replace("\n", "\n    ")
        input_s = "    " + input_s
        input_s = "const static unsigned char " + \
            buf_head + "[] = {\n" + input_s + "};"

        return input_s

    def get_out_path(self, src_path, out_path):
        if out_path is not None:
            file_path = out_path
            # print("file_path", file_path)
            if((file_path.endswith('\\') is not True) and file_path.find('\\') > 0):
                file_path = file_path + '\\'
            if((file_path.endswith('/') is not True) and file_path.find('/') > 0):
                file_path = file_path + '/'
            filename = file_path + os.path.split(src_path)[1]
        else:
            filename = src_path
        filename = filename.replace(".bin", ".dfu")
        return filename

    def get_head_info(self, md5_val, file_size, version, type):
        hrad_info = []
        file_size = file_size.to_bytes(4, byteorder='big', signed=False)
        # print(file_size)
        version = version.split(".")
        version = list(map(int, version))
        version = bytes(version)
        if(type == "factory"):
            type = bytes([0])
        elif(type == "engineer"):
            type = bytes([1])
        elif(type == "user"):
            type = bytes([2])
        else:
            type = bytes([255])
        # print(type, 16 - len(file_size + version) - 1)
        null_bytes = bytes(16 - len(file_size + version) - 1)
        # print(null_bytes)

        # 读取RSA钥匙
        if(self.privkey or self.privkey):
            self.read_key()

        # 用RSA2048签名MD5(私钥签名)
        crypto = self.privkey_sign(md5_val)
        if(len(crypto) < 1):
            print("私钥签名失败")
        hrad_info = crypto + file_size + version + type + null_bytes
        return hrad_info

    # md5采用加盐方式签名
    def md5_opt(self, data, salt):
        obj = hashlib.md5(salt.encode('utf-8'))
        obj.update(data)
        print("MD5:", obj.digest().hex())
        return obj.digest()

    # md5常规签名
    def md5_opt1(self, data):
        data_sha = hashlib.md5(data).digest()
        print("MD5:", data_sha.hex())
        return data_sha

    # 创建一个新RSA钥匙
    def create_key(self):
        start = datetime.now()
        try:
            try:
                # 备份当前秘钥
                if os.path.exists(self.pubkey_path) == True:
                    os.rename(self.pubkey_path, self.pubkey_path + '.' +
                              str(int(time.time())) + ".bak")

                if os.path.exists(self.privkey_path) == True:
                    os.rename(self.privkey_path, self.privkey_path + '.' +
                              str(int(time.time())) + ".bak")

                if os.path.exists(self.keys_path) == True:
                    os.rename(self.keys_path, self.keys_path + '.' +
                              str(int(time.time())) + ".bak")
            except Exception:
                print("备份秘钥失败！")
                print(self.pubkey_path)
                print(self.privkey_path)
                print(self.keys_path)
            try:
                # 生成密钥
                (self.pubkey, self.privkey) = rsa.newkeys(2048)
            except Exception:
                print("生成秘钥失败！")

            # 保存密钥
            with open(self.pubkey_path, 'wb') as f:
                f.write(self.pubkey.save_pkcs1())
                f.close()

            with open(self.privkey_path, 'wb') as f:
                f.write(self.privkey.save_pkcs1())
                f.close()
            try:
                self.generateh_h_file()
            except Exception:
                print("生成H文件秘钥失败！")

        except Exception:
            print("秘钥创建失败！")
        print("创建秘钥用时:" + str((datetime.now() - start)))

    # 读取RSA钥匙
    def read_key(self):
        try:
            with open(self.pubkey_path, 'rb') as f:
                self.pubkey = rsa.PublicKey.load_pkcs1(f.read())
                f.close()
        except Exception:
            print("公钥秘钥读取错误，可能不存在。")
        try:
            with open(self.privkey_path, 'rb') as f:
                self.privkey = rsa.PrivateKey.load_pkcs1(f.read())
                f.close()
        except Exception:
            print("私钥秘钥读取错误，可能不存在。")

    # 查看 RSA钥匙
    def show_key(self):
        try:
            # 读取RSA钥匙
            if(self.privkey or self.privkey):
                self.read_key()
            print("公钥:", self.pubkey.hex())
            print("私钥:", self.privkey.hex())
        except Exception:
            print("秘钥读取错误，可能不存在。")

    # 生成秘钥H文件
    def generateh_h_file(self):
        out_s = "#ifndef __KEY_RAS_H__\n"
        out_s = out_s + "#define __KEY_RAS_H__\r\n"
        # n --> modulus rsa.
        out_buff = self.hex_output(rsa.transform.int2bytes(self.privkey.n))
        out_s = out_s + self.buff_output(out_buff, "key_m")
        out_s = out_s + "\r\n"
        # e --> public Exponent
        out_buff = self.hex_output(rsa.transform.int2bytes(self.privkey.e))
        out_s = out_s + self.buff_output(out_buff, "key_e")

        out_s = out_s + "\r\n"
        # d --> private Exponent
        out_buff = self.hex_output(rsa.transform.int2bytes(self.privkey.d))
        out_s = out_s + self.buff_output(out_buff, "key_ex")

        out_s = out_s + "\r\n"
        # p --> prime1
        out_buff = self.hex_output(rsa.transform.int2bytes(self.privkey.p))
        out_s = out_s + self.buff_output(out_buff, "key_p1")

        out_s = out_s + "\r\n"
        # q --> prime2
        out_buff = self.hex_output(rsa.transform.int2bytes(self.privkey.q))
        out_s = out_s + self.buff_output(out_buff, "key_p2")

        out_s = out_s + "\r\n"
        # exp1 --> exponent1
        out_buff = self.hex_output(rsa.transform.int2bytes(self.privkey.exp1))
        out_s = out_s + self.buff_output(out_buff, "key_e1")

        out_s = out_s + "\r\n"
        # exp2 --> exponent2
        out_buff = self.hex_output(rsa.transform.int2bytes(self.privkey.exp2))
        out_s = out_s + self.buff_output(out_buff, "key_e2")

        out_s = out_s + "\r\n"
        # coef --> coefficient
        out_buff = self.hex_output(rsa.transform.int2bytes(self.privkey.coef))
        out_s = out_s + self.buff_output(out_buff, "key_c")

        out_s = out_s + "\r\n#endif\n"
        with open(self.keys_path, 'w') as f:
            f.write(out_s)
            f.close()

    # RSA使用私钥签名
    def privkey_sign(self, data):
        keylength = rsa.common.byte_size(self.privkey.n)
        padded = rsa.pkcs1._pad_for_signing(data, keylength)
        payload = rsa.transform.bytes2int(padded)
        encrypted = self.privkey.blinded_encrypt(payload)
        block = rsa.transform.int2bytes(encrypted, keylength)
        return block

    # RSA使用公钥解签
    def pubkey_decrypt(self, data):
        keylength = rsa.common.byte_size(self.pubkey.n)
        encrypted = rsa.transform.bytes2int(data)
        decrypted = rsa.core.decrypt_int(
                    encrypted, self.pubkey.e, self.pubkey.n)
        text = rsa.transform.int2bytes(decrypted, keylength)
        try:
            if len(text) > 0:
                if(text[0] == 0 and text[1] == 1):
                    text = text[2:]
                    for i in range(0, len(text)):
                        if(text[i] != 255):
                            return text[i+1:]
            return 0
        except Exception:
            print("公钥解签失败!")

    # RSA使用公钥签名
    def pubkey_sign(self, data):
        crypto = rsa.encrypt(data, self.pubkey)
        return crypto

    # RSA使用私钥钥解签
    def privkey_decrypt(self, data):
        message = rsa.decrypt(data, self.privkey)
        return message
