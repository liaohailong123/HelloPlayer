# iOS集成经验

## 前提条件,已经编译好 iPhoneOS 和 iPhoneSimulator 两个平台的 arm64 架构的静态库

## 新建framework,把静态库拖进来,头文件可以不拖,然后在 framework 的 build settings -> linking-general -> mach-os type 设置为 static library

## 分别生成两个 framework 
    Debug-iphoneos/xxx.framework
    Debug-iphonesimulator/xxx.framework
    在 Headers 里面,手动拷贝所有静态库的头文件

## 使用 xcodebuild 命令 -create-xcframework 生成 xcframework,把两个平台的framework合成一个xcframework

## 另外创建一个 framework, 把上面生成的xcframework 拷贝进去

## 然后在 framework 里面,手动在 build settings 里面,配置 Search Paths -> Headers Search Paths -> 添加Headers路径,选择 recursive 

## 然后建一个 app 模块, 依赖 framework 模块, 同样也配置 Search Paths -> Headers Search Paths -> 添加Headers路径,选择 recursive 


## 新建c++代码之后,需要注意文件内部不要写错代码(报错不明显),否则会导致整体编译
    class Hello {}; 最后这个;号一定要注意写上
    
    
## iOS工程里面,如果想用 reference 的方式依赖外部目录,需要先 New Group 这样 add files 操作才会有 reference 选项出来, New Folder 是只有 move 和 copy 两个选项.


## 右键点击项目的 xcodeproject 文件,选择查看包内容,然后看 project.pbxproj 里面可以搜索到 依赖库的路径

## 在framework工程中编写metal着色器需要注意:
    .metal文件是否成功编译出 default.metallib,通过看 Show the report navigator 编译结果
    default.metallib 一般文件在 .../Products/Debug-iphonesimulator/xxx.framework/default.metallib 
    然后需要在app工程 Target -> General -> Frameworks,Librarys,and Embedded Content 中把framework工程调整成 Embed & Sign
    否则 app 工程编译时不会把 framework 的资源带上!
        [this->device newDefaultLibrary] 这句代码会找app模块里面自带的.metal产物,framework的.metal产物需要手动指定路径来new出 id <MTLLibrary>
