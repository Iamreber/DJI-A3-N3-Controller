/**
 * @file pilot.h
 * @brief all api about controlling pilot 
 * @author mafp
 * @email 767102280@qq.com
 * @version 1.0.0
 * @date 2019/10/19
 */
#include "pilot_node.h"

bool Pilot::init()
{
    ros::NodeHandle nh;
    ros::NodeHandle nh_private("~");
    
    //! dji_sdk的控制节点，向该节点发布控制信息
    ctrl_cmd_pub = nh.advertise<sensor_msgs::Joy>("dji_sdk/flight_control_setpoint_generic",10);

    //! dji_sdk的一些服务
    sdk_ctrl_authority_service = nh.serviceClient<dji_sdk::SDKControlAuthority> ("dji_sdk/sdk_control_authority");
	sdk_drone_task_service = nh.serviceClient<dji_sdk::DroneTaskControl>("dji_sdk/drone_task_control");
	//sdk_drone_arm_service = nh.serviceClient<dji_sdk::DroneArmControl>("dji_sdk/drone_arm_control");
	sdk_query_version_service = nh.serviceClient<dji_sdk::QueryDroneVersion>("dji_sdk/query_drone_version");
    sdk_set_local_ref_service = nh.serviceClient<dji_sdk::SetLocalPosRef> ("dji_sdk/set_local_pos_ref");
    return true;
}
bool Pilot::obtain_control()
{
  dji_sdk::SDKControlAuthority authority;
  authority.request.control_enable=1;
  sdk_ctrl_authority_service.call(authority);

  if(!authority.response.result)
  {
    ROS_ERROR("obtain control failed!");
    return false;
  }

  return true;
}
bool Pilot::takeoff()
{
    //! 起飞前先获取位置参考
    dji_sdk::SetLocalPosRef localPosReferenceSetter;
    sdk_set_local_ref_service.call(localPosReferenceSetter);
    if(!localPosReferenceSetter.response.result)
    {
        ROS_ERROR("GPS health insufficient - No local frame reference for height. Exiting.");
        return false;
    }
    //! 飞行任务控制服务
    dji_sdk::DroneTaskControl droneTaskControl;
    droneTaskControl.request.task = dji_sdk::DroneTaskControl::Request::TASK_TAKEOFF;//! 4
    sdk_drone_task_service.call(droneTaskControl);
    ros::Time start_time = ros::Time::now();
    if(!droneTaskControl.response.result)
    {
        return false;
    }
    ros::Duration(0.01).sleep();
    ros::spinOnce();

    //! 启动电机
    while (flight_status != DJISDK::FlightStatus::STATUS_ON_GROUND &&
         display_mode != DJISDK::DisplayMode::MODE_ENGINE_START &&
         ros::Time::now() - start_time < ros::Duration(5)) {
    ros::Duration(0.01).sleep();
    ros::spinOnce();
    }
    //! 电机启动失败
    if(ros::Time::now() - start_time > ros::Duration(5)) 
    {
        ROS_ERROR("Takeoff failed. Motors are not spinnning.");
        return false;
    }
    else 
    {
        start_time = ros::Time::now();
        ROS_INFO("Motor Spinning ...");
        ros::spinOnce();
    }

    //! 飞机升天
    while (flight_status != DJISDK::FlightStatus::STATUS_IN_AIR &&
          (display_mode != DJISDK::DisplayMode::MODE_ASSISTED_TAKEOFF || display_mode != DJISDK::DisplayMode::MODE_AUTO_TAKEOFF) &&
          ros::Time::now() - start_time < ros::Duration(20)) {
    ros::Duration(0.01).sleep();
    ros::spinOnce();
    }
    //! 升天失败
    if(ros::Time::now() - start_time > ros::Duration(20)) 
    {
        ROS_ERROR("Takeoff failed. Aircraft is still on the ground, but the motors are spinning.");
        return false;
    }
    else 
    {
        start_time = ros::Time::now();
        ROS_INFO("Ascending...");
        ros::spinOnce();
    }

    //! 最后再做一些检查
    while ( (display_mode == DJISDK::DisplayMode::MODE_ASSISTED_TAKEOFF || display_mode == DJISDK::DisplayMode::MODE_AUTO_TAKEOFF) &&
          ros::Time::now() - start_time < ros::Duration(20)) 
    {
        ros::Duration(0.01).sleep();
        ros::spinOnce();
    }

    if ( display_mode != DJISDK::DisplayMode::MODE_P_GPS || display_mode != DJISDK::DisplayMode::MODE_ATTITUDE)
    {
        ROS_INFO("Successful takeoff!");
        start_time = ros::Time::now();
    }
    else
    {
        ROS_ERROR("Takeoff finished, but the aircraft is in an unexpected mode. Please connect DJI GO.");
        return false;
    }
    return true;
}

