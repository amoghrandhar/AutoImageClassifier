#!/bin/bash

OUTPUT_Lines="$(wc -l < examples/_temp/cdc.txt)"
#echo $OUTPUT_Lines

MAIN_Feature_FUNCTION="./build/tools/my_Extract_Features.bin"
#MAIN_Feature_FUNCTION="./build/tools/extract_features.bin"
#echo $MAIN_Feature_FUNCTION

#CAFFE_MODEL="models/bvlc_reference_caffenet/bvlc_reference_caffenet.caffemodel"							#CAFFE reference model
CAFFE_MODEL="/media/amogh/Data/NeuralNet_Models/VGG_19/VGG_ILSVRC_19_layers.caffemodel"							#VGG_19 model
echo $CAFFE_MODEL

MODEL_LAYER_TO_USE="fc7"
#echo $MODEL_LAYER_TO_USE

#IMAGENET_MEAN_PROTO="examples/_temp/imagenet_val.prototxt"										#Caffe refernce prototext
IMAGENET_MEAN_PROTO="/media/amogh/Data/NeuralNet_Models/VGG_19/VGG_ILSVRC_19_layers_deploy.prototxt"					#VGG 19 prototext
#echo ${IMAGENET_MEAN_PROTO}

DATABASE_NAME="examples/_temp/features$1"
#echo $DATABASE_NAME

DATABASE_USED="leveldb"
#echo $DATABASE_USED

GPU="GPU"
#echo $GPU

# 	0                     2           	3                 4                 5           6		7	  8	
$MAIN_Feature_FUNCTION $CAFFE_MODEL $IMAGENET_MEAN_PROTO $MODEL_LAYER_TO_USE $DATABASE_NAME $OUTPUT_Lines $DATABASE_USED $GPU

#./build/tools/extract_features.bin models/bvlc_reference_caffenet/bvlc_reference_caffenet.caffemodel examples/_temp/imagenet_val.prototxt fc7 examples/_temp/features ${OUTPUT_Lines} leveldb

# @prg_name @caffeModelName @caffe_model @proto.txt @model_layer_fc7 
