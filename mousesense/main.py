import fire
import sys

sys.path.append('/usr/local/lib')
import pyrealsense2 as rs

def main():
  ctx = rs.context()
  devs = ctx.query_devices()
  print(f'There are {len(devs)} devices')
  print(f'Connecting to first device...')
  dev = devs[0]
  print(dev)
  adv_mode = rs.rs400_advanced_mode(dev)
  print(f'Advanced mode is {adv_mode.is_enabled()}')
  pipeline = rs.pipeline()
  config = rs.config()
  config.enable_stream(rs.stream.depth, 640, 480, rs.format.z16, 30)
  config.enable_stream(rs.stream.color, 640, 480, rs.format.bgr8, 30)
  config.enable_stream(rs.stream.infrared, 640, 480, rs.format.y8, 30)

def cli():
  fire.Fire(main)

if __name__ == '__main__':
  fire.Fire()