bool Pilot::land()
{
    dji_sdk::DroneTaskControl droneTaskControl;

    droneTaskControl.request.task = dji_sdk::DroneTaskControl::Request::TASK_LAND;//! 4
    sdk_drone_task_service.call(droneTaskControl);
    if(!droneTaskControl.response.result)
    {
    ROS_ERROR("drone_land fail");
    return false;
    }
    return true;
}

void Pilot::setPosHori(float px,float py)
{
    sensor_msgs::Joy ctrlCmd;
    uint8_t flag = (DJISDK::HORIZONTAL_POSITION |
                    DJISDK::VERTICAL_POSITION   |
                    DJISDK::YAW_ANGLE           |
                    DJISDK::HORIZONTAL_BODY     |
                    DJISDK::STABLE_ENABLE);
    ctrlCmd.axes.push_back(px);
    ctrlCmd.axes.push_back(py);
    ctrlCmd.axes.push_back(0);
    ctrlCmd.axes.push_back(0);
    ctrlCmd.axes.push_back(flag);
    ctrl_cmd_pub.publish(ctrlCmd);
}

void Pilot::setVelHori(float vx,float vy)
{
    sensor_msgs::Joy ctrlCmd;
    uint8_t flag = (DJISDK::HORIZONTAL_VELOCITY |
                    DJISDK::VERTICAL_VELOCITY   |
                    DJISDK::YAW_ANGLE           |
                    DJISDK::HORIZONTAL_BODY     |
                    DJISDK::STABLE_ENABLE);
    ctrlCmd.axes.push_back(vx);
    ctrlCmd.axes.push_back(vy);
    ctrlCmd.axes.push_back(0);
    ctrlCmd.axes.push_back(0);
    ctrlCmd.axes.push_back(flag);
    ctrl_cmd_pub.publish(ctrlCmd);
}

void Pilot::setPosVert(float h)
{
    sensor_msgs::Joy ctrlCmd;
    uint8_t flag = (DJISDK::HORIZONTAL_POSITION |
                    DJISDK::VERTICAL_POSITION   |
                    DJISDK::YAW_ANGLE           |
                    DJISDK::HORIZONTAL_BODY     |
                    DJISDK::STABLE_ENABLE);
    ctrlCmd.axes.push_back(0);
    ctrlCmd.axes.push_back(0);
    ctrlCmd.axes.push_back(h);
    ctrlCmd.axes.push_back(0);
    ctrlCmd.axes.push_back(flag);
    ctrl_cmd_pub.publish(ctrlCmd);
}

void Pilot::setVelVert(float v)
{
    sensor_msgs::Joy ctrlCmd;
    uint8_t flag = (DJISDK::HORIZONTAL_POSITION |
                    DJISDK::VERTICAL_VELOCITY   |
                    DJISDK::YAW_ANGLE           |
                    DJISDK::HORIZONTAL_BODY     |
                    DJISDK::STABLE_ENABLE);
    ctrlCmd.axes.push_back(0);
    ctrlCmd.axes.push_back(0);
    ctrlCmd.axes.push_back(v);
    ctrlCmd.axes.push_back(0);
    ctrlCmd.axes.push_back(flag);
    ctrl_cmd_pub.publish(ctrlCmd);
}

