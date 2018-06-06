import fire
import sys

sys.path.append('/usr/local/lib')
import pyrealsense2 as rs

def main():
  ctx = rs.context()
  devs = ctx.query_devices()
  print(f'There are {len(devs)} devices')
  for i in devs:
    print(i)
  print(f'Connecting to first device...')
  dev = devs[0]
  adv_mode = rs.rs400_advanced_mode(dev)
  # print(f'Advanced mode is {adv_mode.is_enabled()}')
  print(adv_mode)

def cli():
  fire.Fire(main)

if __name__ == '__main__':
  fire.Fire()
