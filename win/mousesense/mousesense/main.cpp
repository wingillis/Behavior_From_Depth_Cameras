//
//  main.cpp
//  mousesense
//
//  Created by Winthrop Gillis on 4/14/18.
//  Copyright © 2018 Winthrop Gillis. All rights reserved.
//

#include <iostream>
#include <iomanip>
#include <signal.h>
#include <atomic>
#include <thread>
#include <librealsense2/rs.hpp>
#include <librealsense2/rs_advanced_mode.hpp>
#include <streambuf>
#include <fstream>
#include <string>
#include <stdio.h>
#include "fileSaver.hpp"

atomic<bool> saving(true);
rs2::frame_queue queue(180);

void handle_interrupt(int sig) {
    printf("Program shutting down...\n");
    saving.store(false, memory_order_release);
    return;
}

string load_json(string fname) {
    ifstream file(fname);
    string contents((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    return (string)contents;
}

void saving_thread() {
    FILE *pPipe;
    int filenum = 0;
    long save_every = 2700; // frames
    char depthname[20], tsname[20], framename[20];
//    sprintf(depthname, "depth-%03d.bin", filenum);
    sprintf(depthname, "depth-%03d.avi", filenum);
    sprintf(tsname, "depth_ts-%03d.txt", filenum);
    sprintf(framename, "framenumber-%03d.txt", filenum);
//    FileSaver depth((string)depthname, true);
    FileSaver timeStamp((string)tsname, false);
    FileSaver frameNumber((string)framename, false);
    stringstream sstm;
    sstm << "ffmpeg -y -loglevel fatal -threads 6 -framerate 30 -f rawvideo -s 1280x720 -pix_fmt gray16le -i - -an -vcodec ffv1 -slices 24 -slicecrc 1 -r 30 " << depthname;
    
    if ( !(pPipe = popen(sstm.str().c_str(), "w")) ) {
        printf("popen error\n");
        exit(1);
    }
    
    while(saving.load(memory_order_acquire)) {
        rs2::frame fr;
        if (queue.poll_for_frame(&fr)) {
            long current_frame = fr.get_frame_number();
//            depth.write(fr.as<rs2::video_frame>());
            rs2::video_frame vfr = fr.as<rs2::video_frame>();
            fwrite(vfr.get_data(), 2, vfr.get_height() * vfr.get_width(), pPipe);
            fflush(pPipe);
            timeStamp.write(fr.get_timestamp());
            frameNumber.writeFrameNumber(current_frame);
            if (current_frame % save_every == 0 && current_frame != 0) {
                filenum++;
//                sprintf(depthname, "depth-%03d.bin", filenum);
                sprintf(depthname, "depth-%03d.avi", filenum);
                sprintf(tsname, "depth_ts-%03d.txt", filenum);
                sprintf(framename, "framenumber-%03d.txt", filenum);
//                depth = FileSaver((string)depthname, true);
//                fclose(pPipe);
                timeStamp = FileSaver((string)tsname, false);
                frameNumber = FileSaver((string)framename, false);
            }
        }
    }
    
    fflush(pPipe);
    fclose(pPipe);
    
    printf("Out of the saving loop\n");
    return;
    
}

int main(int argc, const char * argv[]) {
    int width, height, fps;
    if (argc == 3 && strcmp(argv[2], "fast") == 0) {
        printf("You've selected the 'fast' option - 90 fps at 640x480 px\n");
        width = 640;
        height = 480;
        fps = 90;
    } else if (argc == 3 && strcmp(argv[2], "hd") == 0) {
        printf("You've selected the 'high-definition' option - 30 fps at 1280x720 px\n");
        width = 1280;
        height = 720;
        fps = 30;
    } else {
        // let's just make the default "fast" as well
        printf("You gave either the incorrect number of arguments or did not specify a recording type\n");
        return EXIT_FAILURE;
    }

    string params = load_json(argv[1]);
    
    
    // setup capturing the interrupt
    signal(SIGINT, &handle_interrupt);
    //signal(SIGKILL, &handle_interrupt);
    
    thread save_thread(&saving_thread);
//    t.detach();
    
    // define the realsense pipe
    rs2::pipeline pipe;
    
    // define the realsense configuration
    rs2::config config;

    config.enable_stream(RS2_STREAM_DEPTH, 0, width, height, RS2_FORMAT_Z16, fps);
    
    rs2::pipeline_profile profile = pipe.start(config);
    rs400::advanced_mode dev = profile.get_device();
    dev.load_json(params);

    printf("Initialized\n");
    while(saving.load(memory_order_acquire)) {
        // capture stream and save it
        rs2::frameset frameset = pipe.wait_for_frames();
        rs2::depth_frame frame = frameset.get_depth_frame();

        queue.enqueue(frame);
    }
    
    printf("Out of the acquisition loop\n");
    
    pipe.stop();
    save_thread.join();
    
    return EXIT_SUCCESS;
}
//} catch (const rs2::error & e) {
//    std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
//    return EXIT_FAILURE;
//} catch (const std::exception& e) {
//    std::cerr << e.what() << std::endl;
//    return EXIT_FAILURE;
//
//}
