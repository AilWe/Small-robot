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

[https://lh3.googleusercontent.com/VIQi0gNvdpBqaOq4LrY0f4yZ_1Si3EiSdGmHAotrwYnDEWqa2HlbxA8W90uNIASGGjpF6p6EXLH3m-5hk0HOiq_G_YqMMjXOJyakvTcl8BlMVGwAum2_qEZgWrALIUhzFUN_DxqBM3le7tELQ3iU5GEqWTnWnWLzeNEd3iOz4tiCNgMNSHq6NhbKkdJM20W5XXhkv4PqKurY0i6A7UGCutxcBfima9B2wcT9fVrUK0MDDuf3Qp9yKLGKwOs7BUBszDvj1f5OYJjqVCqZyHd1swgOuuWbbGhEvQOFVElYfhcJ1Y4wKHnnTqQuFcIPKI7vokxF4RqkObyYwjyy7W0d2rttWnwqQUm9vBiuAg-BvCyb7tKbt-6PC2rSBanTEPckio5YW2tqavad_CaUwaJujo6JNKhEFThu65_IcX7kOdaUMCUHSyk6V5YFLUmfozP7QbiAYXFE23-YD_lrLB2wb4FLxKGiL78Kc_7NR87uDH0wt0BzsUWT1YSy_Bo7Bv-juWXGsM8S35WZG9-QiFILoVkYIBxlHVxpS23HAxbWdP_nrZ0B1hEQLzLoSUIT92_2Pda7sGjQuNS_qirYB-PlvNj06jD-Lb12yNFEN69AHLabt3khMPa-al6ImtrUKSAE_l52_Z3iHkK1zDsWILVMuOzm992HHSsGNNq23yFN7Wp5W0O5jq7ItOpsgh3RxMF3n6ISUE8TtuJc_hb_jKU8MA=w320-h180-no?authuser=0]](https://youtu.be/W_aOmFOVz1w )


