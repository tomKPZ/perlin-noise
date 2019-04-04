#! /usr/bin/env python

import json
import matplotlib.animation
import matplotlib.pyplot
import sys

mats = json.loads(sys.stdin.read().replace(',]', ']'))

fig = matplotlib.pyplot.figure()
im = matplotlib.pyplot.imshow(mats[0])

def animate(i):
    im.set_array(mats[i % len(mats)])
    return im

anim = matplotlib.animation.FuncAnimation(fig, animate, interval=1, frames=len(mats))
anim.save('animation.mp4', fps=30, extra_args=['-vcodec', 'libx264'])
matplotlib.pyplot.show()
