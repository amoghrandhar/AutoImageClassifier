#!/bin/bash

MAIN_Feature_FUNCTION="./build/tools/my_Extract_Features.bin"
#MAIN_Feature_FUNCTION="./build/tools/extract_features.bin"
#echo $MAIN_Feature_FUNCTION

#CAFFE_MODEL="models/bvlc_reference_caffenet/bvlc_reference_caffenet.caffemodel"							#CAFFE reference model
CAFFE_MODEL="/media/amogh/Data/NeuralNet_Models/VGG_19/VGG_ILSVRC_19_layers.caffemodel"							#VGG_19 model
echo $CAFFE_MODEL

#MODEL_LAYER_TO_USE="fc8"
MODEL_LAYER_TO_USE="fc7"
#echo $MODEL_LAYER_TO_USE

#PROTO="examples/_temp/imagenet_val.prototxt"										#Caffe refernce prototext
PROTO="/media/amogh/Data/NeuralNet_Models/VGG_19/VGG_ILSVRC_19_layers_deploy.prototxt"					#VGG 19 prototext
#echo ${PROTO}


GPU="GPU"
#echo $GPU


$MAIN_Feature_FUNCTION "VGG_NET_MODEL_MEAN" $CAFFE_MODEL $PROTO $MODEL_LAYER_TO_USE $GPU

# @prg_name @caffeModelName @caffe_model @proto.txt @model_layer_fc7 
