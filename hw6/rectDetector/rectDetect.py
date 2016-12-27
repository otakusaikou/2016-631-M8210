#!/usr/bin/env python
# -*- coding: utf-8 -*-
import numpy as np
import matplotlib.pyplot as plt
import cv2

img = cv2.imread("../img/rects.bmp")
gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
gray = cv2.blur(gray, (5, 5))
edge = cv2.Canny(gray, 50, 150, 3)

def isPinRectangle(r, xp, yp):
    x = r[:, 0]
    y = r[:, 1]

    tx1 = np.append(r[:2, 0], xp)
    ty1 = np.append(r[:2, 1], yp)
    tx2 = np.append(r[1:3, 0], xp)
    ty2 = np.append(r[1:3, 1], yp)
    tx3 = np.append(r[2:, 0], xp)
    ty3 = np.append(r[2:, 1], yp)
    tx4 = np.append(np.roll(r[:, 0], 1)[:2], xp)
    ty4 = np.append(np.roll(r[:, 1], 1)[:2], yp)

    areaR = 0.5*np.abs(np.dot(x,np.roll(y,1))-np.dot(y,np.roll(x,1)))
    areaT1 = 0.5*np.abs(np.dot(tx1,np.roll(ty1,1))-np.dot(ty1,np.roll(tx1,1)))
    areaT2 = 0.5*np.abs(np.dot(tx2,np.roll(ty2,1))-np.dot(ty2,np.roll(tx2,1)))
    areaT3 = 0.5*np.abs(np.dot(tx3,np.roll(ty3,1))-np.dot(ty3,np.roll(tx3,1)))
    areaT4 = 0.5*np.abs(np.dot(tx4,np.roll(ty4,1))-np.dot(ty4,np.roll(tx4,1)))
    return round(abs(areaR - (areaT1 + areaT2 + areaT3 + areaT4)), 8) == 0


def getIntersect(xas, yas, xae, yae, xbs, ybs, xbe, ybe):
    ma = ((yae - yas)*1.0 / (xae - xas))
    ba = yas - ma * xas

    mb = ((ybe - ybs)*1.0 / (xbe - xbs))
    bb = ybs - mb * xbs
    X = (bb - ba)*1.0 / (ma - mb)
    Y = ma*X + ba

    minXa, maxXa = min([xas, xae]), max([xas, xae])
    minYa, maxYa = min([yas, yae]), max([yas, yae])
    minXb, maxXb = min([xbs, xbe]), max([xbs, xbe])
    minYb, maxYb = min([ybs, ybe]), max([ybs, ybe])
    if X >= minXa and X <= maxXa and Y >= minYa and Y <= maxYa and X >= minXb and X <= maxXb and Y >= minYb and Y <= maxYb:
        return X, Y
    else:
        return -1, -1

def findParallel(t):
    parallList = []
    for i in range(len(t)):
        for j in range(i + 1, len(t)):
            if (abs(t[i] - t[j]) <= 1):
                parallList.append([i, j])
    return parallList

def findPerpendicular(parallList):
    perpendList = []
    for i in range(len(parallList)):
        for j in range(i + 1, len(parallList)):
            tA = (t[parallList[i][0]] + t[parallList[i][1]]) / 2.
            tB = (t[parallList[j][0]] + t[parallList[j][1]]) / 2.
            if abs(tA + tB - 180) < 10 or abs(abs(tA - tB) - 90) < 10:
                perpendList.append([i, j])
    return perpendList

t = np.array([140, 68, 39, 165, 140, 69, 40, 165])
r = np.array([172, 188, 262, 409, 419, 461, 508, 545])
x1 = np.array([289, 0, 334, 138, -33, 0, 661, -2])
y1 = np.array([0, 171, 0, 0, 0, 460, 0, 0])
x2 = np.array([718, 512, -79, 275, 396, 512, 231, 135])
y2 = np.array([512, -35, 512, 512, 512, 263, 512, 512])

parallList = findParallel(t)
perpendList = findPerpendicular(parallList)


