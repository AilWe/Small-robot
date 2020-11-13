Running Environment:
School of Computing System Lab Machines.
Please use http://virtual.computing.clemson.edu/ to apply for SoC Desktop(GPU Enabled).
I tested and find that both SoC Desktop(GPU Enabled) and Terminal(GPU Enabled) should work well. But if you login directly through your own terminal with "ssh -X" command, it will have problems in running the executable file.

This project is modified from assignment 5. So the compiling step is similar.
Running Steps:
1) go to ./main folder
cd main

2) compile with command:
make -f Makefile

3) run with command:
./cgfinal vert.shader frag.shader img0_checkers.png img1_lina.png img2_drow.png img3_cm.png img4_sky.jpg img5_floor.jpg img6_text31.jpg

or you can use the run.sh script inside main folder to run everything:
cd main
./run.sh


video link:
https://drive.google.com/file/d/1qkFurOUS6gAM7tsj8gjbhyjeBLNSkph3/view?usp=sharing
