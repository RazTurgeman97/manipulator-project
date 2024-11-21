import sys
if sys.prefix == '/usr':
    sys.real_prefix = sys.prefix
    sys.prefix = sys.exec_prefix = '/home/raz/projects/manipulator-project/arduinobot_ws/src/arduinobot_py_examples/install/arduinobot_py_examples'
