import sys
if sys.prefix == '/usr':
    sys.real_prefix = sys.prefix
    sys.prefix = sys.exec_prefix = '/home/raz/projects/manipulator-project/Arduino-Bot/arduinobot_ws/install/arduinobot_py_examples'
