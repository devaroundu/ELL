####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     cntk_import.py (importers)
##  Authors:  Chris Lovett
##
##  Requires: Python 3.x
##
####################################################################################################

import sys
import os
import numpy as np
import cv2
import argparse
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '../../utilities/pythonlibs'))
import find_ell
import cntk_to_ell
import ell_utilities
import json
import ziptools

def main(argv):
    arg_parser = argparse.ArgumentParser(
        "Converts CNTK model to ELL model\n"
        "Example:\n"
        "    cntk_import.py VGG16_ImageNet_Caffe.model\n"
        "This outputs 'VGG16_ImageNet_Caffe.ell' and 'VGG16_ImageNet_Caffe_config.json'\n")

    arg_parser.add_argument("cntk_model_file", help="path to a CNTK model file, or a zip archive of a CNTK model file")
    arg_parser.add_argument("--zip_ell_model", help="zips the output ELL model if set", action="store_true")

    args = arg_parser.parse_args(argv)

    # extract the model if it's in an archive
    unzip = ziptools.Extractor(args.cntk_model_file)
    success, filename = unzip.extract_file(".cntk")
    if (success):
        print("extracted: " + filename)
    else:
        # not a zip archive
        filename = args.cntk_model_file

    predictor = cntk_to_ell.predictor_from_cntk_model(filename)

    input_shape = predictor.GetInputShape()
    output_shape = predictor.GetOutputShape()
    
    model_file_name = os.path.splitext(filename)[0]+'.ell'
    head, tail = os.path.split(model_file_name)

    ell_map = ell_utilities.ell_map_from_float_predictor(predictor)
    print("Saving model file: '" + model_file_name + "'")
    ell_map.Save(model_file_name)

    if (args.zip_ell_model):
        print("Zipping model file: '" + model_file_name + ".zip'")
        zipper = ziptools.Zipper()
        zipper.zip_file(model_file_name, model_file_name + ".zip")
        os.remove(model_file_name)

if __name__ == "__main__":
    argv = sys.argv
    argv.pop(0) # when passed directly into parse_args, the first argument (program name) is not skipped
    main(argv)
