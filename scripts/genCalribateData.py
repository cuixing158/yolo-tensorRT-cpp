# -*- coding: utf-8 -*-
"""
Created on Sun Sep 27 14:35:30 2020

@author: cuixingxing
"""

import os

def genImglist(path = r'../data'):
    jpgfiles = os.listdir(path)
    with open('../configs/calibration_images.txt','w') as fid:
        for jpgfile in jpgfiles:
            if os.path.splitext(jpgfile)[1] == '.jpg':
                fid.write(path+'/'+jpgfile+'\n')
        
if __name__=='__main__':
    genImglist()
