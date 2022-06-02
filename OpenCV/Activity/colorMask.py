import cv2 as cv
import numpy as np


def nothing(x):
    pass


captura = cv.VideoCapture(0)

cv.namedWindow("Video")
cv.createTrackbar('Hmin', 'Video', 0, 255, nothing)
cv.createTrackbar('Smin', 'Video', 0, 255, nothing)
cv.createTrackbar('Vmin', 'Video', 0, 255, nothing)

cv.createTrackbar('Hmax', 'Video', 0, 255, nothing)
cv.createTrackbar('Smax', 'Video', 0, 255, nothing)
cv.createTrackbar('Vmax', 'Video', 0, 255, nothing)

while(1):
    ret, frame = captura.read()
    if not ret:
        break
    cv.imshow("Video", frame)

    hsv = cv.cvtColor(frame, cv.COLOR_BGR2HSV)

    hmin = cv.getTrackbarPos('Hmin', 'Video')
    smin = cv.getTrackbarPos('Smin', 'Video')
    vmin = cv.getTrackbarPos('Vmin', 'Video')

    hmax = cv.getTrackbarPos('Hmax', 'Video')
    smax = cv.getTrackbarPos('Smax', 'Video')
    vmax = cv.getTrackbarPos('Vmax', 'Video')

    minMask = np.array([hmin, smin, vmin])
    maxMask = np.array([hmax, smax, vmax])

    mask = cv.inRange(hsv, minMask, maxMask)
    result = cv.bitwise_and(frame, frame, mask=mask)

    img_gray = cv.cvtColor(result, cv.COLOR_BGR2GRAY)
    img_blur = cv.GaussianBlur(img_gray, (7, 7), 0)

    sobelxy = cv.Sobel(src=img_blur, ddepth=cv.CV_64F, dx=1, dy=1, ksize=5)

    (cnts, _) = cv.findContours(mask.copy(),
                                cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)
    if (type(cnts) == type(None)):
        continue
    elif len(cnts) > 0:
        cnt = sorted(cnts, key=cv.contourArea, reverse=True)[0]
        rect = np.int32(cv.boxPoints(cv.minAreaRect(cnt)))
        cv.drawContours(frame, [rect], -1, (0, 255, 255),
                        2)
        cv.imshow("conto", frame)

    cv.imshow('result', result)
    cv.imshow('sobel edge', sobelxy)

    k = cv.waitKey(30) & 0xff
    if k == 115:
        cv.imwrite('imagem.png', frame)
        cv.imshow("Imagem Salva", cv.imread('imagem.png'))
        print('imagem salva como imagem.png')
    elif k == 27:
        break

captura.release()
cv.destroyAllWindows()
