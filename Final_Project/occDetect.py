#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""Detect the occluded object points."""
import numpy as np
import pandas as pd
from scipy.cluster.vq import kmeans2
from scipy import spatial


np.set_printoptions(suppress=True)  # Disable scientific notation for numpy


def occDetect(EOFileName, ptFileName, cleanedPtFileName, occlusionPtFileName):
    """Seperate normal and occluded points then output them respectively."""
    print "Processing point file: " + ptFileName + "..."
    # XL, YL, ZL, O, P, K, SigXL, SigYL, SigZL, SigO, SigP, SigK
    EO = np.genfromtxt(EOFileName)

    # Get the 3D edge points and build KD-tree with image coordinates
    ptSet = pd.read_csv(ptFileName, delimiter=';').values
    X, Y, Z, R, G, B, row, col, id = np.hsplit(ptSet, 9)
    tree = spatial.cKDTree(np.dstack((col, row)).reshape(-1, 2))

    # Get the candidate set of occluded points
    k = 8
    dis, idx = tree.query(
        np.dstack((col, row)).reshape(-1, 2),
        k=k+1)

    # Compute distance from exposure station to object points
    disEO = np.hypot(
        X[idx].reshape(-1, k+1) - EO[0],
        Y[idx].reshape(-1, k+1) - EO[1],
        Z[idx].reshape(-1, k+1) - EO[2])

    # First step of occlusion detection. In this step, we classify the
    # candidate point sets with distance to camera. If the standard
    # deviation is relatively big, then label these points as occluded area.
    disStd = disEO.std(axis=1)
    label1 = kmeans2(disStd*100, np.array([0, 10]), 2)[1]     # Use centmeter
    occSet = disEO[label1 == 1]
    occSetIdx = idx[label1 == 1]    # Index of occluded point sets

    # Second step of occlusion detection. We normalize the labeled distances
    # firstly, then classify these point set again. Points with higher index
    # value will be labeled as occluded object points.
    occSetStd = disStd[label1 == 1]
    outSetMin = occSet.min(axis=1)

    # Normalized Index = (D_i - min(D_i) / std(D_i))
    NI = (occSet - outSetMin.reshape(-1, 1)) / occSetStd.reshape(-1, 1)

    label2 = kmeans2(NI.ravel(), np.array([0, 2]), 2)[1]
    idx = np.unique(occSetIdx.ravel()[label2 == 1])
    isInOccArea = np.zeros((len(ptSet)), np.bool)
    isInOccArea[idx] = True

    idx = np.unique(occSetIdx.ravel()[label2 == 0])
    isOccObj = np.zeros((len(ptSet)), np.bool)
    isOccObj[idx] = True

    # Write out the results
    np.savetxt(
        occlusionPtFileName,
        ptSet[isInOccArea],
        fmt="%.6f %.6f %.6f %d %d %d %.6f %.6f %d",
        header="X Y Z R G B row col id",
        comments='')

    # Write out the results
    np.savetxt(
        occlusionPtFileName.replace("_occ", "_obj"),
        ptSet[isOccObj],
        fmt="%.6f %.6f %.6f %d %d %d %.6f %.6f %d",
        header="X Y Z R G B row col id",
        comments='')

    np.savetxt(
        cleanedPtFileName,
        ptSet[~isInOccArea & ~isOccObj],
        fmt="%.6f %.6f %.6f %d %d %d %.6f %.6f %d",
        header="X Y Z R G B row col id",
        comments='')


def main():
    import glob
    # Define file names
    a1 = EOFileList = glob.glob('./param/EO*')
    a2 = ptFileList = map(lambda x: x.replace("param/EO_", "ptCloud/"), EOFileList)
    a3 = cleanedPtFileList = map(lambda x: x.replace("param/EO_", "ptCloud/cleaned/").replace(".txt", "_clean.txt"), EOFileList)
    a4 = occlusionPtFileList = map(lambda x: x.replace("param/EO_", "ptCloud/occlusion/").replace(".txt", "_occ.txt"), EOFileList)

    for i in range(len(EOFileList)):
        occDetect(
            EOFileList[i],
            ptFileList[i],
            cleanedPtFileList[i],
            occlusionPtFileList[i])

if __name__ == "__main__":
    main()
