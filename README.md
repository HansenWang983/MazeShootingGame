# 迷宫射击游戏

 **CG final project, a simple first-person-shooter game in maze, based on OpenGL 3.3+** 



## 文件目录

- 3rd_party/ 包括已下载的第三方库。

  ```
  3rd_party/
  ----assimp/ 
  ----glad/
  ----glfw/
  ----glm/
  ```

  版本分别为

  - assimp-4.1.0
  - [glad](https://glad.dav1d.de/)（Language 设置为 **C/C++**，在API选项中，选择 **3.3** 以上的 OpenGL(gl) 版本，模式 (Profile) 设置为**Core**，并且选中**生成加载器** (Generate a loader) 选项 ）
  - glfw-3.3.bin.WIN32
  - glm-0.9.8.5

- configuration/ 中为 Cmake 需要的文件，用于配置项目的根目录，以加载 Shader 等文件。

- doc/ 存储项目文档，其中的 imgs 存放相关截图展示阶段性的效果。

- include/  包括本地的头文件，引用自 <https://github.com/JoeyDeVries/LearnOpenGL> 。

- resources/ 包括素材资源模型等文件，引用自 <https://github.com/JoeyDeVries/LearnOpenGL> 。

- src/ 包括源文件，其中 Shader/ 中为着色器文件。



## 安装环境依赖

- 目前只支持 Windows 10 
- 在项目根目录下新建 build/，使用 Cmake-gui 生成 VS 解决方案
- 将 `\build\3rd_party\assimp\include\assimp`中的 `config.h` 移动到 `3rd_party\assimp\include\assimp` 中
- 使用 VS 打开 `build/MazeShootingGame.sln`,将 MazeShootingGame 作为启动项目后，然后设置 build 属性为 Debug 和 Win32，最后生成解决方案，如果没问题的话，启动 ``build/Debug` 中的 `MazeShootingGame.exe` 运行即可。



## 测试截图

![test_env](doc\imgs\test_env.jpg)





## 开发说明

1. 目前的第三方库只支持如上所列，如需使用其他的第三方库，则需要更新 CmakeLists.txt ，并重新 Cmake 生成新的解决方案，这样才可以在源文件中引入头文件。