## A3 Flight Controller
- Author: mafp
- Date: 2019/10/12
- Version: 1.0.0

### 依赖库
- DJI SDK CORE
- DJI SDK ROS

### 怎么操作
```
git clone https://titanx.liuyinyi.cn/mafp/A3Controller.git
cd A3Controller

// 编译dji sdk 
cd src/Onboard-SDK
mkdir build && cd build
cmake ..
make djiosdk-core
sudo make install

//然后到顶层目录A3Controller下
catkin_make
```

### 代码规范
代码架构如下：
```
├── README.md (说明文档)
└── src (各个包package的主文件)
    ├── CMakeLists.txt 
    ├── Onboard-SDK
    ├── Onboard-SDK-ROS
    └── pilot (飞控)
```
### 已开发功能
暂无
### 待开发功能
- 前进后退左右悬停上升下降的接口
- 起飞降落
- 速度设置
- 位置设置
- 倾角设置(俯仰/横滚/偏航)
- 设置waypoint飞行
- 获取位置/速度/姿态/高度/偏航角信息
- 云台控制
- 视觉定位