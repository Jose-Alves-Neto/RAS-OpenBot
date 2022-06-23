import cv2 as cv
import matplotlib
import numpy as np
import sys


captura = cv.VideoCapture(0)
count = 15

while(1):
    ret, frame = captura.read()
    if not ret:
        break
    cv.imshow("Video", frame)

    k = cv.waitKey(30) & 0xff
    if k == 115:
        cv.imwrite('Images\Calib\\teste' + str(count) + '.png', frame)
        cv.imshow("Imagem Salva", cv.imread('Images\Calib\\teste' + str(count) + '.png'))
        print('imagem salva como imagem.png')
        count+=1
    elif k == 27:
        break

captura.release()
cv.destroyAllWindows()
