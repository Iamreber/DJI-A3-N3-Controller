#ifndef __PILOT_NOTE_H
#define __PILOT_NOTE_H

#include <ros/ros.h>
#include <geometry_msgs/Vector3>
#include <std_msgs/Float64>

#include <pthread>

#include <dji_sdk/dji_sdk.h>

class Pilot
{
    public:
        Pilot();
        ~Pilot();
        bool init();
        bool takeoff();
        bool land();
        void setPosHori(Float64 px,Float64 py);
        void setVelHori(Float64 vx,Float64 vy);
        void setPosVert(Float64 h);
        void setVelVert(Float64 v);
        void setYaw(Float64 yaw);
    protected:
        ros::ServiceClient sdk_ctrl_authority_service;
		ros::ServiceClient sdk_drone_task_service;
		ros::ServiceClient sdk_drone_arm_service;
		ros::ServiceClient sdk_query_version_service;

        ros::Publisher ctrl_cmd_pub;
        

};
#endif 