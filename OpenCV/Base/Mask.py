import cv2 as cv
import numpy as np
import matplotlib.pyplot as plt

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

record = False


def constrain(val, min_val, max_val):
    return min(max_val, max(min_val, val))


def resize(img, amount):
    width = int(img.shape[1] * amount/100)
    height = int(img.shape[0] * amount/100)
    return cv.resize(img, (width, height), interpolation=cv.INTER_AREA)


def writePos(x, y, angle):
    with open("pos.txt", "a") as f:
        f.write("\n" + str(x) + " " + str(y) + " " + str(angle * 180 / np.pi))


def plot():
    with open("pos.txt", "r") as f:
        x, y = [], []
        for line in f:
            if line == "\n":
                continue
            x.append(int(line.split()[0]))
            y.append(int(line.split()[1]))
        plt.plot(x, y, 'ro')
        plt.axis([0, 640, 480, 0])
        plt.show()


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


def on_low_S_thresh_trackbar(val):
    global s_min
    s_max = cv.getTrackbarPos(max_S_name, window_mask)
    s_min = constrain(val, 0, s_max-1)
    cv.setTrackbarPos(min_S_name, window_mask, s_min)
    if (val > s_max):
        cv.setTrackbarPos(max_S_name, window_mask, val + 1)


def on_max_S_thresh_trackbar(val):
    global s_max
    s_min = cv.getTrackbarPos(min_S_name, window_mask)
    s_max = constrain(val, s_min, max_value)
    cv.setTrackbarPos(max_S_name, window_mask, s_max)
    if (val < s_min):
        cv.setTrackbarPos(min_S_name, window_mask, val - 1)


def on_min_V_thresh_trackbar(val):
    global v_min
    v_max = cv.getTrackbarPos(max_V_name, window_mask)
    v_min = constrain(val, 0, v_max-1)
    cv.setTrackbarPos(min_V_name, window_mask, v_min)
    if (val > v_max):
        cv.setTrackbarPos(max_V_name, window_mask, val + 1)


def on_max_V_thresh_trackbar(val):
    global v_max
    v_min = cv.getTrackbarPos(min_V_name, window_mask)
    v_max = constrain(val, v_min, max_value)
    cv.setTrackbarPos(max_V_name, window_mask, v_max)
    if (val < v_min):
        cv.setTrackbarPos(min_V_name, window_mask, val - 1)


def getMaskValue():
    return np.array([h_min, s_min, v_min]), np.array([h_max, s_max, v_max])


captura = cv.VideoCapture(-1, cv.CAP_MSMF)
captura.set(cv.CAP_PROP_FRAME_WIDTH, 640)
captura.set(cv.CAP_PROP_FRAME_HEIGHT, 480)
print(captura)
window_mask = 'Colorbars'

cv.namedWindow(window_mask)

cv.createTrackbar(min_H_name, window_mask, h_min,
                  max_value_H, on_low_H_thresh_trackbar)
cv.createTrackbar(max_H_name, window_mask, h_max,
                  max_value_H, on_max_H_thresh_trackbar)

cv.createTrackbar(min_S_name, window_mask, s_min,
                  max_value, on_low_S_thresh_trackbar)
cv.createTrackbar(max_S_name, window_mask, s_max,
                  max_value, on_max_S_thresh_trackbar)

cv.createTrackbar(min_V_name, window_mask, v_min,
                  max_value, on_min_V_thresh_trackbar)
cv.createTrackbar(max_V_name, window_mask, v_max,
                  max_value, on_max_V_thresh_trackbar)

kernel = np.ones((2, 3), np.uint8)

while(True):
    ret, frame = captura.read()
    if not ret:
        break

    hsv = cv.cvtColor(frame, cv.COLOR_BGR2HSV)

    minMask, maxMask = getMaskValue()
    mask = cv.inRange(hsv, minMask, maxMask)
    frameMask = cv.bitwise_and(hsv, frame, mask=mask)
    result = cv.dilate(frameMask, kernel, iterations=1)

    (cnts, _) = cv.findContours(cv.cvtColor(result.copy(), cv.COLOR_BGR2GRAY),
                                cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)
    if (type(cnts) == type(None)):
        continue
    elif len(cnts) > 0:
        cnt = sorted(cnts, key=cv.contourArea, reverse=True)[0]
        if record:
            M = cv.moments(cnt)
            if (M['m00'] != 0):
                cx = int(M['m10']/M['m00'])
                cy = int(M['m01']/M['m00'])
                cv.circle(frame, (cx, cy), 7, (0, 0, 255), -1)
                coords = cv.boxPoints(cv.minAreaRect(cnt))
                angle = np.arctan2(
                    coords[3][1] - coords[1][1], coords[3][0] - coords[1][0])
                writePos(cx, cy, angle)

        rect = np.int32(cv.boxPoints(cv.minAreaRect(cnt)))
        cv.drawContours(frame, [rect], -1, (0, 255, 255), 2)
        cv.imshow("conto", frame)

    Vert = np.vstack([
        np.hstack([frame, hsv]),
        np.hstack([frameMask, result])
    ])

    cv.imshow('Cam', resize(Vert, 50))

    k = cv.waitKey(30) & 0xff
    if k == 27:
        plot()
        break
    elif k == ord('r'):
        record = not record

cv.destroyAllWindows()
