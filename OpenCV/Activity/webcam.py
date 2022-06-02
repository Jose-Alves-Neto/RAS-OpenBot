import cv2 as cv
import matplotlib
import numpy as np
import sys


captura = cv.VideoCapture(0)

while(1):
    ret, frame = captura.read()
    cv.imshow("Video", frame)

    k = cv.waitKey(30) & 0xff
    if k == 115:
        cv.imwrite('imagem.png', frame)
        cv.imshow("Imagem Salva", cv.imread('imagem.png'))
        print('imagem salva como imagem.png')
    elif k == 27:
        break

captura.release()
cv.destroyAllWindows()
