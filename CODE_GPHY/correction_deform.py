# -*- coding: utf-8 -*-
import numpy as np
import cv2
import matplotlib.pyplot as plt
from skimage.transform import PiecewiseAffineTransform, warp
from skimage import io
import csv
import math

from scipy.cluster.vq import whiten, kmeans
import scipy.cluster.hierarchy as hcluster

def PointsInCircum(x, y, r,n=100):
    return [[x+math.cos(2*math.pi/n*i)*r,y+math.sin(2*math.pi/n*i)*r] for i in xrange(0,n)]
    
def findIntersections(img):
    #Source : http://stackoverflow.com/a/11366549/2949601
    imgBlur = cv2.GaussianBlur(img, (11, 11), 0)
    
    
    #Vertical lines
    sobelX = cv2.Sobel(imgBlur, cv2.CV_16S, 1, 0)
    sobelX = cv2.convertScaleAbs(sobelX)
    cv2.normalize(sobelX, sobelX, 0, 255, cv2.NORM_MINMAX)
    

    kernelx = cv2.getStructuringElement(cv2.MORPH_RECT, (3, 15))
    close = cv2.morphologyEx(sobelX, cv2.MORPH_DILATE, kernelx)
    close = cv2.threshold(close, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)[1]
    
    

    contour, hier = cv2.findContours(close, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    
    for c in contour:
        x,y,w,h = cv2.boundingRect(c)
        if h / w > 5:
            cv2.drawContours(close, [c], 0, 255, -1)
        else:
            cv2.drawContours(close, [c], 0, 0, -1)
            

    closex = cv2.morphologyEx(close, cv2.MORPH_DILATE, None, iterations = 2);
    
    
    #Horizontal lines
    sobelY = cv2.Sobel(imgBlur, cv2.CV_16S, 0, 1)
    sobelY = cv2.convertScaleAbs(sobelY)
    cv2.normalize(sobelY, sobelY, 0, 255, cv2.NORM_MINMAX)
    
    kernely = cv2.getStructuringElement(cv2.MORPH_RECT, (15, 3))
    close = cv2.morphologyEx(sobelY, cv2.MORPH_DILATE, kernely)
    close = cv2.threshold(close, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)[1]
    
    contour, hier = cv2.findContours(close, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    
    for c in contour:
        x,y,w,h = cv2.boundingRect(c)
        if w / h > 5:
            cv2.drawContours(close, [c], 0, 255, -1)
        else:
            cv2.drawContours(close, [c], 0, 0, -1)

    closey = cv2.morphologyEx(close, cv2.MORPH_DILATE, None, iterations = 2);
    
       
    
    #Find intersections
    intersections = cv2.bitwise_and(closex, closey);
    
    #Calculate mean and std-dev of width
    contour, hier = cv2.findContours(intersections, cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE)
    widths = []
    for c in contour:
        x,y,w,h = cv2.boundingRect(c)
        widths.append(w)
        
    widths = np.array(widths)
    meanWidth = np.mean(widths)
    stdD = np.std(widths);

    #Find coordinates
    points = []
    for c in contour:
        x,y,w,h = cv2.boundingRect(c)
        if w >= (meanWidth - stdD) and w <= (meanWidth + stdD): #Size filter
            mom = cv2.moments(c)
            p = (mom['m10'] / mom['m00'], mom['m01'] / mom['m00'])
            if p[0] >= 0 and p[1] >= 0 and p[0] <= imgBlur.shape[1] and p[1] <= imgBlur.shape[0]:
                if imgBlur[p[1], p[0]] > 35:
                    points.append(p)
    return points
    
def findLines(img, threshold = 10, plot = True):
    imgBlur = cv2.GaussianBlur(img, (5, 5), 0)
    imgT = cv2.threshold(imgBlur, 70, 255, cv2.THRESH_BINARY)[1]

    lines = cv2.HoughLines(imgT, 1, np.pi / 180, 200);
    lines = np.squeeze(lines)    
    lines = lines[lines[:,1] > 0.5]
    
    """fig, ax = plt.subplots()
    ax.imshow(imgT, cmap="gray")
    
    for rho,theta in lines:
        a = np.cos(theta)
        b = np.sin(theta)
        x0 = a*rho
        y0 = b*rho
        x1 = int(x0 + 1000*(-b))
        y1 = int(y0 + 1000*(a))
        x2 = int(x0 - 1000*(-b))
        y2 = int(y0 - 1000*(a))
    
        ax.plot([x1,x2], [y1,y2], 'm-', lw=2)    
    rows, cols = img.shape[0], img.shape[1]
    ax.axis((0, cols, rows, 0))
    plt.show()  """
    
    clusters = hcluster.fclusterdata(lines, threshold, criterion="distance")
    n_clusters = len(set(clusters))
    
    if plot:
        plt.scatter(*np.transpose(lines), c=clusters)
        title = "threshold: %f, number of clusters: %d" % (threshold, n_clusters)
        plt.title(title)
        plt.show()
    
    mean_lines=[[] for i in range(n_clusters)]
    for i in xrange(0, len(lines)):
        mean_lines[clusters[i]-1].append(list(lines[i]))
        
    return np.array([np.mean(np.array(zip(*mean_lines[i])), axis=1) for i in xrange(len(mean_lines))])
    
def moveIntersections(intersections, lines, max_dist):
    def distPointDroite(point, droite):
        return abs(droite[0]*point[0]+droite[1]*point[1]+droite[2])/math.sqrt(droite[0]**2+droite[1]**2)
    def toCartesian(line):
        rho = line[0]
        theta = line[1]
        return [np.cos(theta)/np.sin(theta), 1.0, -rho/np.sin(theta)]
    for inter in intersections:
        for l in lines:
            eq = toCartesian(l)
            dist = distPointDroite(inter, eq)

def rotateImage(image, angle):
  image_center = tuple(np.array(image.shape)/2)
  rot_mat = cv2.getRotationMatrix2D(image_center,angle,1.0)
  result = cv2.warpAffine(image, rot_mat, image.shape,flags=cv2.INTER_LINEAR)
  return result    
    

plt.close('all')

img = cv2.imread("photonique_deformation/img.png", cv2.CV_LOAD_IMAGE_GRAYSCALE)
img = rotateImage(img, 15)
rows, cols = img.shape[0], img.shape[1]

inter = findIntersections(img)

#for p in inter:
#    cv2.circle(img, (int(p[0]), int(p[1])), 5, 255, 1);

lines = findLines(img)

fig, ax = plt.subplots()
ax.imshow(img, cmap="gray")
ax.plot(*zip(*inter),marker='o', color='b', ls='')

for rho,theta in lines:
    a = np.cos(theta)
    b = np.sin(theta)
    x0 = a*rho
    y0 = b*rho
    x1 = int(x0 + 1000*(-b))
    y1 = int(y0 + 1000*(a))
    x2 = int(x0 - 1000*(-b))
    y2 = int(y0 - 1000*(a))
    
    ax.plot([x1,x2], [y1,y2], 'm-', lw=2)
    #cv2.line(img, (x1, y1), (x2, y2), 255, 1)
    

#ax.axis((-360, 360, min(lines[:, 1]), max(lines[:, 1])))
    


#ax.plot(tform.inverse(src)[:, 0], tform.inverse(src)[:, 1], '.b')
ax.axis((0, cols, rows, 0))
plt.show()

#src=[]
#target=[]
#
#
#
#csvfile=open('photonique_deformation/points.txt', 'r')
#csvreader = csv.reader(csvfile, delimiter=' ', quotechar='|')
#for row in csvreader:
#    src.append([float(row[0]), float(row[1])])
#    target.append([float(row[2]), float(row[3])])
#    
#csvfile.close()
#
#print("%i points"%size(src))
#
#cercle=PointsInCircum(img.shape[0]/2, img.shape[1]/2, img.shape[0]/2, 200)
#src+=cercle
#target+=cercle
#
#
#src=np.array(src)
#target=np.array(target)
#
##Y
#tform = PiecewiseAffineTransform()
#tform.estimate(target, src)
#
#out = warp(img, tform, output_shape=(rows, cols))
#
#fig, ax = plt.subplots()
#ax.imshow(out, cmap="gray")
##ax.plot(tform.inverse(src)[:, 0], tform.inverse(src)[:, 1], '.b')
#ax.axis((0, cols, rows, 0))
#plt.show()
#
#mapX=np.zeros((rows, cols))
#mapY=np.zeros((rows, cols))
#
#X, Y=np.meshgrid(np.arange(0, cols), np.arange(0, rows))
#coords = np.dstack([X.flat, Y.flat])[0]
#warp_map = tform(coords)
#mapX[coords[:,0], coords[:,1]]=warp_map[:, 0]
#mapY[coords[:,0], coords[:,1]]=warp_map[:, 0]
#
#mapX=np.interp(mapX, [-1, max(cols, rows)], [0, 65535])
#cv2.imwrite("mapX.png", mapX.astype(np.uint16))
#
#mapY=np.interp(mapY, [-1, max(cols, rows)], [0, 65535])
#cv2.imwrite("mapY.png", mapY.astype(np.uint16))