#!/bin/bash

# 获取当前脚本的路径
script_dir=$(dirname "$(realpath "$0")")

echo "当前脚本所在的路径是: $script_dir"

# 先把 helloplayer 模块 hap 包构建出来
# 构建方式: 选中 helloplayer 目录,点击顶部 build-> BuildHap(s)
ohpm publish "$script_dir/build/default/outputs/default/helloplayer.har" --publish_registry https://ohpm.openharmony.cn/ohpm