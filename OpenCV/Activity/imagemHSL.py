import cv2 as cv
import sys
import numpy as np


img = cv.imread("logoras.png")
if img is None:
    sys.exit("Could not read the image.")
gray = cv.cvtColor(img, cv.COLOR_BGR2GRAY)
hsv = cv.cvtColor(img, cv.COLOR_BGR2HSV)
lab = cv.cvtColor(img, cv.COLOR_BGR2LAB)

cv.imshow("Original", img)
cv.imshow("Gray", gray)
cv.imshow("HSV", hsv)
cv.imshow("L*a*b*", lab)

cv.waitKey(0)
