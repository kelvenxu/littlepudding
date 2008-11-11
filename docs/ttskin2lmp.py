#! /usr/bin/env python
import os
from gimpfu import *
def bmp2png(filename, raw_filename, png_filename, png_raw_filename):
	image = pdb.file_bmp_load(filename, raw_filename)
	layer = pdb.gimp_image_flatten(image)
	drawable = pdb.gimp_image_get_active_drawable(image)
	pdb.gimp_layer_add_alpha(layer)
	color=(255, 0, 255)
	threshold=15.0
	operation=CHANNEL_OP_REPLACE
	antialias=True
	feather=False
	feather_radius=0
	sample_merged=False
	pdb.gimp_selection_none(image)
	pdb.gimp_by_color_select(drawable, color, threshold, operation, antialias, feather, feather_radius, sample_merged)
	is_empty = pdb.gimp_selection_is_empty(image)
	if  is_empty:
		print "select nothing. done"
	else:
		pdb.gimp_edit_clear(drawable)
	pdb.file_png_save_defaults(image, drawable, png_filename, png_raw_filename)

def ttskin2lmp(path):
	filename=""
	raw_filename=""
	png_filename=""
	png_raw_filename=""
	bmps = os.listdir(path)
	for bmp in bmps:
		if os.path.isdir(bmp):
			continue
		else:
			file_ext = os.path.splitext(bmp)
			if file_ext[1] == ".bmp":
				filename =path + "/" +  bmp
				raw_filename = filename
				png_filename = path + "/" + file_ext[0] + ".png"
				png_raw_filename = png_filename
				print filename
				print raw_filename
				print png_filename
				print png_raw_filename
				bmp2png(filename, raw_filename, png_filename, png_raw_filename)
			else:
				continue
			

register(
  "ttskin2lmp", "",
  "<Toolbox>/Xtns/Script-Fu/Test/_TTskin2lmp", "",
  [
  (PF_STRING, "arg0", "argument 0", "test string"),
  ],
  [],
  ttskin2lmp
  )

main()
	
