# -*- mode: python ; coding: utf-8 -*-

import sys
import gooey
gooey_root = os.path.dirname(gooey.__file__)
gooey_languages = Tree(os.path.join(gooey_root, 'languages'), prefix = 'gooey/languages')
gooey_images = Tree(os.path.join(gooey_root, 'images'), prefix = 'gooey/images')


grazer_executable = "grazer"
if sys.platform.startswith("win"):
    grazer_executable += ".exe"

block_cipher = None

a = Analysis(
    ['gui.py'],
    binaries=[("./bin/"+grazer_executable, "./bin")],
    hiddenimports=[],
    hookspath=None,
    runtime_hooks=None,
)

pyz = PYZ(a.pure, a.zipped_data, cipher=block_cipher)

options = [('u', None, 'OPTION')]

exe = EXE(
    pyz,
    a.scripts,
    a.binaries,
    a.zipfiles,
    a.datas,
    options,
    gooey_languages, # Add them in to collected files
    gooey_images, # Same here.
    name='grazer_gui',
    debug=False,
    strip=None,
    upx=True,
    console=False,
    icon=os.path.join(gooey_root, 'images', 'program_icon.ico')
)
