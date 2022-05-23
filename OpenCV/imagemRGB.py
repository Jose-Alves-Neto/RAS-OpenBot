import cv2 as cv
import sys
import numpy as np


img = cv.imread("logoras.png")
if img is None:
    sys.exit("Could not read the image.")
(canalAzul, canalVerde, canalVermelho) = cv.split(img)
zeros = np.zeros(img.shape[:2], dtype="uint8")
cv.imshow("Vermelho", cv.merge([zeros, zeros, canalVermelho]))
cv.imshow("Verde", cv.merge([zeros, canalVerde, zeros]))
cv.imshow("Azul", cv.merge([canalAzul, zeros, zeros]))
cv.imshow("Original", img)
cv.waitKey(0)
