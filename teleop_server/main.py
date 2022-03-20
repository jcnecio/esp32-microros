
import rclpy
import threading

from flask import Flask, request
from rclpy.node import Node

from geometry_msgs.msg import Twist, Vector3


class MinimalPublisher(Node):
    def __init__(self):
        self.current_linear = [0, 0, 0]
        self.current_angular = [0, 0, 0]
        super().__init__('minimal_publisher')
        self.publisher_ = self.create_publisher(Twist, '/cmd_vel', 10)
        timer_period = 0.5
        self.timer = self.create_timer(timer_period, self.timer_callback)

    def timer_callback(self):
        msg = Twist(
            linear=Vector3(
                x=float(self.current_linear[0]),
                y=float(self.current_linear[1]),
                z=float(self.current_linear[2]),
            ),
            angular=Vector3(
                x=float(self.current_angular[0]),
                y=float(self.current_angular[1]),
                z=float(self.current_angular[2]),
            )
        )
        self.publisher_.publish(msg)
        self.get_logger().info('Publishing: "%s"' % msg)

    
app = Flask(__name__)

rclpy.init()
minimal_publisher = MinimalPublisher()

@app.route("/")
def move():
    dir = int(request.args.get('dir', 0))
    minimal_publisher.current_linear = [0,0,0]
    minimal_publisher.current_angular = [0,0,0]
    if dir == 1:
        minimal_publisher.current_linear = [1,0,0]
    if dir == 2:
        minimal_publisher.current_linear = [-1,0,0]
    if dir == 3:
        minimal_publisher.current_angular = [0,0,-1]
    if dir == 4:
        minimal_publisher.current_angular = [0,0,1]
    return f"{minimal_publisher.current_linear},{minimal_publisher.current_angular}"

def main(args=None):
    threading.Thread(target=lambda: app.run(host="0.0.0.0", debug=True, use_reloader=False)).start()
    rclpy.spin(minimal_publisher)
    minimal_publisher.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()