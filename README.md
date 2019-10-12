# A3 Flight Controller
- Author: mafp
- Date: 2019/10/12
- Version: 1.0.0

## 依赖库
- DJI SDK CORE
- DJI SDK ROS

## 怎么操作
```
git clone https://titanx.liuyinyi.cn/mafp/A3Controller.git
cd A3Controller/src
git clone https://github.com/dji-sdk/Onboard-SDK
cd (dji_sdk_directory)
mkdir build && cd build
cmake ..
make djiosdk-core
sudo make install
cd ../../   //切换到src下
git clone https://github.com/dji-sdk/Onboard-SDK-ROS.git  
cd ..
catkin_make
```

## 代码规范
暂无
## 已开发功能
暂无
## 待开发功能
- 前进后退左右悬停上升下降的接口
- 起飞降落
- 速度设置
- 位置设置
- 倾角设置(俯仰/横滚/偏航)
- 设置waypoint飞行
- 获取位置/速度/姿态/高度/偏航角信息
- 云台控制