'''
Very quick script to compress any raw data into ffv1 format
'''
from moseq2_extract.io.video import write_frames, get_movie_info
import numpy as np
import sys

def main():
    dims = (int(sys.argv[2]), int(sys.argv[3]))
    filename = sys.argv[1]
    outname = filename.replace('bin', 'avi')
    nframes = get_movie_info(filename, frame_dims=dims)['nframes']
    frames = np.memmap(filename, dtype='uint16', mode='r', shape=(nframes, dims[1], dims[0]))
    # frames = frames.reshape(nframes, dims[1], dims[0])
    write_frames(outname, frames, fps=int(sys.argv[4]), frame_size='{}x{}'.format(*dims))

if __name__ == '__main__':
    main()
