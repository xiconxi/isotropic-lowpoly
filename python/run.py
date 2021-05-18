import glob
import os
import concurrent.futures
#
# def weight_cvt(file):
#     print(file)
#     img_name = os.path.basename(file).replace(".bmp","")
#     os.system("rm -rf output/"+img_name)
#     os.system("mkdir -p output/"+img_name)
#     os.system("./ImageStippling "+file+" output/"+img_name+"/img > output/"+img_name+"/log.txt")
#     os.system("ffmpeg -i output/"+img_name+"/img%d.svg -r 2 output/videos/"+img_name+".gif -y")
#     os.system("ffmpeg -i output/"+img_name+"/imgstippling.svg output/videos/"+img_name+".png -y")
#
# def movie_overlay(m1, m2, m3, m4):
#     command = "ffmpeg "
#     command += "-re -i {} -re -i {} -re -i {} -re -i {} -filter_complex".format(m1, m2, m3, m4)
#     command += ''' "nullsrc=size=1024x1024 [base]; [0:v] setpts=PTS-STARTPTS,scale=512x512 [upperleft];   [1:v] setpts=PTS-STARTPTS, scale=512x512 [upperright];
#         [2:v] setpts=PTS-STARTPTS, scale=512x512 [lowerleft];
#         [3:v] setpts=PTS-STARTPTS, scale=512x512 [lowerright];
#         [base][upperleft] overlay=shortest=1[tmp1];
#         [tmp1][upperright] overlay=shortest=1:x=512 [tmp2];
#         [tmp2][lowerleft] overlay=shortest=1:y=512 [tmp3];
#         [tmp3][lowerright] overlay=shortest=1:x=512:y=512" 4x4.mp4'''
#     os.system(command)

if __name__ == "__main__":
    import sys 
    import time

    sample_rate = float(sys.argv[1]) if len(sys.argv) > 1 else 0.01
    n_iter  = float(sys.argv[2]) if len(sys.argv) > 2 else 20

    for i in range(1, 7):
        cmd = "./lowpoly_cvt ../data/gradient/" + str(i)+ ".bmp ../data/"+str(i)+".jpeg ../imgs/svg/"+str(i)+ " "+ str(sample_rate) +" " + str(n_iter)
        print(cmd)
        os.system(cmd)

        # os.system("cairosvg -o ../imgs/"+str(i)+"lowpoly_tri.png  ../imgs/svg/"+str(i)+"lowpoly_tri.svg")
        # os.system("cairosvg -o ../imgs/"+str(i)+"lowpoly_point.png  ../imgs/svg/"+str(i)+"lowpoly_point.svg")

    

    for i in range(1, 7):
        os.system("cairosvg -o ../imgs/"+str(i)+"lowpoly_tri.png  ../imgs/svg/"+str(i)+"lowpoly_tri.svg")
        os.system("cairosvg -o ../imgs/"+str(i)+"lowpoly_point.png  ../imgs/svg/"+str(i)+"lowpoly_point.svg")


    # os.system("./lowpoly_cvt ../data/gradient/1.bmp ../data/1.jpeg ./1 5000 5")
    # os.system("./lowpoly_cvt ../data/gradient/2.bmp ../data/2.jpeg ./2 5000 15")
    # os.system("./lowpoly_cvt ../data/gradient/3.bmp ../data/3.jpeg ./3 5000 15")
    # os.system("cairosvg -o 1lowpoly_tri.png 1lowpoly_tri.svg")
    # os.system("cairosvg -o 2lowpoly_tri.png 2lowpoly_tri.svg")
    # os.system("cairosvg -o 3lowpoly_tri.png 3lowpoly_tri.svg")
    # os.system("mkdir -p output/videos")
    # with concurrent.futures.ProcessPoolExecutor() as executor:
    #     for file in executor.map(weight_cvt, list(glob.glob("../data/*.bmp"))):
    #         pass
    # movie_overlay("videos/lena_gray.mp4", "videos/mao.mp4", "videos/rose.mp4", "videos/sig20.mp4")
