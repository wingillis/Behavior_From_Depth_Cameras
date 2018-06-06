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

def cli():
  fire.Fire(main)

if __name__ == '__main__':
  fire.Fire()
