import sys
if '--pycnn-viz' in sys.argv:
    from pycnn_viz import *
else:
    def PrintGraphviz(**kwarge):
        print "Run with --pycnn-viz to get the visualization behavior."
    from _pycnn import *
