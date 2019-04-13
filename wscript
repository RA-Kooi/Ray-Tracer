#!/usr/bin/env python3
# encoding: utf-8

top = '.'
out = 'build'

def options(opt):
	opt.load('LotusWaf', tooldir='LotusWaf')

def configure(cfg):
	cfg.load('LotusWaf', tooldir='LotusWaf')

def build(bld):
	bld.recurse('3rdparty/stb')
	bld.recurse('libRay')
	bld.recurse(['Ray Tracer'])
