#include "pilot_node.h"

void Pilot::init()
{
    ros::NodeHandle nh;
    ros::NodeHandle nh_private("~");

    ctrl_cmd_pub = nh.advertise<sensor_msgs::Joy>("dji_sdk/flight_control_setpoint_generic");
    sdk_ctrl_authority_service = nh.serviceClient<dji_sdk::SDKControlAuthority> ("dji_sdk/sdk_control_authority");
	sdk_drone_task_service = nh.serviceClient<dji_sdk::DroneTaskControl>("dji_sdk/drone_task_control");
	sdk_drone_arm_service = nh.serviceClient<dji_sdk::DroneArmControl>("dji_sdk/drone_arm_control");
	sdk_query_version_service = nh.serviceClient<dji_sdk::QueryDroneVersion>("dji_sdk/query_drone_version");

    return;
}
void Pilot::setPosHori(Float64 px,Float64 py)
{
    

}