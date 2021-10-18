# This is a animation in modern OpenGL project


## introduction

OpenGL is a graphics library with a wide range of APIs to render 2D and 3D graphics 
on the screen. It provides more than 200 functions for easily rendering vector graphics
on GPU, including primitive function, attribute functions, viewing functions, input 
functions, and control functions. CUDA is a parallel computing platform. It enables 
software developers to use GPU efficiently. CUDA-OpenGL Interoperability enables
the calculation of OpenGL can be done in parallel to achieve better performance. Even 
it is possible to finish all visualization without leaving GPU. 
## Running environment

School of Computing System Lab Machines.
Please use http://virtual.computing.clemson.edu/ to apply for SoC Desktop(GPU Enabled).
If you login directly through your own terminal with "ssh -X" command, it will have problems in running the executable file.

## Result
Run program:
1) go to ./main folder
`cd main`

2) compile with command:
`make -f Makefile`

3) run with command:
`./cgfinal vert.shader frag.shader img0_checkers.png img1_lina.png img2_drow.png img3_cm.png img4_sky.jpg img5_floor.jpg img6_text31.jpg`

or 
you can use the run.sh script inside main folder to run everything:
`cd main`
`./run.sh`


Video link:

[![Watch the video](https://i.imgur.com/vKb2F1B.png)](https://youtu.be/W_aOmFOVz1w)



