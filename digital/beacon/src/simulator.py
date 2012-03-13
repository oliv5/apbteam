# simu - Beacon simulation. {{{
#
# Copyright (C) 2011 Florent DUCHON
#
# APBTeam:
#        Web: http://apbteam.org/
#      Email: team AT apbteam DOT org
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
# }}}

"""Graphic interface for beacon simulator."""

import re
from Tkinter import *
from simu.inter.drawable import *
from subprocess import Popen, PIPE
from math import pi
import math
import random
import decimal
import time
	
import fcntl
import os
from subprocess import *
	
	
class Obstacle:
    def __init__ (self, id, pos, radius, factor):
	self.id = id
        self.pos = pos
        self.radius = radius
        self.factor = factor

    def move (self, pos):
        self.pos = pos


class Beacon (Drawable):
	def __init__ (self,onto, num, pos, orientation, size):
		Drawable.__init__ (self, onto)
		self.id = num 			# ID
		self.pos = pos 			# position (x,y)
		self.orientation = orientation 	# orientation in degrees
		self.size = size		# side size
		self.angle = {}			# obstacles angle
		self.mode = 0			# 0 = deactivated
						# 1 = activated
	def draw_beacon (self): 
		# Color depends of the beacon mode
		if self.mode is 1:
			color = 'black'
		else:
			color = 'red'
		# Draw it
		self.draw_rectangle ((self.pos[0]-self.size/2,self.pos[1]-self.size/2),(self.pos[0]+self.size/2,self.pos[1]+self.size/2), fill = color)
	
	def draw_axes (self):
		self.draw_line (self.pos,(self.pos[0]+(300)*math.cos(math.radians(self.orientation)),self.pos[1]+300*math.sin(math.radians(self.orientation))),fill='red', arrow = LAST)
		self.draw_line (self.pos,(self.pos[0]+(300)*math.cos(math.radians(self.orientation+90)),self.pos[1]+300*math.sin(math.radians(self.orientation+90))),fill='red', arrow = LAST)
		
	def toogle_mode (self): 
		if self.mode is 0:
			self.mode = 1
		else:
			self.mode = 0


class Area (Drawable):
	def __init__ (self, onto, border_min, border_max):
		Drawable.__init__ (self, onto)
		self.border_min = border_min
		self.border_max = border_max
		self.border = None
		self.phantoms_counter = 0
		self.obstacles = [ ]
		self.beacons = [ ] 
		self.phantoms = [ ] # virtual obstacles computed by the beacon module.

	def draw (self):
		self.reset ()
		self.draw_rectangle (self.border_min, self.border_max, fill = 'grey')
		self.draw_rectangle ((self.border_min[0],self.border_max[1]-500),(self.border_min[0]+500,self.border_max[1]), fill = 'blue')
		self.draw_rectangle ((self.border_max[0]-500,self.border_max[1]-500), self.border_max, fill = 'red')
		self.draw_line ((0,0),(150,0),fill='black', arrow = LAST)
		self.draw_line ((0,0),(0,150),fill='black', arrow = LAST)

		for b in self.beacons:
			if b.pos is not None:
				b.draw_beacon ()
		for o in self.obstacles:
			if o.pos is not None:
				self.draw_circle (o.pos, o.radius,fill = o.factor and 'gray50' or 'gray25')
		for p in self.phantoms:
			if p is not None:
				self.draw_circle (p, 20,fill = 0 and 'gray50' or 'red')

	def show_angles (self):
		for b in self.beacons:
			for o in self.obstacles:
				self.draw_line ((b.pos[0],b.pos[1]),(o.pos[0],o.pos[1]),fill='cyan', arrow = NONE)

	def populate (self):
		self.obstacles.append (Obstacle (1,(random.randrange(100,2700),random.randrange(100,1700)), 200, 0))
		self.obstacles.append (Obstacle (2,(random.randrange(100,2700), random.randrange(100,1700)), 200, 0))
		
		#self.obstacles.append (Obstacle (1,(1666,1411), 200, 0))
		#self.obstacles.append (Obstacle (2,(2604,344), 200, 0))
		#self.obstacles.append (Obstacle ((500, 500), 200, 0))

		self.beacons.append (Beacon (self, 1, (-40,2040), 270,80))
		self.beacons.append (Beacon (self, 2, (-40,-40),0,80))
		self.beacons.append (Beacon (self, 3, (3040,1000), 180,80))

	def add_phantom (self,center):
		# Only take care of the 100 latest value. Delete previous one.
		if self.phantoms_counter is 100:
			del self.phantoms[0]
		else:
			self.phantoms_counter += 1
		self.phantoms.append(center)


