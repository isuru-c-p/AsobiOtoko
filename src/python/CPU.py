from z80 import *
import types
#this file is classes  for compatibility to let the AsobiOtoko emulator
#utilise unit tests from pyboy python gameboy emulator.


class _Registers(object): # should act more or less like a dictionary
	def __init__(self,z80):
		self.z80 = z80
		self.regMap8 = {'a' : REGA ,'b' : REGB,'c':REGC,'d': REGD,'e' : REGE,'f' :REGF,'h':REGH,'l':REGL }
		self.regMap16 = {'pc' : PC ,'sp' : SP, 'mem':REGMEM}
	def __getitem__(self,key):
		if key in self.regMap8:
			return getReg(self.z80,self.regMap8[key])
		elif key in self.regMap16:
			return getReg16(self.z80,self.regMap16[key])
		else:
			raise Exception
	def __setitem__(self,key,value):
		if key in self.regMap8:
			setReg(self.z80,self.regMap8[key],value)
		elif key in self.regMap16:
			setReg16(self.z80,self.regMap16[key],value)
		else:
			raise Exception
	def __contains__(self,item):
		return (item in self.regMap8) or (item in self.regMap16)
			
class _MMU(object):
	size = 2**16
	def __init__(self,cpu):
		self.mmu = cpu.mmu
	def rb(self,addr):
		return rb(self.mmu,addr)
	def wb(self,addr,val):
		wb(self.mmu,addr,val)
	def rw(self,addr):
		return rw(self.mmu,addr)
	def ww(self,addr,val):
		ww(self.mmu,addr,val)


class fakemethod(object):
	def __init__(self,func,name):
		self.func = func
		self.__name__ = name
	def __call__(self,arg):
		return self.func(arg.cpu)

class CPU(object):
	def __init__(self):
		self.cpu = z80()
		self.flags = {'zero' : 7, 'sub' : 6, 'halfcarry' : 5, 'carry' : 4 }
		initZ80(self.cpu)
		self.registers = _Registers(self.cpu)
		self.mmu = _MMU(self.cpu)
		names = [x for x in globals().keys() if (x.startswith('i_') or x.startswith('i2_') )]
		for n in names:
			cfunc = globals()[n]
			method = fakemethod(cfunc,n)
			self.__setattr__(n,method)
	def setFlag(self, **kwargs): 
		self.setFlags(filter(lambda key : kwargs[key],kwargs))
	
	def setFlags(self,flagsIn):
		mask = reduce(lambda x,flag : x | (1 << self.flags[flag]),flagsIn,0 )
		self.registers['f'] |= mask
	
	def resetFlags(self, flagsIn):
		mask = reduce(lambda x,flag : x | (1 << self.flags[flag]),flagsIn,0 )
		self.registers['f'] &= ~mask
		
	def getBit(self,reg,bit):
		return ((self.registers[reg] >> bit) & 0x1)

	def setBit(self,r,bit):
		self.registers[r] |=  ((1 << bit) & 255 )

	def getFlag(self, flagName):
		return self.getBit('f',self.flags[flagName])
	def resetBit(self,r,bit):
		self.registers[r] &=  (~(1<<bit) & 255)	
		
	def setMemoryBit(self,h,l,bit):
		h,l = self.registers[h],self.registers[l]
		self.loadRegFromMemory('mem',h,l)
		self.setBit('mem',bit)
		self.writeMemory(h,l,self.registers['mem'])

	def getMemoryBit(self,h,l,bit):
		h,l = self.registers[h],self.registers[l]
		self.loadRegFromMemory('mem',h,l)
		return self.getBit('mem',bit)
		
	def resetMemoryBit(self,h,l,bit):
		h,l = self.registers[h],self.registers[l]
		self.loadRegFromMemory('mem',h,l)
		self.resetBit('mem',bit)
		self.writeMemory(h,l,self.registers['mem'])
		
	def push(self,im):
		if im < 256:
			push(self.cpu,im)
		else:
			push(self.cpu,(im>>8)&255)
			push(self.cpu,im&255)
			
	def pop(self,reg):
		pop(self.cpu,reg)
		
		
	def loadRegFromMemory(self,dest, high, low, twobytes=0):
		if len(dest)==2 and (dest not in self.registers.keys()):
			val = self.mmu.rw((high<<8)+low)
			self.registers[dest[0]] = (val >> 8)
			self.registers[dest[1]] = (val & 255)
		else:
			if twobytes == 0:
				self.registers[dest] = self.mmu.rb((high<<8)+low)
			else:
				self.registers[dest] = self.mmu.rw((high<<8)+low)
		
	def writeMemory(self,high, low, value, twobytes=0):
		if twobytes:
			self.mmu.ww((high<<8)+low, value)
		else:
			self.mmu.wb((high<<8)+low, value)
		
	#convenience function, you can index the cpu[''] and get either a byte or a register	
	def __getitem__(self, index):
		if type(index) == str:
			if index in self.registers:#(len(index)==1) or (index == 'pc') or (index == 'sp') or (index == 'mem'):
				return self.registers[index]
			elif (len(index) == 2) :
				#if index[0] in self.registers and index[1] in self.registers:
				return (self.registers[index[0]]<<8) + self.registers[index[1]]
		elif type(index) == int:
			if not 0 <= index < self.mmu.size :
				raise Exception("cpu btye access index out of range")
			return self.mmu.rb(index)
		else:
			raise Exception("Cannot index the cpu object with key %s"%index)
			
	#convenience function, you can index the cpu[''] and set either a byte or a register
	# be careful as size of value isnt checked so you could overflow an 8 bit register with unpredictable
	#results
	def __setitem__(self, index,val):
		if type(index) == str:
			if index in self.registers:
				self.registers[index] = val & (2**(len(index)*8) - 1)
			elif index[0] in self.registers and index[1] in self.registers:
				self.registers[index[0]] = (val >> 8) & 255
				self.registers[index[1]] = val & 255
		elif type(index) == int:
			if not 0 <= index < self.mmu.size :
				raise Exception("cpu btye access index out of range")
			return self.mmu.wb(index,val)
		else:
			raise Exception("Cannot index the cpu object with key %s"%index)
		
