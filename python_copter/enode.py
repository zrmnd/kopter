#!/usr/bin/env python
# license removed for brevity
import rospy
from geometry_msgs.msg import Twist
import Queue


q = Queue.Queue(maxsize=32)


def pub_shift(shift):
    global q
    q.put(shift, False)

def enode():
    global q
    pub = rospy.Publisher('/encoder', Twist, queue_size=10)
    msg = Twist()
    msg.linear.y = 0
    msg.linear.z = 0
    msg.angular.x = 0
    msg.angular.y = 0
    msg.angular.z = 0	
    rospy.init_node('enode', anonymous=True)
    rate = rospy.Rate(10) # 10hz
    while not rospy.is_shutdown():
        if not q.empty():
            msg.linear.x = q.get()
            pub.publish(msg)
        rate.sleep()

if __name__ == '__main__':
    pub_shift(1)
    pub_shift(2)
    pub_shift(3)

    try:
        enode()
    except rospy.ROSInterruptException:
        pass