class AreaView (DrawableCanvas):
	def __init__ (self, border_min, border_max, master = None):
		self.border_min = border_min
		self.border_max = border_max
		width = border_max[0] - border_min[0]
		height = border_max[1] - border_min[0]
		DrawableCanvas.__init__ (self, width * 1.3, height * 1.3, -width / 2,-height / 2,master, borderwidth = 1, relief = 'sunken',			background = 'white')
		self.area = Area (self, border_min, border_max)
		self.area.populate ()

	def draw (self):
		self.area.draw ()
		
	def add_phantom (self,center):
		self.area.add_phantom(center)


class beacon_simu (Frame):
	def __init__ (self, border_min, border_max, master = None):
		Frame.__init__ (self, master)
		self.pack (expand = 1, fill = 'both')
		self.createWidgets (border_min, border_max)
		self.robot_pos.set("Robot position = (0 , 0)")
		self.phantom_pos.set("Last phantom position = ")
		args = []
		args[0:0] = [ './beacon.host' ]
		self.p = Popen (args,shell=True, stdin = PIPE, stdout = PIPE)
		self.counter = 0
		self.valeur_ko = 0
		self.valeur_ok = 0
		self.total = 0
		
	def createWidgets (self, border_min, border_max):
		# Bottom Panel
		self.bottomFrame = Frame (self)
		self.bottomFrame.pack (side = 'bottom', fill = 'both')
		
		# First subPanel for display options
		self.subPanel1 = Frame (self.bottomFrame)
		self.subPanel1.pack (side = 'left', fill = 'both')
		self.display_axes = IntVar ()
		self.axesButton = Checkbutton (self.subPanel1,variable = self.display_axes, command = self.update,text = 'Display axes', indicatoron = True)
		self.axesButton.pack (anchor='w')
		self.display_angles = IntVar ()
		self.anglesButton = Checkbutton (self.subPanel1,variable = self.display_angles, command = self.update,text = 'Display angles', indicatoron = True)
		self.anglesButton.pack (anchor='w')	
	
		# Second subPanel for simulator options
		self.subPanel2 = Frame (self.bottomFrame)
		self.subPanel2.pack (side = 'left', fill = 'both')
		self.mode = StringVar()
		self.manualRadioButton = Radiobutton (self.subPanel2, text = 'Manual', variable = self.mode, value='manual').pack(anchor='w')
		self.autoRadioButton = Radiobutton (self.subPanel2, text = 'Auto', variable = self.mode, value='auto').pack(anchor='w')
		
		# Third subPanel for simulator options
		self.subPanel3 = Frame (self.bottomFrame)
		self.subPanel3.pack (side = 'left', fill = 'both')
		self.precisionScale = Scale (self.subPanel3, label = 'Precision', orient = 'horizontal', from_ = 1, to = 3)
		self.precisionScale.pack ()
		
		# Fourth subPanel for buttons
		self.subPanel4 = Frame (self.bottomFrame)
		self.subPanel4.pack (side = 'left', fill = 'both')
		self.clearPhantomsButton = Button (self.subPanel4, text = 'Clear', command = self.clear_phantoms)
		self.clearPhantomsButton.pack (side = 'bottom')
		self.startButton = Button (self.subPanel4, text = 'Start', command = self.start)
		self.startButton.pack (side = 'top')
		
		# Fifth subPanel for Label
		self.subPanel5 = Frame (self.bottomFrame)
		self.subPanel5.pack (side = 'left', fill = 'both')
		self.robot_pos = StringVar()
		self.label = Label(self.subPanel5, textvariable=self.robot_pos).pack(anchor='w')
		self.phantom_pos = StringVar()
		self.label = Label(self.subPanel5, textvariable=self.phantom_pos).pack(anchor='w')
		
		# Sixth subPanel for Exit button
		self.subPanel6 = Frame (self.bottomFrame)
		self.subPanel6.pack (side = 'right', fill = 'both')
		self.quitButton = Button (self.subPanel6, text = 'Quit', command = self.exit)
		self.quitButton.pack (side = 'right', fill = 'both')		
		self.recoveryButton = Button (self.subPanel6, text = 'Recovery', command = self.recovery)
		self.recoveryButton.pack (side = 'right', fill = 'both')				
		self.areaview = AreaView (border_min, border_max, self)
		self.areaview.pack (expand = True, fill = 'both')
		self.areaview.bind ('<1>', self.click)
		
	def clear (self):
		self.areaview.area.draw ()

	def clear_phantoms (self):
		del self.areaview.area.phantoms[:]
		self.areaview.area.phantoms_counter = 0
		self.update()

	def update (self):
		self.areaview.area.update ()
		self.areaview.area.draw ()
		if self.display_angles.get() is 1:
			self.areaview.area.show_angles()
		if self.display_axes.get() is 1:
			for b in self.areaview.area.beacons:
				b.draw_axes()

	def click (self, ev):
		pos = self.areaview.screen_coord ((ev.x, ev.y))
		# Update obstacles position
		for o in self.areaview.area.obstacles:
			if  self.areaview.area.border_min[0] < pos[0] < self.areaview.area.border_max[0] and self.areaview.area.border_min[1] < pos[1] < self.areaview.area.border_max[1]:
				dx = o.pos[0] - pos[0]
				dy = o.pos[1] - pos[1]
				if dx * dx + dy * dy < 300*300:
					print "Obstacle trouve"
					o.pos = pos
				self.robot_pos.set("Robot position = (%.0f , %.0f)" % pos)

		# Check beacon mode
		for b in self.areaview.area.beacons:
			dx = abs(b.pos[0] - pos[0])
			dy = abs(b.pos[1] - pos[1])
			if dx < b.size and dy < b.size:
				b.toogle_mode ()
		# Update area
		self.update ()

	def call_algorithm (self,num,angle,angleID):
		temp = []
		self.p.stdin.write(str(num)+'\n')
		self.p.stdin.flush()
		self.p.stdin.write(str(angle)+'\n')
		self.p.stdin.flush()
		self.p.stdin.write(str(angleID)+'\n')
		self.p.stdin.flush()
		for o in self.areaview.area.obstacles:
			x = self.p.stdout.readline().split('\n')
			y= self.p.stdout.readline().split('\n')
			trust = self.p.stdout.readline().split('\n')
			temp.append([x,y,trust])
		return temp

	def rotate_beacons (self): # Simulate a rotation for a all beacons, ie set beacon.angles.	
		# Set the requested imprecision
 		imprecision = self.precisionScale.get () #1 to 3 degrees
		imprecision = int (math.radians(imprecision)*1000)
		imprecision = decimal.Decimal(random.randrange(-imprecision,imprecision))/1000
		#imprecision = 0
		# Compute angles for every beaconss
		for b in self.areaview.area.beacons:
			for o in self.areaview.area.obstacles:
				if b.id is 1:
					b.angle[o.id] = math.atan(float(o.pos[0])/(float(2000)-float(o.pos[1])))+float(imprecision)
				if b.id is 2:
					b.angle[o.id] = math.atan(float(o.pos[0])/float(o.pos[1]))+float(imprecision)
					#print math.degrees(b.angle[o.id])
				if b.id is 3:
					b.angle[o.id] = math.atan((float(3000)-float(o.pos[0]))/(float(1000)-float(o.pos[1])))+float(imprecision)
					if b.angle[o.id] < 0:
						b.angle[o.id] = pi - abs(b.angle[o.id])


	def manual_mode (self):
		# Manual mode : warning : two beacons must already be activated
		self.rotate_beacons ()
		for b in self.areaview.area.beacons:
			if b.mode is 1:
					phantom_pos = self.call_algorithm(b.id,b.angle[1],1)
					phantom_pos = self.call_algorithm(b.id,b.angle[2],2)
		self.areaview.add_phantom((int (phantom_pos[0][0]),int (phantom_pos[0][1])))
		self.areaview.add_phantom((int (phantom_pos[1][0]),int (phantom_pos[1][1])))
		#self.phantom_pos.set("Last phantom position = (%.0f , %.0f)" %(float(phantom_pos[0]),float(phantom_pos[1])))
		self.update ()

	def automatic_mode (self):
		# Automatic  mode : all beacons are used
		self.rotate_beacons ()
		select = random.randrange(1,4)
		for b in self.areaview.area.beacons:
			if b.id is select:
				temp = 1
				for o in self.areaview.area.obstacles:
					phantom_pos = self.call_algorithm(b.id,b.angle[temp],temp)
					for o in self.areaview.area.obstacles:
						if phantom_pos[temp-1][2][0] is not '0':
							self.areaview.add_phantom((int (phantom_pos[temp-1][0][0]),int (phantom_pos[temp-1][1][0])))
					temp = temp + 1




	def automatic_mode_old (self):
		# Automatic  mode : all beacons are used
		self.rotate_beacons ()
		select = random.randrange(1,4)
		for b in self.areaview.area.beacons:
			if b.id is select:
				phantom_pos = self.call_algorithm(b.id,b.angle[1],1)
				if phantom_pos[0][2][0] is not '0':
					self.areaview.add_phantom((int (phantom_pos[0][0][0]),int (phantom_pos[0][1][0])))
				if phantom_pos[1][2][0] is not '0':
					self.areaview.add_phantom((int (phantom_pos[1][0][0]),int (phantom_pos[1][1][0])))
				phantom_pos = self.call_algorithm(b.id,b.angle[2],2)
				# Draw the computed position
				if phantom_pos[0][2][0] is not '0':
					self.areaview.add_phantom((int (phantom_pos[0][0][0]),int (phantom_pos[0][1][0])))
				if phantom_pos[1][2][0] is not '0':
					self.areaview.add_phantom((int (phantom_pos[1][0][0]),int (phantom_pos[1][1][0])))


	def automatic_mode_for_recovery_test (self):
		# Automatic  mode : all beacons are used
		self.rotate_beacons ()
		trust = 0
		select = random.randrange(1,4)
		for b in self.areaview.area.beacons:
			if b.id is select:
				phantom_pos = self.call_algorithm(b.id,b.angle[1],1)
				if phantom_pos[0][2][0] is not '0':
					self.areaview.add_phantom((int (phantom_pos[0][0][0]),int (phantom_pos[0][1][0])))
					trust += 1
				if phantom_pos[1][2][0] is not '0':
					self.areaview.add_phantom((int (phantom_pos[1][0][0]),int (phantom_pos[1][1][0])))
					trust += 1
				if trust != 0:
					return trust
				phantom_pos = self.call_algorithm(b.id,b.angle[2],2)
				# Draw the computed position
				if phantom_pos[0][2][0] is not '0':
					print phantom_pos
					self.areaview.add_phantom((int (phantom_pos[0][0][0]),int (phantom_pos[0][1][0])))
					trust += 1
				if phantom_pos[1][2][0] is not '0':
					self.areaview.add_phantom((int (phantom_pos[1][0][0]),int (phantom_pos[1][1][0])))
					trust += 1
				return trust

	def start (self) :
		if self.mode.get() == "manual":
			self.manual_mode()
			return
		if self.mode.get() == "auto":
			#for o in self.areaview.area.obstacles:
				#x = o.pos[0] + float (decimal.Decimal(random.randrange(-5,40)))
				#y = o.pos[1] + float (decimal.Decimal(random.randrange(-25,25)))
				#o.pos = (x,y)
			self.automatic_mode()
			self.after (30,self.start)
			self.update()
		else:
			print "No mode selected"
			return
	def exit (self):
		self.p.kill()
		self.quit()

	def recovery (self):
		recovery = 0
		self.clear_phantoms()
		while recovery == 0:
			recovery = self.automatic_mode_for_recovery_test()
		#self.update()
		#print self.areaview.area.phantoms
		self.update()
		for o in self.areaview.area.obstacles:
			#print "obstacle"
			#print "o.pos[0] = %d - self.areaview.area.phantoms[0][0] = %d" %(o.pos[0],self.areaview.area.phantoms[0][0])
			dx1 = o.pos[0] - self.areaview.area.phantoms[0][0]
			dy1 = o.pos[1] - self.areaview.area.phantoms[0][1]
			dx2 = o.pos[0] - self.areaview.area.phantoms[1][0]
			dy2 = o.pos[1] - self.areaview.area.phantoms[1][1]
			total1 = dx1 * dx1 + dy1 * dy1
			total2 = dx2 * dx2 + dy2 * dy2
			x  = random.randrange(100,2700)
			y = random.randrange(100,1700)
			o.pos = (x,y)
			self.total += 1	
			if total1 < 90000 or total2 < 90000:
				#print "1 trouve"
				self.valeur_ok+=1
			else:
				#print "1 pas trouve"
				self.valeur_ko+=1
		print "#######################################"
		print self.areaview.area.phantoms
		print "OK  = %d" %(self.valeur_ok)
		print "KO  = %d" %(self.valeur_ko)
		print "Total = %d" %(self.total)
		print "New position (%d,%d) (%d,%d)" %(self.areaview.area.obstacles[0].pos[0],self.areaview.area.obstacles[0].pos[1],self.areaview.area.obstacles[1].pos[0],self.areaview.area.obstacles[1].pos[1])
		print "#######################################"
		#time.sleep(10)
		#self.clear_phantoms()
		self.after (100,self.recovery)

	



if __name__ == '__main__':
	app = beacon_simu ((0, 0), (3000, 2000))	
	app.mainloop ()