plt.figure()
rectList = []
for parallPair in perpendList:
    rect = []
    for idxA in parallList[parallPair[0]]:
        for idxB in parallList[parallPair[1]]:
            plt.plot([x1[idxA],x2[idxA]], [512-y1[idxA], 512-y2[idxA]], "b-")
            plt.plot([x1[idxB],x2[idxB]], [512-y1[idxB], 512-y2[idxB]], "b-")
            X, Y = getIntersect(x1[idxA], y1[idxA], x2[idxA], y2[idxA], x1[idxB], y1[idxB], x2[idxB], y2[idxB])
            plt.plot(X, 512-Y, "bo")
            rect.append([X, Y])

    rectList.append(rect)
plt.axis('equal')
plt.show()

rectList2 = []
for rect in rectList:
    pts = np.array(rect)
    xc = pts[:, 0].mean()
    yc = pts[:, 1].mean()
    ang = np.arctan2(pts[:, 1] - yc, pts[:, 0] - xc)
    ang[ang < 0] += 2*np.pi

    ptIdx = np.argsort(ang)
    rect2 = []
    for i in ptIdx:
        rect2.append(list(pts[i]))
    rect2.append(rect2[0])
    rectList2.append(rect2)

inner = []
for rectIdxA in range(len(rectList2)):
    for rectIdxB in range(rectIdxA + 1, len(rectList2)):
        rectA = rectList2[rectIdxA]
        rectB = rectList2[rectIdxB]
        for pt in rectA[:-1]:
            if isPinRectangle(np.array(rectB)[:-1], pt[0], pt[1]):
                plt.plot(pt[0], 512-pt[1], "ro")
                inner.append([pt[0], pt[1]])

        for i in range(len(rectA) - 1):
            for j in range(len(rectB) - 1):
                X, Y = getIntersect(rectA[i][0], rectA[i][1], rectA[i+1][0], rectA[i+1][1], rectB[j][0], rectB[j][1], rectB[j+1][0], rectB[j+1][1])
                if X > 0 and Y > 0:#and edge[int(Y)-5:int(Y)+5, int(X)-5:int(X)+5].std() > 0:
                    plt.plot([rectA[i][0], rectA[i+1][0]], [512-rectA[i][1], 512-rectA[i+1][1]], "b-")
                    plt.plot([rectB[j][0], rectB[j+1][0]], [512-rectB[j][1], 512-rectB[j+1][1]], "r-")
                    plt.plot(X, 512-Y, "bo")
                    inner.append([X, Y])
plt.axis('equal')
plt.show()



pts = np.array(inner)
xc = pts[:, 0].mean()
yc = pts[:, 1].mean()
ang = np.arctan2(pts[:, 1] - yc, pts[:, 0] - xc)
ang[ang < 0] += 2*np.pi
ptIdx = np.argsort(ang)
inner2 = []
for i in ptIdx:
    inner2.append(list(pts[i]))
inner2.append(inner2[0])

inner2 = np.array(inner2)
plt.plot(inner2[:, 0], inner2[:, 1], "r-")
rectInner = 0.5*np.abs(np.dot(inner2[:-1, 0],np.roll(inner2[:-1, 1],1))-np.dot(inner2[:-1, 1],np.roll(inner2[:-1, 0],1)))
xx, yy = np.hsplit((inner2[:-1, :] - np.roll(inner2[:-1, :], -1, axis=0)), 2)
periInner = np.hypot(yy, xx).sum()

rect = np.array(rectList2[0])
plt.plot(rect[:, 0], rect[:, 1], "g-")
rectAreaA = 0.5*np.abs(np.dot(rect[:-1, 0],np.roll(rect[:-1, 1],1))-np.dot(rect[:-1, 1],np.roll(rect[:-1, 0],1)))
xx, yy = np.hsplit((rect[:-1, :] - np.roll(rect[:-1, :], -1, axis=0)), 2)
periRectA = np.hypot(yy, xx).sum()

rect = np.array(rectList2[1])
plt.plot(rect[:, 0], rect[:, 1], "b-")
rectAreaB = 0.5*np.abs(np.dot(rect[:-1, 0],np.roll(rect[:-1, 1],1))-np.dot(rect[:-1, 1],np.roll(rect[:-1, 0],1)))
xx, yy = np.hsplit((rect[:-1, :] - np.roll(rect[:-1, :], -1, axis=0)), 2)
periRectB = np.hypot(yy, xx).sum()
plt.axis('equal')
plt.show()

print rectAreaA, rectAreaB, rectInner
print rectAreaA + rectAreaB - rectInner
print periRectA + periRectB - periInner
