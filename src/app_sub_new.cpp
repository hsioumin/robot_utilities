#include <iostream>
#include <stdio.h>
#include "ros/ros.h"
#include "std_msgs/String.h"
#include <string>
#include "actionlib_msgs/GoalID.h"
#include <stdlib.h>
#include "geometry_msgs/Vector3.h"
#include <string.h>
#include <errno.h>

using namespace std;
float init_pose_x = 0.0f;
float init_pose_y = 0.0f;
float init_pose_a = 0.0f;
 
void SlamMode()
{
  system("rosnode kill amcl");
  system("rosnode kill map_server");
  system("launch andbot 35_5F_gmapping.launch &");
}
 
void write_launch_file()
{
  FILE *file = NULL;
	file = fopen("/home/odroid/catkin_ws/src/metal/andbot/launch/min.launch","w+");
  if(file == NULL)
	{
		cout << "error msg" << "errno:" << errno << " " << strerror(errno) << endl;
		return;
	}
	fprintf(file,"<launch>\n");
  fprintf(file,"\t<param name=\"/amcl/initial_pose_x\"  value=\"%.2f\"/>\n",init_pose_x);
  fprintf(file,"\t<param name=\"/amcl/initial_pose_y\"  value=\"%.2f\"/>\n",init_pose_y);
  fprintf(file,"\t<param name=\"/amcl/initial_pose_a\"  value=\"%.2f\"/>\n",init_pose_a);
  fprintf(file,"</launch>\n");
  fclose(file);
}

void NavMode()
{
  system("roscd andbot/map;rosrun map_server map_saver -f mappp");
	write_launch_file();
  system("rosnode kill slam_gmapping");
  system("roslaunch andbot andbot2.launch &");
}
 
void Reboot()
{
  system("echo 8246 | sudo -S reboot");
}

void Shutdown()
{
  system("sync; sync; echo 8246 | sudo -S shutdown -h now");
}
 
void CancelGoal()
{
 
  system("rostopic pub -1 /move_base/cancel actionlib_msgs/GoalID -- {}");
/*  cout << "cancel goal" << endl;
//  system("rostopic pub /move_base/cancel actionlib_msgs/GoalID -- {}");
  ros::NodeHandle n;
  ros::Publisher chatter_pub = n.advertise<actionlib_msgs::GoalID>("/move_base/cancel", 1000);
  actionlib_msgs::GoalID goal_id;
  if (ros::ok())
  {
    cout << "ros is ok" << endl;
  chatter_pub.publish(goal_id);
  ros::spinOnce();
  }
  cout << "exit cancel goal" << endl;*/
}
 
 
void poseCallback(const geometry_msgs::Vector3 &msg)
{
  init_pose_x = msg.x;
  init_pose_y = msg.y;
  init_pose_a = msg.z;
	cout << "xyz" << endl;
	cout << msg.x << " " << msg.y << " " << msg.z << endl;
}
void chatterCallback(const std_msgs::String::ConstPtr& msg)
{
  string tmp = msg->data;
  if(tmp.compare("1")==0)
  {
		    SlamMode();
  }
  else if(tmp.compare("2")==0)
  {
        NavMode();
  }
  else if(tmp.compare("3")==0)
  {
        Reboot();
  }
  else if(tmp.compare("4")==0)
  {
        Shutdown();
  }
  else if(tmp.compare("5")==0)
  {
        CancelGoal();
  }
  else
  {
  ROS_INFO("ERROR");
  }
}
 
int main(int argc, char **argv)
{
  ros::init(argc, argv, "app_sub");

  ros::NodeHandle n;

  ros::Subscriber sub = n.subscribe("robot_utilities", 1000, chatterCallback);
  ros::Subscriber sub_get_pose = n.subscribe("/andbot/current_position_euler", 1000, poseCallback);
  ros::spin();
 
  return 0;
}
