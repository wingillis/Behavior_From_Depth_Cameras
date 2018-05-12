import sys
from moseq2_extract.io.video import read_frames, write_frames_preview

filename = sys.argv[1]

chunksize = 2500

frames = read_frames(filename, frames=range(chunksize), fps=90, frame_size=(640,480))

write_frames_preview('movie.mp4', frames, fps=45, depth_min=2900, depth_max=2400)
