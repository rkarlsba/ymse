#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai

mpv \
	-vo=gpu \
	--gpu-context=drm \
    --input-cursor=no \
    --input-vo-keyboard=no \
    --input-default-bindings=no \
    --hwdec=drm \
    --untimed \
    --no-cache \
    --profile=low-latency \
    --opengl-glfinish=yes \
    --opengl-swapinterval=0 \
    --gpu-hwdec-interop=drmprime-drm \
    --drm-draw-plane=overlay \
    --drm-drmprime-video-plane=primary \
    --framedrop=no \
    --speed=1.01 \
    --video-latency-hacks=yes \
    --opengl-glfinish=yes \
    --opengl-swapinterval=0 \
    tcp://10.0.0.1:1234\?listen
