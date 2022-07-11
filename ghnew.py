#https://github.com/murtazahassan/OpenCV-Python-Tutorials-and-Projects/blob/master/Intermediate/RealTime_Shape_Detection_Contours.py
import cv2
import numpy as np
import serial
import time
import utils

print("start")
port = "COM3"
bluetooth = serial.Serial(port,baudrate = 9600, timeout=1)
print("connected")
bluetooth.flushInput()

frameWidth = 640
frameHeight = 480
cap = cv2.VideoCapture(0)
cap.set(3, frameWidth)
cap.set(4, frameHeight)

def empty(a):
    pass

cv2.namedWindow("Parameters")
cv2.resizeWindow("Parameters",640,240)
cv2.createTrackbar("Threshold1","Parameters",23,255,empty)
cv2.createTrackbar("Threshold2","Parameters",20,255,empty)
cv2.createTrackbar("Area","Parameters",5000,30000,empty)

def stackImages(scale,imgArray):
    rows = len(imgArray)
    cols = len(imgArray[0])
    rowsAvailable = isinstance(imgArray[0], list)
    width = imgArray[0][0].shape[1]
    height = imgArray[0][0].shape[0]
    if rowsAvailable:
        for x in range ( 0, rows):
            for y in range(0, cols):
                if imgArray[x][y].shape[:2] == imgArray[0][0].shape [:2]:
                    imgArray[x][y] = cv2.resize(imgArray[x][y], (0, 0), None, scale, scale)
                else:
                    imgArray[x][y] = cv2.resize(imgArray[x][y], (imgArray[0][0].shape[1], imgArray[0][0].shape[0]), None, scale, scale)
                if len(imgArray[x][y].shape) == 2: imgArray[x][y]= cv2.cvtColor( imgArray[x][y], cv2.COLOR_GRAY2BGR)
        imageBlank = np.zeros((height, width, 3), np.uint8)
        hor = [imageBlank]*rows
        hor_con = [imageBlank]*rows
        for x in range(0, rows):
            hor[x] = np.hstack(imgArray[x])
        ver = np.vstack(hor)
    else:
        for x in range(0, rows):
            if imgArray[x].shape[:2] == imgArray[0].shape[:2]:
                imgArray[x] = cv2.resize(imgArray[x], (0, 0), None, scale, scale)
            else:
                imgArray[x] = cv2.resize(imgArray[x], (imgArray[0].shape[1], imgArray[0].shape[0]), None,scale, scale)
            if len(imgArray[x].shape) == 2: imgArray[x] = cv2.cvtColor(imgArray[x], cv2.COLOR_GRAY2BGR)
        hor= np.hstack(imgArray)
        ver = hor
    return ver

