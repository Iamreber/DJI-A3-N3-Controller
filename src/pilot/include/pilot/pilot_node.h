/**
 * @file pilot.h
 * @brief declare the class Pilot and the class Mission
 * @author mafp
 * @email 767102280@qq.com
 * @version 1.0.0
 * @date 2019/10/19
 */

#ifndef __PILOT_NOTE_H
#define __PILOT_NOTE_H

#include <ros/ros.h>
#include <geometry_msgs/Vector3.h>
#include <geometry_msgs/Vector3Stamped.h>
#include <geometry_msgs/QuaternionStamped.h>
#include <geometry_msgs/PointStamped.h>
#include <std_msgs/Float64.h>
#include <std_msgs/UInt8.h>
#include <std_msgs/Float32.h>
#include <sensor_msgs/Joy.h>
#include <sensor_msgs/Imu.h>
#include <sensor_msgs/NavSatFix.h>


#include <dji_sdk/dji_sdk.h>
#include <dji_sdk/DroneTaskControl.h>
#include <dji_sdk/SDKControlAuthority.h>
#include <dji_sdk/QueryDroneVersion.h>
#include <dji_sdk/SetLocalPosRef.h>


#include <tf/tf.h>
class Pilot
{
    //! 初始化函数及控制函数
    public:
        Pilot();
        ~Pilot();
        bool init();
        bool obtain_control();
        bool takeoff();
        bool land();
        void setPosHori(float px,float py);
        void setVelHori(float vx,float vy);
        void setPosVert(float h);
        void setVelVert(float v);
        void setYaw(float yaw);

    //! 无人机状态变量
    private:
        sensor_msgs::NavSatFix data_gps_position;
        geometry_msgs::Point data_local_position;
        geometry_msgs::Vector3 data_attitude;
        // geometry_msgs::Vector3 liner_acc;
        // geometry_msgs::Vector3 angular_vel;
        geometry_msgs::Vector3 velocity;
        std_msgs::Float32 height;
        sensor_msgs::Imu data_imu;
        uint8_t flight_status;
        uint8_t display_mode;

    //! ros ServiceClient 及 Publisher
    protected:
        ros::ServiceClient sdk_ctrl_authority_service;
		ros::ServiceClient sdk_drone_task_service;
		ros::ServiceClient sdk_drone_arm_service;
		ros::ServiceClient sdk_query_version_service;
        
        ros::Publisher ctrl_cmd_pub;
    
    //! 无人机状态获取函数
    public:
        geometry_msgs::Vector3 attitude_pull();     //! 把四元数转换成姿态角作为返回值
        geometry_msgs::Vector3 position_pull();     //! gps 位置信息
        uint8_t flight_status_pull();               //!
        uint8_t display_mode_pull();                //!
        geometry_msgs::Vector3 liner_acc_pull();    //! 线加速度
        geometry_msgs::Vector3 angular_vel_pull();  //! 角速度
        geometry_msgs::Vector3 velocity_pull();     //! ENU坐标系下的速度
        std_msgs::Float32 height_pull();            //! 相对起飞点的高度
    };
class Mission
{
    

};
	//! ros 回调函数
	extern Pilot* p; //! 实例化对象指针
        void dji_attitude_callback(const geometry_msgs::QuaternionStamped::ConstPtr& msg);
        void dji_gps_callback(const sensor_msgs::NavSatFix::ConstPtr& msg);
        void dji_flight_status_callback(const std_msgs::UInt8::ConstPtr& msg);
        void dji_display_mode_callback(const std_msgs::UInt8::ConstPtr& msg);
        void dji_local_position_callback(const geometry_msgs::PointStamped::ConstPtr& msg);
        void dji_imu_callback(const sensor_msgs::Imu::ConstPtr& msg);
        void dji_velocity_callback(const geometry_msgs::Vector3Stamped::ConstPtr& msg);
        void dji_height_callback(const std_msgs::Float32::ConstPtr& msg);


#endif 