void Pilot::setYaw(float yaw)
{
    sensor_msgs::Joy ctrlCmd;
    uint8_t flag = (DJISDK::HORIZONTAL_VELOCITY |
                    DJISDK::VERTICAL_VELOCITY   |
                    DJISDK::YAW_ANGLE           |
                    DJISDK::HORIZONTAL_BODY     |
                    DJISDK::STABLE_ENABLE);
    ctrlCmd.axes.push_back(0);
    ctrlCmd.axes.push_back(0);
    ctrlCmd.axes.push_back(0);
    ctrlCmd.axes.push_back(yaw);
    ctrlCmd.axes.push_back(flag);
    ctrl_cmd_pub.publish(ctrlCmd);
}
//! 
//！获取无人机状态信息
geometry_msgs::Vector3 Pilot::attitude_pull()
{
    geometry_msgs::Vector3 ret;
    tf::Matrix3x3 R_FLU2ENU(tf::Quaternion(data_attitude.quaternion.x, data_attitude.quaternion.y, data_attitude.quaternion.z, data_attitude.quaternion.w));
    R_FLU2ENU.getRPY(ret.x, ret.y, ret.z);
    return ret;
}
geometry_msgs::Vector3 Pilot::position_pull()
{
    geometry_msgs::Vector3 ret;
    ret.x = data_local_position.x;
    ret.y = data_local_position.y;
    ret.z = data_local_position.z;
    return ret;
}     
uint8_t Pilot::flight_status_pull()
{
    return flight_status;
}               
uint8_t Pilot::display_mode_pull()   
{
    return display_mode;
}      
geometry_msgs::Vector3 Pilot::liner_acc_pull()
{
    return data_imu.linear_acceleration;
}
geometry_msgs::Vector3 Pilot::angular_vel_pull()
{
    return data_imu.angular_velocity;
}
geometry_msgs::Vector3 Pilot::velocity_pull()
{
    geometry_msgs::Vector3 ret = velocity.vector;
    return ret;
}
float Pilot::height_pull()
{
    return (float)height.data;
}

//! 状态更新函数
void Pilot::update_attitude(const geometry_msgs::QuaternionStamped attitude)
{
    data_attitude = attitude;
}
void Pilot::update_gps_position(const sensor_msgs::NavSatFix gps_pos)
{
    data_gps_position = gps_pos;
}
void Pilot::update_flight_status(const std_msgs::UInt8 status)
{
    flight_status = status.data;
}
void Pilot::update_display_mode(const std_msgs::UInt8 mode)
{
    display_mode = mode.data;
}
void Pilot::update_local_position(const geometry_msgs::PointStamped local_position)
{
    data_local_position = local_position.point;
}
void Pilot::update_imu(const sensor_msgs::Imu imu)
{
    data_imu = imu;
}
void Pilot::update_velocity(const geometry_msgs::Vector3Stamped vel)
{
    velocity = vel;
}
void Pilot::update_height(const std_msgs::Float32 h)
{
    height = h;
}

//! 回调函数定义

void dji_attitude_callback(const geometry_msgs::QuaternionStamped::ConstPtr& msg)
{
    //! 四元数转换成欧拉角
    //TODO
    if(p)
    {
    	p->update_attitude(*msg);
    }
    return;
}
void dji_gps_callback(const sensor_msgs::NavSatFix::ConstPtr& msg)
{
if(p)
{
    p->update_gps_position(*msg);
} 
    return;

}
void dji_flight_status_callback(const std_msgs::UInt8::ConstPtr& msg)
{
if(p)
{
    p->update_flight_status(*msg);
}    
    return;
}
void dji_display_mode_callback(const std_msgs::UInt8::ConstPtr& msg)
{
if(p)
{
    p->update_display_mode(*msg);
} 
    return;
}
void dji_local_position_callback(const geometry_msgs::PointStamped::ConstPtr& msg)
{
if(p)
{
    p->update_local_position(*msg);
}
    return;
}
void dji_imu_callback(const sensor_msgs::Imu::ConstPtr& msg)
{
if(p)
{
    p->update_imu(*msg);
}
	return;
}
void dji_velocity_callback(const geometry_msgs::Vector3Stamped::ConstPtr& msg)
{
if(p)
    p->update_velocity(*msg);
    return;
}
void dji_height_callback(const std_msgs::Float32::ConstPtr& msg)
{
if(p)
    p->update_height(*msg);
return;
}

