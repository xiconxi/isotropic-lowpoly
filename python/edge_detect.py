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

def process_image(i):
    I = cv2.imread('../data/'+str(i)+'.jpeg', cv2.IMREAD_COLOR)
    gradient = np.zeros_like(I)
    for c in range(I.shape[2]):
        smoothed = cv2.bilateralFilter(I[:,:,c], 5, 20, 100)
        gradient[:, :, c] = guide_filtering(I[:,:,c], smoothed, 10, (0.03*255) ** 2)
        # gradient[np.where(gradient < 100)] =  0
        gradient[:, :, c] = guide_filtering(gradient[:, :, c], smoothed, 3, (0.15*255) ** 2)
        gradient[:, :, c] = gradient[:, :, c] * 255.0/np.max(gradient[:, :, c])
        # print(np.max(gradient[:, :, c]))
    gradient[:, :, 0] = np.max(gradient, axis=2)
    gradient[:, :, 1] = gradient[:, :, 0]
    gradient[:, :, 2] = gradient[:, :, 0]

    gradient[np.where(gradient < 20)] = 0


    cv2.imwrite("../data/gradient/"+str(i)+".bmp", gradient)


    cv2.imshow("img", np.concatenate([I, gradient  ], axis=1))

    cv2.waitKey()

if __name__ == '__main__':
    for i in range(1, 6):
        process_image(i)
