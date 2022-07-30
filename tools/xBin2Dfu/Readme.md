##Compile Command
```
使用脚本时，需要安装第三方库
环境：      python 3.6.8
编译：      .\make.bat main.py
安装命令:   pip install xxx
依赖：rsa, 其余根据提示安装。

使用帮助：
-h                       查看帮助
-s <path>                bin文件路径         例: E:\folk.bin
-o <path>                dfu文件输出路径     例: D:\
-v <version num>         版本号              例: 0.0.1.0
-t <type>                类型                例: factory或user或engineer
-k                       查看秘钥            例:
-c                       创建秘钥            例:
完整例子: xxx.exe -s E:\folk.bin -o D:\ -v 0.0.1.0 -t factory

钥匙路径：.\xBin2Dfu\key\
(生成的钥匙目录里面会含有keys.h文件，请放置指定地点使用。例：/utils/rsa/)


两种使用方式：
第一种，脚本运行。(需要python环境并安装第三方RSA库)。
路径：xBox/Makefile
使用方式：
IMAGE_DFU: $(IMAGE_BIN)
	@python $(PWD)/tools/xBin2Dfu/xBin2Dfu.py -s $(KBUILD_OUTPUT)/$(T).bin -o $(KBUILD_OUTPUT) -v $(REVISION_INFO_S) -t $(REVISION_TYPE_S)

第二种，exe运行(无需环境)。
路径：xBox/Makefile
IMAGE_DFU: $(IMAGE_BIN)
	@$(PWD)/tools/xBin2Dfu/xBin2Dfu.exe -s $(KBUILD_OUTPUT)/$(T).bin -o $(KBUILD_OUTPUT) -v $(REVISION_INFO_S) -t $(REVISION_TYPE_S)
```
