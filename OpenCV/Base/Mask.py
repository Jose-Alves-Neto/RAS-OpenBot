import cv2 as cv
import numpy as np

max_value = 255
max_value_H = 360//2

h_min = 0
s_min = 0
v_min = 0

h_max = max_value_H
s_max = max_value
v_max = max_value

min_H_name, max_H_name = "Hmin", "Hmax"
min_S_name, max_S_name = "Smin", "Smax"
min_V_name, max_V_name = "Vmin", "Vmax"


def nothing(x):
    pass


def constrain(val, min_val, max_val):
    return min(max_val, max(min_val, val))


def on_low_H_thresh_trackbar(val):
    global h_min
    h_max = cv.getTrackbarPos(max_H_name, window_mask)
    h_min = constrain(val, 0, h_max-1)
    cv.setTrackbarPos(min_H_name, window_mask, h_min)
    if (val > h_max):
        cv.setTrackbarPos(max_H_name, window_mask, val + 1)


def on_max_H_thresh_trackbar(val):
    global h_max
    h_min = cv.getTrackbarPos(min_H_name, window_mask)
    h_max = constrain(val, h_min, max_value_H)
    cv.setTrackbarPos(max_H_name, window_mask, h_max)
    if (val < h_min):
        cv.setTrackbarPos(min_H_name, window_mask, val - 1)


def getMaskValue():
    return np.array([h_min, s_min, v_min]), np.array([h_max, s_max, v_max])


captura = cv.VideoCapture(0, cv.CAP_MSMF)

window_mask = 'Colorbars'

cv.namedWindow(window_mask)

cv.createTrackbar(min_H_name, window_mask, h_min,
                  max_value_H, on_low_H_thresh_trackbar)
cv.createTrackbar(max_H_name, window_mask, h_max,
                  max_value_H, on_max_H_thresh_trackbar)

cv.createTrackbar(min_S_name, window_mask, s_min, max_value, nothing)
cv.createTrackbar(max_S_name, window_mask, s_max, max_value, nothing)

cv.createTrackbar(min_V_name, window_mask, v_min, max_value, nothing)
cv.createTrackbar(max_V_name, window_mask, v_max, max_value, nothing)

kernel = np.ones((5, 5), np.uint8)

while(True):
    ret, frame = captura.read()
    if not ret:
        break

    hsv = cv.cvtColor(frame, cv.COLOR_BGR2HSV)

    minMask, maxMask = getMaskValue()
    mask = cv.inRange(hsv, minMask, maxMask)
    frameMask = cv.bitwise_and(hsv, frame, mask=mask)
    result = cv.dilate(frameMask, kernel, iterations=2)

    (cnts, _) = cv.findContours(cv.cvtColor(result.copy(), cv.COLOR_BGR2GRAY),
                                cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)
    if (type(cnts) == type(None)):
        continue
    elif len(cnts) > 0:
        cnt = sorted(cnts, key=cv.contourArea, reverse=True)[0]
        rect = np.int32(cv.boxPoints(cv.minAreaRect(cnt)))
        cv.drawContours(frame, [rect], -1, (0, 255, 255),
                        2)
        cv.imshow("conto", frame)

    Vert = np.vstack([
        np.hstack([frame, hsv]),
        np.hstack([frameMask, result])
    ])

    cv.imshow('Cam', Vert)

    k = cv.waitKey(30) & 0xff
    if k == 27:
        print(cv.getWindowImageRect('Colorbars'))
        break

cv.destroyAllWindows()
