from cv2 import imshow
import numpy as np
import cv2 as cv
import glob

# Dimensões do Tabuleiro de Xadrez
cbcol = 7
cbrow = 9
cbw = 2

# Critério
criteria = (cv.TERM_CRITERIA_EPS + cv.TERM_CRITERIA_MAX_ITER, cbw, 0.001)


# preparar os pontos do objeto, como (0,0,0), (1,0,0), (2,0,0) ....,(6,5,0)
objp = np.zeros((cbrow * cbcol, 3), np.float32)
objp[:, :2] = np.mgrid[0:cbcol, 0:cbrow].T.reshape(-1, 2)
# Vetores para armazenar os pontos de objeto e pontos de imagem de todas as imagens.
objpoints = []  # ponto 3d no espaço do mundo real
imgpoints = []  # ponto 2d no plano da imagem.
images = glob.glob('./Images/Calib/*.png')
print(images)
i = 0
for fname in images:
    print(fname)
    img = cv.imread(fname)
    gray = cv.cvtColor(img, cv.COLOR_BGR2GRAY)
    # Encontra os cantos do tabuleiro de xadrez
    ret, corners = cv.findChessboardCorners(gray, (cbcol, cbrow), None)
    # Se encontrado, adiciona os pontos de objeto e pontos de imagem (após refiná-los)
    #print(ret)
    #print(corners)
    if ret == True:
        objpoints.append(objp)
        corners2 = cv.cornerSubPix(gray, corners, (11, 11), (-1, -1), criteria)
        imgpoints.append(corners)
        # Desenha e mostra os cantos
        cv.drawChessboardCorners(img, (cbcol, cbrow), corners2, ret)
        cv.imwrite('.\Images\Calib\Results\\res' +
                   str(i) + '.jpg', img)
        cv.imshow('img', img)
        cv.waitKey(50)
        i += 1
retval, cameraMatrix, distCoeffs, rvecs, tvecs = cv.calibrateCamera(objpoints, imgpoints, gray.shape[::-1], None, None)
print(retval, distCoeffs, rvecs, tvecs)

np.savez('calibration_data.npz', cameraMatrix=cameraMatrix, distCoeffs=distCoeffs, rvecs=rvecs, tvecs=tvecs)

img = cv.imread('.\Images\Calib\\teste0.png')
h, w = img.shape[:2]
newcameramtx, roi = cv.getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, (w,h), 1, (w,h))

# undistort
dst = cv.undistort(img, cameraMatrix, distCoeffs, None, newcameramtx)
# crop the image
x, y, w, h = roi
dst = dst[y:y+h, x:x+w]
cv.imwrite('calibresult.png', dst)

mean_error = 0
for i in range(len(objpoints)):
    imgpoints2, _ = cv.projectPoints(objpoints[i], rvecs[i], tvecs[i], cameraMatrix, distCoeffs)
    error = cv.norm(imgpoints[i], imgpoints2, cv.NORM_L2)/len(imgpoints2)
    mean_error += error
print( "total error: {}".format(mean_error/len(objpoints)) )

cv.destroyAllWindows()
