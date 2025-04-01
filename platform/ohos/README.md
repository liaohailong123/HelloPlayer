# 开发总结

## 如何创建 native 模块
    右键项目名称 new -> Module 选Static模块，这样才能与项目一起构建

## 如何编译c/c++运行到模拟器
    1，编写模块代码，在entry主模块添加依赖 参考：oh-package.json5
    2，在c/c++代码模块，打开 build-profile.json5 务必添加 abiFilters 指定编译产物CPU架构，否则部署模拟器会失败
```
  "buildOption": {
    "externalNativeOptions": {
      "path": "./src/main/cpp/CMakeLists.txt",
      "arguments": "",
      "cppFlags": "",
      "abiFilters": ["arm64-v8a","x86_64"] // 一定要手动加上 x86_64 架构 
    },
  },
```

## Napi 与 ArkTs 双向传递数据
    参考：https://gitee.com/harmonyos_samples/ComplexTypePass/blob/master/entry/src/main/cpp/napi_init.cpp
    ts中不能直接引用ets代码，解决方式：把需要引入的ets代码修改后缀为.ts

## 鸿蒙模块打HAP包推中央仓库
    参考官网: https://ohpm.openharmony.cn/#/cn/home