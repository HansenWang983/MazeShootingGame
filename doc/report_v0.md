## report v0

### 项目介绍 & 实现结果

该项目主环境为一个迷宫。玩家以第一视角在穿越迷宫的时候，将不定时和不定位置弹出黑色或白色的框（或是不同的Sprite），一部分是平民、一部分是敌人。如果误杀平民则游戏结束，击杀敌人得一定分数，到达终点后展示用时和得分。

目前的实现结果：已经实现迷宫主环境，实现了basic的要求，并完成了bonus里面的一部分要求。

### 开发环境 & 使用到的第三方库

开发环境：OpenGL 3.3

第三方库：

- assimp
- freetype
- glew
- glfw
- glm

### 实现功能列表

Basic：

- Camera Roaming
- Simple lighting and shading (phong)
- Texture mapping
- Shadow mapping
- Model import & Mesh viewing (Assimp)

Bonus:

- Sky Box

- Collision Detection

- Fluid Simulation
  ![image_10.png](https://i.loli.net/2019/06/14/5d03b543344c933254.png)

### 遇到的问题和解决方案（v0）

- 各部件整合问题

  由于之前的火是在外部写的，所以一开始model矩阵没有设置好，导致火无法在迷宫里面显示。后来发现要先在迷宫的坐标系下表示出物件的位置，然后使用getModel函数（通过平移、旋转、缩放）得到model矩阵，再传入顶点着色器就可以解决问题了。

  还有导入外部部件可能需要glEnable(Blend)来将部件与迷宫混合。

  未启用Blend

  ![image_11.png](https://i.loli.net/2019/06/14/5d03b79786c9144803.png)

  启用Blend

  ![image_12.png](https://i.loli.net/2019/06/14/5d03ba29c7d9990636.png)

### 小组成员分工（v0）

| 组员 | 分工 |
| ---- | ---- |
|    王友坤  |    负责主环境搭建  |
|王泽浩|负责外观和游戏逻辑|
|黄紫菱|光照明模型、天气|
|王雁玲|流体模拟|