def getContours(img,imgContour):
    contours, hierarchy = cv2.findContours(img, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    for cnt in contours:
        area = cv2.contourArea(cnt)
        areaMin = cv2.getTrackbarPos("Area", "Parameters")
        if area > areaMin:
            cv2.drawContours(imgContour, cnt, -1, (255, 0, 255), 7)
            peri = cv2.arcLength(cnt, True)
            approx = cv2.approxPolyDP(cnt, 0.02 * peri, True)
            print(len(approx))
            x , y , w, h = cv2.boundingRect(approx)
            cv2.rectangle(imgContour, (x , y ), (x + w , y + h ), (0, 255, 0), 5)

            cv2.putText(imgContour, "Points: " + str(len(approx)), (x + w + 20, y + 20), cv2.FONT_HERSHEY_COMPLEX, .7,
                        (0, 255, 0), 2)
            cv2.putText(imgContour, "Area: " + str(int(area)), (x + w + 20, y + 45), cv2.FONT_HERSHEY_COMPLEX, 0.7,
                        (0, 255, 0), 2)
            if len(approx) == 3:
                cv2.putText(imgContour, "Cedeaza", (x + w + 0, y +00), cv2.FONT_HERSHEY_COMPLEX, .7, (0, 255, 0), 2)
                bluetooth.write(bytes(str("i"), 'utf-8'))
            if len(approx) == 8:
                cv2.putText(imgContour, "Stop", (x + w + 0, y + 00), cv2.FONT_HERSHEY_COMPLEX, .7, (0, 255, 0),2)
                bluetooth.write(bytes(str("2"), 'utf-8'))
            if len(approx) == 4:
                cv2.putText(imgContour, "Prioritate", (x + w + 0, y + 00), cv2.FONT_HERSHEY_COMPLEX, .7, (0, 255, 0), 2)
                bluetooth.write(bytes(str("k"), 'utf-8'))

            if len(approx) == 9:
                cv2.putText(imgContour, "Drum liber", (x + w + 0, y + 00), cv2.FONT_HERSHEY_COMPLEX, .7, (0, 255, 0), 2)
                bluetooth.write(bytes(str("1"), 'utf-8'))

                #time.sleep(0.5)
            if len(approx) == 7:
                success, img = cap.read()

                imgcont, conts, _ = utils.getContours(img, cThr=[150, 175], showCanny=False, minArea=1000, filter=7,
                                                      draw=False)  # Selecting the White box
                if len(conts) != 0:
                    for obj in conts:
                        cv2.polylines(imgcont, [obj[2]], True, (0, 255, 0), 2)
                    # cv2.imshow('Region of Interest',imgcont)
                    x, y, w, h = cv2.boundingRect(conts[0][2])

                    cv2.rectangle(img, (x - 20, y - 20), (x + w + 20, y + h + 20), (0, 255, 0), 2)
                    crop_img = img[y - 20:y + h + 20, x - 20:x + w + 20]
                    # cv2.imshow("Warp",crop_img)
                    tip = tuple(conts[0][2][0][0])

                    M = cv2.moments(conts[0][2])
                    cX = int(M["m10"] / M["m00"])  # Finding the centroid of the arrow
                    cY = int(M["m01"] / M["m00"])

                    cv2.circle(img, (cX, cY), 7, (255, 255, 255), -1)
                    cv2.putText(img, "center", (cX - 20, cY - 20), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2)
                    # cv2.circle(img,tip, 7, (255,0,0), -1)
                    # cv2.circle(img,(100,cY), 7, (255,0,0), -1)

                    angle = int(utils.getAngle(tip, (cX, cY), (30, cY)))
                    # print(angle)

                    if 315 <= angle <= 359 or 0 <= angle < 45:
                        print("Left")
                        cv2.putText(imgContour, "Stanga", (x + w + 0, y +00), cv2.FONT_HERSHEY_COMPLEX, .7, (0, 255, 0), 2)
                        bluetooth.write(bytes(str("a"), 'utf-8'))
                    elif 260 < angle < 279:
                        print('UP')
                        cv2.putText(imgContour, "Sus", (x + w + 0, y +00), cv2.FONT_HERSHEY_COMPLEX, .7, (0, 255, 0), 2)
                        bluetooth.write(bytes(str("1"), 'utf-8'))
                    elif 135 < angle < 225:
                        print('Right')
                        cv2.putText(imgContour, "Dreapta", (x + w + 0, y +00), cv2.FONT_HERSHEY_COMPLEX, .7, (0, 255, 0), 2)
                        bluetooth.write(bytes(str("d"), 'utf-8'))
                    else:
                        print('Down')
                        cv2.putText(imgContour, "Jos", (x + w + 0, y +00), cv2.FONT_HERSHEY_COMPLEX, .7, (0, 255, 0), 2)
                        bluetooth.write(bytes(str("s"), 'utf-8'))

            #bluetooth.write(bytes(str("1"), 'utf-8'))





while True:
    success, img = cap.read()
    imgContour = img.copy()
    imgBlur = cv2.GaussianBlur(img, (7, 7), 1)
    imgGray = cv2.cvtColor(imgBlur, cv2.COLOR_BGR2GRAY)
    threshold1 = cv2.getTrackbarPos("Threshold1", "Parameters")
    threshold2 = cv2.getTrackbarPos("Threshold2", "Parameters")
    imgCanny = cv2.Canny(imgGray,threshold1,threshold2)
    kernel = np.ones((5, 5))
    imgDil = cv2.dilate(imgCanny, kernel, iterations=1)
    getContours(imgDil,imgContour)
    imgStack = stackImages(0.8,([img,imgCanny],
                                [imgDil,imgContour]))
    cv2.imshow("Result", imgStack)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break