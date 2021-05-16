import cv2
import numpy as np

def guide_filtering(p, I, r, epsilon):
    p, I = np.copy(p).astype(np.float32), np.copy(I).astype(np.float32)
    I_mean  = cv2.boxFilter(I, -1, (r,r) )
    p_mean = cv2.boxFilter(p, -1, (r,r))
    Ip_mean = cv2.boxFilter(I*p, -1, (r,r))
    II_mean = cv2.boxFilter(I*I, -1, (r,r))

    covIp = Ip_mean - I_mean * p_mean    
    varI = II_mean - I_mean * I_mean
    
    a = covIp/(varI + epsilon)
    b = p_mean - a * I_mean

    a_mean = cv2.boxFilter(a, -1, (r,r))
    b_mean = cv2.boxFilter(b, -1, (r,r))

    return  (a * I + b_mean * 0.1).astype(np.uint8)

if __name__ == '__main__':
    I = cv2.imread('data/1.jpeg')
    I_gray, I_boost = cv2.decolor(I)
    I_0 = cv2.ximgproc.l0Smooth(I_gray)
    gradient = guide_filtering(I_gray, I_0, 5, (0.05*255) ** 2)

    print(np.unique(gradient))


    # gradient[np.where(gradient < 10)] = 0
    # gradient[np.where(gradient > 100)] = 255
    gradient[np.where(gradient < 30)] = 0

    cv2.imwrite("data/gradient/1.bmp", gradient)


    cv2.imshow("img", np.concatenate([I_gray, I_0, gradient], axis=1))
    cv2.waitKey()
