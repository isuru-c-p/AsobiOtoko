from CPU import CPU
import unittest

#parent test class, inherit from this 
class TestCPU(unittest.TestCase):
	def SetUp(self): # automatically called but can recall
		self.cpu = CPU()
		self.cpu.mmu.bios_enabled = 0
	
	def flags_test(self, flagDict, oldFlagsReg):
		map(lambda x: self.assertEqual(self.cpu.getFlag(x), flagDict[x]), flagDict.keys())
		savedFlags = self.cpu['f']
		currentFlags = {}
		map(lambda x: currentFlags.__setitem__(x, self.cpu.getFlag(x)), self.cpu.flags)
		oldFlags = {}
		self.cpu['f'] = oldFlagsReg
		map(lambda x: oldFlags.__setitem__(x, self.cpu.getFlag(x)), self.cpu.flags)
		#print "CurrentFlags: "
		#print currentFlags
		#print "OldFlags: "
		#print oldFlags

		self.cpu['f'] = savedFlags
		map(lambda x: self.assertEqual(oldFlags[x], currentFlags[x]), filter(lambda y: y not in flagDict.keys(), oldFlags.keys()))

	
	def DecTest(self, instruction, register):
		immediate = 0xa0
		self.cpu[register] = immediate
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[register], immediate - 1)
		if len(register) == 1:
			self.flags_test({ 'sub': 1, 'halfcarry' : 1 }, oldFlags)
			immediate = 0x0
			self.cpu[register] = immediate
			oldFlags = self.cpu['f']
			instruction(self.cpu)
			self.assertEqual(self.cpu[register], (immediate - 1) & (2**(len(register)*8) - 1))
			self.flags_test({ 'sub': 1, 'halfcarry' : 1 }, oldFlags)
		else:
			self.assertEqual(self.cpu['f'], oldFlags)
		
	def IncTest(self, instruction, register):
		immediate = 0xa0
		self.cpu[register] = immediate
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[register], immediate + 1)
		if len(register) == 1:
			self.flags_test({ 'sub': 0, 'halfcarry' : 0 }, oldFlags)
			immediate = 0xff if len(register) == 1 else 0xffff
			self.cpu[register] = immediate
			oldFlags = self.cpu['f']
			instruction(self.cpu)
			self.assertEqual(self.cpu[register], (immediate + 1) & (2**(len(register)*8) - 1))
			self.flags_test({ 'zero' : 1, 'sub': 0, 'halfcarry' : 1 }, oldFlags)
		else:
			self.assertEqual(self.cpu['f'], oldFlags)
	
	def INC_XX_mem_test(self, instruction, addressReg):
		immediate = 0xa0
		address = 0xface
		self.cpu[address] = immediate
		self.cpu[addressReg] = address
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[address], immediate + 1)
		self.flags_test({'sub':0, 'halfcarry':0}, oldFlags)
		
		immediate = 0xff
		self.cpu[address] = immediate
		self.cpu[addressReg] = address
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[address], (immediate + 1) & 255)
		self.flags_test({'zero': 1, 'sub':0, 'halfcarry':1}, oldFlags)
		
	def DEC_XX_mem_test(self, instruction, addressReg):
		immediate = 0xa0
		address = 0xface
		self.cpu[address] = immediate
		self.cpu[addressReg] = address
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[address], immediate - 1)
		self.flags_test({'sub':1, 'halfcarry':1}, oldFlags)	
		
		immediate = 0x00
		self.cpu[address] = immediate
		self.cpu[addressReg] = address
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[address], (immediate - 1) & 255)
		self.flags_test({'sub':1, 'halfcarry':1}, oldFlags)	
	
	def LD_X_n_test(self, instruction, register):
		immediate = 0b11101011
		self.cpu[self.cpu['pc']+1] = immediate
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[register], immediate)
		self.flags_test({}, oldFlags)
		
	def RRC_X_test(self, instruction, register):
		immediate = 0b11101011
		self.cpu[register] = immediate
		self.cpu.setFlag(zero=1)
		self.cpu.setFlag(halfcarry=1)
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[register], ((immediate & 0x01) << 7) + (immediate >> 1))
		self.flags_test({'carry': 1, 'sub' : 0, 'halfcarry': 0, 'zero': 0}, oldFlags)
		immediate = 0b000000000
		self.cpu[register] = immediate
		oldFlags = self.cpu['f']
		self.cpu.setFlag(sub=1)
		self.cpu.setFlag(halfcarry=1)
		instruction(self.cpu)
		self.assertEqual(self.cpu[register], ((immediate & 0x01) << 7) + (immediate >> 1))
		self.flags_test({'carry': 0, 'sub' : 0, 'halfcarry': 0, 'zero': 1}, oldFlags)

	def RRC_XX_mem_test(self, instruction, addressReg):
		immediate = 0b11101011
		address = 0x1337
		self.cpu[address] = immediate
		self.cpu[addressReg] = address
		self.cpu.setFlag(zero=1)
		self.cpu.setFlag(halfcarry=1)
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[address], ((immediate & 0x01) << 7) + (immediate >> 1))
		self.flags_test({'carry': 1, 'sub' : 0, 'halfcarry': 0, 'zero': 0}, oldFlags)
		immediate = 0b000000000
		self.cpu[address] = immediate
		oldFlags = self.cpu['f']
		self.cpu.setFlag(sub=1)
		self.cpu.setFlag(halfcarry=1)
		instruction(self.cpu)
		self.assertEqual(self.cpu[address], ((immediate & 0x01) << 7) + (immediate >> 1))
		self.flags_test({'carry': 0, 'sub' : 0, 'halfcarry': 0, 'zero': 1}, oldFlags)
	
		
	def RLC_X_test(self, instruction, register):
		immediate = 0b11101011
		self.cpu[register] = immediate
		self.cpu.setFlags(['zero', 'sub', 'halfcarry'])
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[register], ((immediate << 1) & 255) + (immediate >> 7))
		self.flags_test({'carry' : 1, 'sub': 0, 'halfcarry': 0, 'zero': 0}, oldFlags)
		immediate = 0b00000000
		self.cpu[register] = immediate
		self.cpu.setFlags(['sub', 'halfcarry'])
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[register], ((immediate << 1) & 255) + (immediate >> 7))
		self.flags_test({'carry' : 0, 'sub': 0, 'halfcarry': 0, 'zero': 1}, oldFlags)

	def RLC_XX_mem_test(self, instruction, addressReg):
		address = 0xface
		immediate = 0b11101011
		self.cpu[address] = immediate
		self.cpu[addressReg] = address
		self.cpu.setFlags(['zero', 'sub', 'halfcarry'])
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[address], ((immediate << 1) & 255) + (immediate >> 7))
		self.flags_test({'carry' : 1, 'sub': 0, 'halfcarry': 0, 'zero': 0}, oldFlags)
		immediate = 0b00000000
		self.cpu[address] = immediate
		self.cpu.setFlags(['sub', 'halfcarry'])
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[address], ((immediate << 1) & 255) + (immediate >> 7))
		self.flags_test({'carry' : 0, 'sub': 0, 'halfcarry': 0, 'zero': 1}, oldFlags)

				
		
	def LD_XX_nn_test(self, instruction, register):
		immediate = 0xface
		self.cpu.mmu.ww(self.cpu['pc']+1, immediate)
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[register], immediate) 
		self.flags_test({}, oldFlags)
		
	def LD_XX_X_mem_test(self, instruction, addressReg, srcReg):
		immediate = 0xde
		self.cpu[addressReg] = 0xbeef
		self.cpu[srcReg] = immediate
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu.mmu.rw(0xbeef), immediate)
		self.flags_test({}, oldFlags)
	
	def LD_XX_n_mem_test(self, instruction, destAddressReg):
		immediate = 0xde
		address = 0x1337
		self.cpu[self.cpu['pc']+1] = immediate
		self.cpu[destAddressReg] = address
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[address], immediate)
		self.flags_test({}, oldFlags)
	
	def LD_X_nn_test(self, instruction, dest):
		immediate = 0xde
		address = 0x1337
		self.cpu.mmu.ww(self.cpu['pc']+1, address) 
		self.cpu[address] = immediate
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[dest], immediate)
		self.flags_test({}, oldFlags)
		
	def RL_X_test(self, instruction, register):
		immediate = 0xad
		self.cpu[register] = immediate
		oldcarry = self.cpu.getFlag('carry')
		self.cpu.setFlags(['sub','halfcarry'])
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[register], ((immediate << 1) & 255) + oldcarry)
		self.flags_test({'carry': (immediate >> 7), 'sub': 0, 'halfcarry': 0, 'zero': 0}, oldFlags)
		immediate = 0x0
		self.cpu[register] = immediate
		self.cpu.resetFlags(['carry'])
		oldcarry = self.cpu.getFlag('carry')
		oldFlags = self.cpu['f']
		self.cpu.setFlags(['sub','halfcarry'])
		instruction(self.cpu)
		self.assertEqual(self.cpu[register], ((immediate << 1) & 255) + oldcarry)
		self.flags_test({'carry': 0, 'sub': 0, 'halfcarry': 0, 'zero': 1}, oldFlags)

	def RL_XX_mem_test(self, instruction, addressReg):
		address = 0xeeff
		immediate = 0xad
		self.cpu[address] = immediate
		self.cpu[addressReg] = address 
		oldcarry = self.cpu.getFlag('carry')
		self.cpu.setFlags(['sub','halfcarry'])
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[address], ((immediate << 1) & 255) + oldcarry)
		self.flags_test({'carry': (immediate >> 7), 'sub': 0, 'halfcarry': 0, 'zero': 0}, oldFlags)
		immediate = 0x0
		self.cpu[address] = immediate
		self.cpu.resetFlags(['carry'])
		oldcarry = self.cpu.getFlag('carry')
		oldFlags = self.cpu['f']
		self.cpu.setFlags(['sub','halfcarry'])
		instruction(self.cpu)
		self.assertEqual(self.cpu[address], ((immediate << 1) & 255) + oldcarry)
		self.flags_test({'carry': 0, 'sub': 0, 'halfcarry': 0, 'zero': 1}, oldFlags)

	
	def RR_X_test(self, instruction, register):
		immediate = 0b11010110
		self.cpu[register] = immediate
		oldcarry = self.cpu.getFlag('carry')
		self.cpu.setFlags(['sub', 'halfcarry'])
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[register], (immediate >> 1) + (oldcarry << 7))
		self.flags_test({'carry': (immediate & 0x01), 'sub': 0, 'halfcarry': 0, 'zero': 0}, oldFlags)
		immediate = 0
		self.cpu[register] = immediate
		self.cpu.setFlags(['sub', 'halfcarry'])
		self.cpu.resetFlags(['carry'])
		oldcarry = self.cpu.getFlag('carry')
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[register], (immediate >> 1) + (oldcarry << 7))
		self.flags_test({'carry': 0, 'sub': 0, 'halfcarry': 0, 'zero': 1}, oldFlags)

	def RR_XX_mem_test(self, instruction, addressReg):
		address = 0xface
		immediate = 0b11010110
		self.cpu[address] = immediate
		self.cpu[addressReg] = address
		oldcarry = self.cpu.getFlag('carry')
		self.cpu.setFlags(['sub', 'halfcarry'])
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[address], (immediate >> 1) + (oldcarry << 7))
		self.flags_test({'carry': (immediate & 0x01), 'sub': 0, 'halfcarry': 0, 'zero': 0}, oldFlags)
		immediate = 0
		self.cpu[address] = immediate
		self.cpu.setFlags(['sub', 'halfcarry'])
		self.cpu.resetFlags(['carry'])
		oldcarry = self.cpu.getFlag('carry')
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[address], (immediate >> 1) + (oldcarry << 7))
		self.flags_test({'carry': 0, 'sub': 0, 'halfcarry': 0, 'zero': 1}, oldFlags)

				
	def ADD_X_X_test(self, instruction, reg1, reg2):
		immediate = 0x01
		immediate2 = 0x02 if (reg1 != reg2) else immediate
		self.cpu[reg1] = immediate
		self.cpu[reg2] = immediate2 
		self.cpu.setFlags(['sub', 'halfcarry', 'carry', 'zero'])
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[reg1], immediate + immediate2)
		self.flags_test({'carry':0, 'zero':0, 'sub':0, 'halfcarry':0}, oldFlags)
		immediate = 0xac
		immediate2 = 0xff if (reg1 != reg2) else immediate
		self.cpu[reg1] = immediate
		self.cpu[reg2] = immediate2 
		self.cpu.setFlags(['sub', 'zero'])
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[reg1], (immediate + immediate2) & 255)
		self.flags_test({'carry':1, 'zero':0, 'sub':0, 'halfcarry':1}, oldFlags)
		
		immediate = 0x01
		immediate2 = 0x0F if (reg1 != reg2) else immediate
		self.cpu[reg1] = immediate
		self.cpu[reg2] = immediate2 
		self.cpu.setFlags(['sub', 'carry', 'zero'])
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[reg1], immediate + immediate2)
		self.flags_test({'carry':0, 'zero':0, 'sub':0, 'halfcarry':(reg1 != reg2)}, oldFlags)
		
		
	
	def ADD_XX_XX_test(self, instruction, reg1, reg2):
		immediate = 0xac
		immediate2 = 0xa0bc if (reg1 != reg2) else 0xac
		self.cpu[reg1] = immediate
		self.cpu[reg2] = immediate2
		self.cpu.setFlags(['zero', 'sub'])
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[reg1], (immediate + immediate2) & 65535)
		self.flags_test({'carry':0, 'sub':0, 'zero':1, 'halfcarry':1, 'carry':0}, oldFlags)
		
		
		immediate = 0xffff
		immediate2 = 0xa0 if (reg1 != reg2) else 0xffff
		self.cpu.resetFlags(['zero'])
		self.cpu.setFlags(['sub'])
		self.cpu[reg1] = immediate
		self.cpu[reg2] = immediate2
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[reg1], (immediate + immediate2) & 65535)
		self.flags_test({'carry':1, 'sub':0, 'zero':0, 'halfcarry':1, 'carry':1}, oldFlags)

		immediate = 0x00
		immediate2 = 0x00 
		self.cpu.resetFlags(['zero'])
		self.cpu.setFlags(['sub'])
		self.cpu[reg1] = immediate
		self.cpu[reg2] = immediate2
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[reg1], (immediate + immediate2) & 65535)
		self.flags_test({'carry':0, 'sub':0, 'zero':0, 'halfcarry':0, 'carry':0}, oldFlags)
		
		
	def ADD_X_XX_mem_test(self, instruction, destReg, addressReg):
		immediate = 0xea
		immediate2 = 0xce
		address = 0xbaaa
		self.cpu[addressReg] = address
		self.cpu[address] = immediate
		self.cpu[destReg] = immediate2
		self.cpu.setFlags(['sub'])
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[destReg], (immediate + immediate2) & 255)
		self.flags_test({'carry':1, 'sub':0, 'halfcarry':1, 'zero':0}, oldFlags)
		
		immediate2 = 0x01
		self.cpu[destReg] = immediate2
		self.cpu.setFlags(['sub'])
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[destReg], (immediate + immediate2) & 255)
		self.flags_test({'carry':0, 'sub':0, 'halfcarry':0, 'zero':0}, oldFlags)
	
		immediate2 = 0x06
		self.cpu[destReg] = immediate2
		self.cpu.setFlags(['sub'])
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[destReg], (immediate + immediate2) & 255)
		self.flags_test({'carry':0, 'sub':0, 'halfcarry':1, 'zero':0}, oldFlags)
			
	
	def ADC_X_XX_mem_test(self, instruction, destReg, addressReg):
		immediate = 0xef
		immediate2 = 0xce
		address = 0xbaaa
		self.cpu[addressReg] = address
		self.cpu[address] = immediate
		self.cpu[destReg] = immediate2
		self.cpu.setFlags(['zero'])
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[destReg], (immediate + immediate2) & 255)
		self.flags_test({'carry': 1, 'sub':0, 'halfcarry':1, 'zero':0}, oldFlags)
		
		immediate2 = 0x01
		self.cpu[destReg] = immediate2
		self.cpu.setFlags(['zero'])
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[destReg], (immediate + immediate2 + 1) & 255)
		self.flags_test({'carry': 0, 'sub':0, 'halfcarry':1, 'zero':0}, oldFlags)

		immediate2 = (0xff - 0xef) + 1
		self.cpu[destReg] = immediate2
		self.cpu.resetFlags(['zero'])
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[destReg], (immediate + immediate2) & 255)
		self.flags_test({'carry': 1, 'sub':0, 'halfcarry':1, 'zero':0}, oldFlags)
		#TODO: find out whether zero should be set for this or not for above case
			
	
	def ADC_X_X_test(self, instruction, destReg, srcReg):
		immediate = 0xff
		immediate2 = 0xce if (destReg != srcReg) else immediate
		self.cpu[destReg] = immediate
		self.cpu[srcReg] = immediate2
		self.cpu.setFlags(['zero', 'sub'])
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[destReg], (immediate + immediate2) & 255)
		self.flags_test({'carry': 1, 'zero': 0, 'sub':0, 'halfcarry': 1}, oldFlags)
		
		immediate = 0x01
		immediate2 = 0x05 if (destReg != srcReg) else immediate
		self.cpu.setFlags(['carry', 'zero', 'sub'])
		oldFlags = self.cpu['f']
		self.cpu[destReg] = immediate
		self.cpu[srcReg] = immediate2
		instruction(self.cpu)
		self.assertEqual(self.cpu[destReg], (immediate + immediate2 + 1))
		self.flags_test({'carry':0, 'zero':0, 'sub':0, 'halfcarry':0}, oldFlags)
		
		immediate = 0x00
		immediate2 = 0x00 if (destReg != srcReg) else immediate
		self.cpu.setFlags(['sub', 'halfcarry'])
		oldFlags = self.cpu['f']
		self.cpu[destReg] = immediate
		self.cpu[srcReg] = immediate2
		instruction(self.cpu)
		self.assertEqual(self.cpu[destReg], (immediate + immediate2))
		self.flags_test({'carry':0, 'zero':1, 'sub':0, 'halfcarry':0}, oldFlags)
				
	
	def LD_X_XX_mem_test(self, instruction, destReg, addressReg):
		address = 0xface
		immediate = 0xbe
		self.cpu[address] = immediate
		self.cpu[addressReg] = address
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[destReg], immediate)
		self.flags_test({}, oldFlags)
	
	def LDI_XX_X_mem_test(self, instruction, addressReg, srcReg):
		address = 0xadfe
		immediate = 0x37
		self.cpu[addressReg] = address
		self.cpu[srcReg] = immediate
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[address], immediate)
		self.assertEqual(self.cpu[addressReg], address + 1)
		self.flags_test({}, oldFlags)
	
	def LDI_X_XX_mem_test(self, instruction, destReg, addressReg):
		address = 0xacfe
		immediate = 0xde
		self.cpu[address] = immediate
		self.cpu[addressReg] = address
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[destReg], immediate)
		self.assertEqual(self.cpu[addressReg], address + 1)
		self.flags_test({}, oldFlags)

	def LDD_XX_X_test(self, instruction, addressReg, srcReg):
		address = 0xaabb
		immediate = 0xcd
		self.cpu[srcReg] = immediate
		self.cpu[addressReg] = address
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[address], immediate)
		self.assertEqual(self.cpu[addressReg], address - 1)
		self.flags_test({}, oldFlags)

	def LDD_X_XX_test(self, instruction, destReg, addressReg):
		address = 0xface
		immediate = 0xdd
		self.cpu[address] = immediate
		self.cpu[addressReg] = address
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[destReg], immediate)
		self.assertEqual(self.cpu[addressReg], address - 1)
		self.flags_test({}, oldFlags)

	def LD_X_X_test(self, instruction, destReg, srcReg):
		immediate = 0xfc
		immediate2 = 0xaa if (destReg != srcReg) else 0xfc
		self.cpu[srcReg] = immediate
		self.cpu[destReg] = immediate2
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[destReg], immediate)
		self.flags_test({}, oldFlags)

	def LD_XX_X_test(self, instruction, addressReg, srcReg):
		if srcReg not in ['h','l']:
			immediate = 0xfa
		elif srcReg == 'h':
			immediate = 0xbe
		else:
			immediate = 0xef
		address = 0xbeef
		self.cpu[addressReg] = address
		self.cpu[srcReg] = immediate
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[address], immediate)
		self.flags_test({}, oldFlags)

	def SBC_X_X_test(self, instruction, dest, src):
		immediate = 0xfa
		immediate2 = 0xff if (dest != src) else immediate
		self.cpu[dest] = immediate
		self.cpu[src] = immediate2
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[dest], (immediate - immediate2) & 255)
		if (dest != src):
			self.flags_test({'carry':1, 'halfcarry':1, 'zero':0, 'sub':1}, oldFlags)
		else:
			self.flags_test({'carry':0, 'halfcarry':0, 'zero':1, 'sub':1}, oldFlags)
		carry = self.cpu.getFlag('carry')
		immediate2 = 0x01 if (dest != src) else immediate
		self.cpu[dest] = immediate
		self.cpu[src] = immediate2 if (dest != src) else immediate
		self.cpu.resetFlags(['sub'])
		self.cpu.setFlags(['zero'])
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[dest], (immediate - immediate2 - carry) & 255)
		if (dest != src):
			self.flags_test({'carry':0, 'halfcarry': 0, 'sub':1, 'zero':0}, oldFlags)
		else:
			self.flags_test({'carry':0, 'halfcarry': 0, 'sub':1, 'zero':1}, oldFlags)
		

	def SBC_X_XX_mem_test(self, instruction, dest, addressReg):
		address = 0xacdf
		immediate = 0xbe
		immediate2 = 0xff
		self.cpu[addressReg] = address
		self.cpu[address] = immediate2
		self.cpu[dest] = immediate
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[dest], (immediate - immediate2) & 255)
		self.flags_test({'carry':1, 'halfcarry':1, 'sub':1, 'zero':0}, oldFlags)
		immediate2 = 0x0f
		self.cpu[dest] = immediate
		self.cpu[address] = immediate2
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[dest], (immediate - immediate2 - 1) & 255)
		self.flags_test({'carry':0, 'halfcarry':1, 'sub':1, 'zero':0}, oldFlags)
		

	def SUB_X_X_test(self, instruction, dest, src):
		immediate = 0xfa
		immediate2 = 0xce if (dest != src) else immediate
		self.cpu[dest] = immediate
		self.cpu[src] = immediate2
		self.cpu.setFlags(['zero', 'halfcarry', 'carry'])
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[dest], immediate - immediate2)
		self.flags_test({'carry':0, 'zero': (dest == src), 'sub':1, 'halfcarry': (dest != src)}, oldFlags) 
		
		# test no half carry
		immediate = 0x0a
		immediate2 = 0xf0 if (dest != src) else immediate
		self.cpu[dest] = immediate
		self.cpu[src] = immediate2
		self.cpu.setFlags(['halfcarry', 'carry'])
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[dest], (immediate - immediate2) & 255)
		self.flags_test({'carry':(dest != src), 'zero': (dest == src), 'sub':1, 'halfcarry': 0}, oldFlags) 
				
		# test negative 
		immediate2 = 0xff if (dest != src) else immediate
		self.cpu[dest] = immediate
		self.cpu[src] = immediate2 
		oldFlags = self.cpu['f']
		instruction(self.cpu)
		self.assertEqual(self.cpu[dest], (immediate - immediate2) & 255)
		self.flags_test({'carry': (dest!=src), 'halfcarry': (dest!=src), 'sub':1, 'zero': (dest==src)}, oldFlags)
		
	
	def SUB_X_XX_mem_test(self, instruction, dest, addressReg):
		immediate = 0xfa
		immediate2 = 0xce
		address = 0x1337
		self.cpu[addressReg] = address
		self.cpu[address] = immediate2
		self.cpu[dest] = immediate
		instruction(self.cpu)
		self.assertEqual(self.cpu[dest], immediate - immediate2)
		self.assertEqual(self.cpu.getFlag('carry'), 0)
		immediate2 = 0xff
		self.cpu[address] = immediate2
		self.cpu[dest] = immediate
		instruction(self.cpu)
		self.assertEqual(self.cpu[dest], (immediate - immediate2) & 255)
		self.assertEqual(self.cpu.getFlag('carry'), 1)
			
	def AND_X_X_test(self, instruction, dest, src):
		immediate = 0xad
		immediate2 = 0xce if (dest != src) else immediate
		self.cpu[dest] = immediate
		self.cpu[src] = immediate2
		instruction(self.cpu)
		self.assertEqual(self.cpu[dest], immediate & immediate2)

	def AND_X_XX_mem_test(self, instruction, dest, addressReg):
		immediate = 0xfa
		immediate2 = 0xce
		address = 0x1337
		self.cpu[dest] = immediate
		self.cpu[address] = immediate2
		self.cpu[addressReg] = address
		instruction(self.cpu)
		self.assertEqual(self.cpu[dest], immediate & immediate2)

	def XOR_X_X_test(self, instruction, dest, src):
		immediate = 0xad
		immediate2 = 0xce if (dest != src) else immediate
		self.cpu[dest] = immediate
		self.cpu[src] = immediate2
		instruction(self.cpu)
		self.assertEqual(self.cpu[dest], immediate ^ immediate2)		

	def XOR_X_XX_mem_test(self, instruction, dest, addressReg):
		immediate = 0xfa
		immediate2 = 0xce
		address = 0x1337
		self.cpu[dest] = immediate
		self.cpu[address] = immediate2
		self.cpu[addressReg] = address
		instruction(self.cpu)
		self.assertEqual(self.cpu[dest], immediate ^ immediate2)	

	def OR_X_X_test(self, instruction, dest, src):
		immediate = 0xad
		immediate2 = 0xce if (dest != src) else immediate
		self.cpu[dest] = immediate
		self.cpu[src] = immediate2
		instruction(self.cpu)
		self.assertEqual(self.cpu[dest], immediate | immediate2)			

	def OR_X_XX_mem_test(self, instruction, dest, addressReg):
		immediate = 0xfa
		immediate2 = 0xce
		address = 0x1337
		self.cpu[dest] = immediate
		self.cpu[address] = immediate2
		self.cpu[addressReg] = address
		instruction(self.cpu)
		self.assertEqual(self.cpu[dest], immediate | immediate2)

	def CP_n_test(self, instruction):
		immediate = 0xce
		immediate2 = 0xfa 
		self.cpu['a'] = immediate
		self.cpu[self.cpu['pc']+1] = immediate2
		instruction(self.cpu)
		self.assertEqual(self.cpu['a'], immediate)
		self.assertEqual(self.cpu.getFlag('zero'), 0)
		self.assertEqual(self.cpu.getFlag('carry'), 1)
		immediate = 0xce
		immediate2 = 0xce 
		self.cpu['a'] = immediate
		self.cpu[self.cpu['pc']+1] = immediate2
		instruction(self.cpu)
		self.assertEqual(self.cpu['a'], immediate)
		self.assertEqual(self.cpu.getFlag('zero'), 1)
		self.assertEqual(self.cpu.getFlag('carry'), 0)
		immediate = 0xfa
		immediate2 = 0xce 
		self.cpu['a'] = immediate
		self.cpu[self.cpu['pc']+1] = immediate2
		instruction(self.cpu)
		self.assertEqual(self.cpu['a'], immediate)
		self.assertEqual(self.cpu.getFlag('zero'), 0)
		self.assertEqual(self.cpu.getFlag('carry'), 0)			
		

	def CP_X_X_test(self, instruction, dest, src):
		immediate = 0xce
		immediate2 = 0xfa if (dest != src) else immediate
		self.cpu[dest] = immediate
		self.cpu[src] = immediate2
		instruction(self.cpu)
		self.assertEqual(self.cpu[dest], immediate)
		if (dest != src):
			self.assertEqual(self.cpu.getFlag('zero'), 0)
			self.assertEqual(self.cpu.getFlag('carry'), 1)
		else:
			self.assertEqual(self.cpu.getFlag('zero'), 1)
			self.assertEqual(self.cpu.getFlag('carry'), 0)
		immediate = 0xce
		immediate2 = 0xce if (dest != src) else immediate
		self.cpu[dest] = immediate
		self.cpu[src] = immediate2
		instruction(self.cpu)
		self.assertEqual(self.cpu[dest], immediate)
		self.assertEqual(self.cpu.getFlag('zero'), 1)
		self.assertEqual(self.cpu.getFlag('carry'), 0)
		immediate = 0xfa
		immediate2 = 0xce if (dest != src) else immediate
		self.cpu[dest] = immediate
		self.cpu[src] = immediate2 if (dest != src) else immediate
		instruction(self.cpu)
		self.assertEqual(self.cpu[dest], immediate)
		if (dest != src):
			self.assertEqual(self.cpu.getFlag('zero'), 0)
			self.assertEqual(self.cpu.getFlag('carry'), 0)		
		else:
			self.assertEqual(self.cpu.getFlag('zero'), 1)
			self.assertEqual(self.cpu.getFlag('carry'), 0)				

	def CP_X_XX_mem_test(self, instruction, dest, addressReg):
		immediate = 0xce
		immediate2 = 0xfa
		address = 0xbeef
		self.cpu[addressReg] = address
		self.cpu[dest] = immediate
		self.cpu[address] = immediate2
		instruction(self.cpu)
		self.assertEqual(self.cpu[dest], immediate)
		self.assertEqual(self.cpu.getFlag('zero'), 0)
		self.assertEqual(self.cpu.getFlag('carry'), 1)
		immediate = 0xce
		immediate2 = 0xce
		self.cpu[dest] = immediate
		self.cpu[address] = immediate2
		instruction(self.cpu)
		self.assertEqual(self.cpu[dest], immediate)
		self.assertEqual(self.cpu.getFlag('zero'), 1)
		self.assertEqual(self.cpu.getFlag('carry'), 0)
		immediate = 0xfa
		immediate2 = 0xce
		self.cpu[dest] = immediate
		self.cpu[address] = immediate2
		instruction(self.cpu)
		self.assertEqual(self.cpu[dest], immediate)
		self.assertEqual(self.cpu.getFlag('zero'), 0)
		self.assertEqual(self.cpu.getFlag('carry'), 0)			

	def RelJumpTest(self, instruction, condition, twobytes=0):
		#no jump
		immediate = 0b01010111 if (twobytes == 0) else 0x0ace
		oldPC = self.cpu['pc']
		if (twobytes == 0):
			self.cpu[oldPC+1] = immediate
		else:
			self.cpu.mmu.ww(oldPC+1, immediate)
	
		if condition[0] != 'N' and condition != ".":
			self.cpu.resetFlags([condition])
		elif condition != ".":
			self.cpu.setFlags([condition[1:]])
			
		instruction(self.cpu)
		if (condition != "."):
			if (instruction.__name__.find("_JP") == -1):
				self.assertEqual(self.cpu['pc'], oldPC + 2) #TODO: Is this right?
			else:
				self.assertEqual(self.cpu['pc'], oldPC + 3) 
		else:
			if (instruction.__name__.find("_JP") == -1):
				self.assertEqual(self.cpu['pc'], oldPC + immediate + 2)
			else:
				self.assertEqual(self.cpu['pc'], immediate)
		# jump
		immediate = 0b01010111 if (twobytes == 0) else 0x0ace
		oldPC = self.cpu['pc']
		if (twobytes == 0):
			self.cpu[oldPC+1] = immediate
		else:
			self.cpu.mmu.ww(oldPC+1, immediate)
		
		if condition[0] == "N" and condition != ".":
			self.cpu.resetFlags([condition[1:]])
		elif condition != ".":
			self.cpu.setFlags([condition])
			
		instruction(self.cpu)
		if (instruction.__name__.find("_JP") == -1):
			self.assertEqual(self.cpu['pc'], oldPC + immediate + 2)
		else:
			self.assertEqual(self.cpu['pc'], immediate)
		# negative jump
		immediate = 0b11010111 if (twobytes == 0) else 0xfffe
		oldPC = self.cpu['pc']
		if (twobytes == 0):
			self.cpu[oldPC+1] = immediate
		else:
			self.cpu.mmu.ww(oldPC+1, immediate)
		
		if condition[0] == "N":
			self.cpu.resetFlags([condition[1:]])
		elif condition != ".":
			self.cpu.setFlags([condition])
			
		instruction(self.cpu)
		if (instruction.__name__.find("_JP") == -1):
			self.assertEqual(self.cpu['pc'], oldPC - ((~immediate + 1) & (2**(8*(twobytes+1)) - 1)) + 2)		
		else:
			self.assertEqual(self.cpu['pc'], immediate)

	def RET_test(self, instruction, condition):
		retAddress = 0x1337
		self.cpu.push(retAddress)
		# return
		if (condition[0] == 'N') and (condition != "."):
			self.cpu.resetFlags([condition[1:]])
		elif (condition != "."):
			self.cpu.setFlags([condition])
		instruction(self.cpu)
		self.assertEqual(self.cpu['pc'], retAddress)
		
		# no return 
		if condition != ".":
			self.cpu['pc'] = 0
			if (condition[0] == 'N'):
				self.cpu.setFlags([condition[1:]])
			else:
				self.cpu.resetFlags([condition])
			instruction(self.cpu)
			self.assertEqual(self.cpu['pc'], 1)
	
	def CALL_nn_test(self, instruction, condition):
		# call
		currentAddress = 0x1337
		callAddress = 0xface
		if condition == ".":
			pass
		elif condition[0] == "N":
			self.cpu.resetFlags([condition[1:]])
		else:
			self.cpu.setFlags([condition])
		
		self.cpu['pc'] = currentAddress
		self.cpu.mmu.ww(self.cpu['pc']+1, callAddress)
		instruction(self.cpu)
		self.assertEqual(self.cpu['pc'], callAddress)
		self.assertEqual(self.cpu.mmu.rw(self.cpu['sp']), currentAddress+3)
	
		# no call
		currentAddress = 0x1337
		callAddress = 0xface
		if condition == ".":
			return
		elif condition[0] == "N":
			self.cpu.setFlags([condition[1:]])
		else:
			self.cpu.resetFlags([condition])
		
		self.cpu['pc'] = currentAddress
		self.cpu.mmu.ww(self.cpu['pc']+1, callAddress)
		instruction(self.cpu)
		self.assertEqual(self.cpu['pc'], currentAddress+3)		
	
	def POP_XX_test(self, instruction, dest):
		immediate = 0xface
		self.cpu.push(immediate)
		instruction(self.cpu)
		self.assertEqual(self.cpu[dest], immediate)

	def PUSH_XX_test(self, instruction, src):
		immediate = 0xbeef
		self.cpu[src] = immediate
		instruction(self.cpu)
		self.assertEqual(self.cpu.mmu.rw(self.cpu['sp']), immediate)

	def ADD_X_n_test(self, instruction, dest, signed=0):
		# no carry
		immediate = 0xbe
		immediate2 = 0x01
		self.cpu[dest] = immediate
		self.cpu[self.cpu['pc']+1] = immediate2
		instruction(self.cpu)
		self.assertEqual(self.cpu[dest], (immediate + immediate2) & (2**(8*len(dest))-1))
		self.assertEqual(self.cpu.getFlag('carry'), 0)
		
		# carry
		immediate = 0xbe
		immediate2 = 0xff
		self.cpu[dest] = immediate
		self.cpu[self.cpu['pc']+1] = immediate2
		instruction(self.cpu)
		if signed == 0:
			self.assertEqual(self.cpu[dest], (immediate + immediate2) & (2**(8*len(dest))-1))
		else:
			self.assertEqual(self.cpu[dest], (immediate - ((~immediate2+1)&255)) & (2**(8*len(dest))-1))
		self.assertEqual(self.cpu.getFlag('carry'), 1 if len(dest) == 1 else 0)		


	def RST_X_test(self, instruction, address):
		currentAddress = 0x1337
		self.cpu['pc'] = currentAddress
		instruction(self.cpu)
		self.assertEqual(self.cpu['pc'], int(address))
		self.assertEqual(self.cpu.mmu.rw(self.cpu['sp']), currentAddress+1)

	def ADC_X_n_test(self, instruction, dest):
		immediate = 0xab
		immediate2 = 0xff
		self.cpu[dest] = immediate
		self.cpu[self.cpu['pc']+1] = immediate2
		instruction(self.cpu)
		self.assertEqual(self.cpu[dest], (immediate + immediate2) & 255)
		self.assertEqual(self.cpu.getFlag('carry'), 1)
		immediate = 0xab
		immediate2 = 0x01
		self.cpu[dest] = immediate
		self.cpu[self.cpu['pc']+1] = immediate2
		instruction(self.cpu)
		self.assertEqual(self.cpu[dest], (immediate + immediate2 + 1) & 255)
		self.assertEqual(self.cpu.getFlag('carry'), 0)

	def SUB_X_n_test(self, instruction, dest):
		immediate = 0xab
		immediate2 = 0xbc
		self.cpu[dest] = immediate
		self.cpu[self.cpu['pc']+1] = immediate2
		instruction(self.cpu)
		self.assertEqual(self.cpu[dest], (immediate - immediate2) & 255)
		self.assertEqual(self.cpu.getFlag('carry'), 1)
		# no carry
		immediate = 0xab
		immediate2 = 0x01
		self.cpu[dest] = immediate
		self.cpu[self.cpu['pc']+1] = immediate2
		instruction(self.cpu)
		self.assertEqual(self.cpu[dest], (immediate - immediate2) & 255)
		self.assertEqual(self.cpu.getFlag('carry'), 0)

	def SBC_X_n(self, instruction, dest):
		immediate = 0xab
		immediate2 = 0xff
		self.cpu[dest] = immediate
		self.cpu[self.cpu['pc']+1] = immediate2
		instruction(self.cpu)
		self.assertEqual(self.cpu[dest], (immediate - immediate2) & 255)
		self.assertEqual(self.cpu.getFlag('carry'), 1)
		immediate = 0xab
		immediate2 = 0x01
		self.cpu[dest] = immediate
		self.cpu[self.cpu['pc']+1] = immediate2
		instruction(self.cpu)
		self.assertEqual(self.cpu[dest], (immediate - immediate2 - 1) & 255)
		self.assertEqual(self.cpu.getFlag('carry'), 0)		

	

	#def testNibbleSwap(self):
	#	self.SetUp()
	#	forward = 0b10100001
	#	reverse = 0b00011010
	#	self.cpu['a'] = forward
	#	self.cpu.nibbleSwap('a')
	#	self.assertEqual(reverse,self.cpu['a'])
		#test memory swapping
	#	self.cpu[0] = forward
	#	self.cpu['h'],self.cpu['l'] = 0,0 # explicitly set address to low
	#	self.cpu.nibbleHLSwap()
	#	self.assertEqual(self.cpu[0],reverse)

	#def testResetSetAndGetBit(self):
	#	self.SetUp()
	#	self.cpu.setBit('a',0)
	#	self.assertTrue(self.cpu.getBit('a',0))
	#	self.assertEqual(self.cpu.registers['a'],0b00000001)
	#	self.assertFalse(self.cpu.getBit('a',5))
	#	self.cpu.setBit('a',5)
	#	self.assertTrue(self.cpu.getBit('a',5))
	#	self.assertEqual(self.cpu.registers['a'],0b00100001)
	#	self.cpu.setBit('a',7)
	#	self.assertEqual(self.cpu.registers['a'],0b10100001)
	#	self.cpu.resetBit('a',5)
	#	self.assertEqual(self.cpu.registers['a'],0b10000001)
		#self.assertRaises(Exception,self.cpu.setBit,'a',8)
		#self.assertRaises(Exception,self.cpu.setBit,'a',-2)
		#h and l start at 0
	#	self.cpu.setMemoryBit('h','l',2)
	#	self.assertEqual(self.cpu[0],0b0000100)
	#	self.assertEqual(self.cpu.getMemoryBit('h','l',1),0)
	#	self.assertEqual(self.cpu.getMemoryBit('h','l',2),1)
		
	def testResetSetAndGetFlags(self):
		self.SetUp()
		self.assertEqual(self.cpu.getFlag('zero'),0)
		self.cpu.setFlags(['zero'])
		self.assertEqual(self.cpu.getFlag('zero'),1)
		self.assertEqual(self.cpu.registers['f'],0b10000000)
		self.cpu.resetFlags(['zero'])
		self.assertEqual(self.cpu.getFlag('zero'),0)
		self.assertEqual(self.cpu.registers['f'], 0)
		self.cpu.setFlags(['zero'])
		self.cpu.setFlags(['sub'])
		self.assertEqual(self.cpu.registers['f'], 0b11000000)
		
	def test_i_NOP(self):
		self.SetUp()
		oldFlags = self.cpu['f']
		self.cpu.i_NOP(self.cpu)
		self.flags_test({}, oldFlags)
		
	def test_i_LD_BC_nn(self):
		self.SetUp()
		immediate = 0b0100111010110001
		self.cpu.mmu.ww(self.cpu.registers['pc']+1, immediate)
		self.cpu.i_LD_BC_nn(self.cpu)
		self.assertEqual(self.cpu.registers['c'], immediate & 255)
		self.assertEqual(self.cpu.registers['b'], immediate >> 8)
		
		immediate = 0b0000000010010110
		
		self.cpu.mmu.ww(self.cpu.registers['pc']+1, immediate)
		self.cpu.i_LD_BC_nn(self.cpu)
		self.assertEqual(self.cpu.registers['c'], immediate & 255)
		self.assertEqual(self.cpu.registers['b'], immediate >> 8)
		
	def test_i_LD_BC_A_mem(self):
		self.SetUp()
		address = 0xdeab
		immediate = 0b11101011
		self.cpu['a'] = immediate
		self.cpu['c'] = address & 255
		self.cpu['b'] = address >> 8
		self.cpu.i_LD__BC__A(self.cpu)
		self.assertEqual(self.cpu.mmu.rb(address), immediate)
		
	def test_i_INC_BC(self):
		self.SetUp()
		immediate = 0b1110101100010100
		self.cpu['c'] = immediate & 255
		self.cpu['b'] = immediate >> 8
		self.cpu.i_INC_BC(self.cpu)
		self.assertEqual((self.cpu['b']<<8) + self.cpu['c'], immediate + 1)
		immediate = 0xaa
		self.cpu['c'] = immediate & 255
		self.cpu['b'] = immediate >> 8
		self.cpu.i_INC_BC(self.cpu)
		self.assertEqual((self.cpu['b']<<8) + self.cpu['c'], immediate + 1)
	
	def test_i_INC_B(self):
		self.SetUp()
		immediate = 0b11101011
		self.cpu['b'] = immediate
		oldFlags = self.cpu['f']
		self.cpu.i_INC_B(self.cpu)
		self.assertEqual(self.cpu['b'], immediate + 1)
		self.flags_test({ 'sub': 0, 'halfcarry' : 0}, oldFlags)
		immediate = 0xff
		self.cpu['b'] = immediate
		oldFlags = self.cpu['f']
		self.cpu.i_INC_B(self.cpu)
		self.assertEqual(self.cpu['b'], (immediate+1) & 255)
		self.flags_test({ 'zero' : 1, 'sub' : 0, 'halfcarry' : 1}, oldFlags)
		
	def test_i_DEC_B(self):
		self.SetUp()
		immediate = 0b11101011
		self.cpu['b'] = immediate
		self.cpu.i_DEC_B(self.cpu)
		self.assertEqual(self.cpu['b'], immediate - 1)
		#self.assertEqual(self.cpu.getFlag('carry'), 0)
		immediate = 0x00
		self.cpu['b'] = immediate
		self.cpu.i_DEC_B(self.cpu)
		self.assertEqual(self.cpu['b'], 0xff)
		#self.assertEqual(self.cpu.getFlag('carry'), 1)
		
	def test_i_LD_B_n(self):
		self.SetUp()
		self.LD_X_n_test(self.cpu.i_LD_B_n, 'b')
		
	def test_i_RLC_A(self):
		self.SetUp()
		self.RLC_X_test(self.cpu.i_RLC_A, 'a')
		
	def test_i_LD_nn_SP(self):
		self.SetUp()
		immediate = 0xfa
		self.cpu.mmu.ww(self.cpu['pc']+1, immediate)
		self.cpu['sp'] = 1337
		self.cpu.i_LD__nn__SP(self.cpu)
		self.assertEqual(self.cpu.mmu.rw(0xfa), 1337)
		
	def test_i_ADD_HL_BC(self):
		self.SetUp()
		self.ADD_XX_XX_test(self.cpu.i_ADD_HL_BC, 'hl', 'bc')
		
	def test_i_LD_A_BC_mem(self):
		self.SetUp()
		immediate = 0xa0
		address = 0xabbc
		self.cpu.mmu.wb(address, immediate)
		self.cpu['bc'] = address
		self.cpu.i_LD_A__BC_(self.cpu)
		self.assertEqual(self.cpu['a'], immediate)
	
	def test_i_DEC_BC(self):
		self.SetUp()
		self.DecTest(self.cpu.i_DEC_BC, 'bc')
		
	def test_i_INC_C(self):
		self.SetUp()
		self.IncTest(self.cpu.i_INC_C, 'c')
	
	def test_i_DEC_C(self):
		self.SetUp()
		self.DecTest(self.cpu.i_DEC_C,'c')
		
	def test_i_LD_C_n(self):
		self.SetUp()
		self.LD_X_n_test(self.cpu.i_LD_C_n, 'c')
		
	def test_i_RRC_A(self):
		self.SetUp()
		self.RRC_X_test(self.cpu.i_RRC_A, 'a')
		
	def test_i_STOP(self):
		#TODO: test properly
		pass
		
	def test_i_LD_DE_nn(self):
		self.SetUp()
		self.LD_XX_nn_test(self.cpu.i_LD_DE_nn, 'de')
		
	def test_i_LD_DE_A_mem(self):
		self.SetUp()
		self.LD_XX_X_mem_test(self.cpu.i_LD__DE__A, 'de', 'a')
	
	def test_i_INC_DE(self):
		self.SetUp()
		self.IncTest(self.cpu.i_INC_DE, 'de')
		
	def test_i_INC_D(self):
		self.SetUp()
		self.IncTest(self.cpu.i_INC_D, 'd')
		
	def test_i_DEC_D(self):
		self.SetUp()
		self.DecTest(self.cpu.i_DEC_D, 'd')
		
	def test_i_LD_D_n(self):
		self.SetUp()
		self.LD_X_n_test(self.cpu.i_LD_D_n, 'd')
		
	def test_i_RL_A(self):
		self.SetUp()
		self.RL_X_test(self.cpu.i_RL_A, 'a')
		
	def test_i_JR_n(self):
		self.SetUp()
		immediate = 0b01001001
		originalPC = self.cpu['pc']
		self.cpu[self.cpu['pc']+1] = immediate
		self.cpu.i_JR_n(self.cpu)
		self.assertEqual(self.cpu['pc'], originalPC + immediate + 2)
		self.SetUp()
		immediate = 0b11001001
		originalPC = 500
		self.cpu['pc'] = originalPC
		#print "original PC: %s, immediate: %s, two's complement immediate: %s" % (originalPC, immediate, (~immediate + 1))
		self.cpu[self.cpu['pc']+1] = immediate
		self.cpu.i_JR_n(self.cpu)
		self.assertEqual(self.cpu['pc'], originalPC - ((~immediate + 1) & 255) + 2)
	
	def test_i_ADD_HL_DE(self):
		self.SetUp()
		self.ADD_XX_XX_test(self.cpu.i_ADD_HL_DE, 'hl', 'de')
		
	def test_i_LD_A_DE_mem(self):
		self.SetUp()
		self.LD_X_XX_mem_test(self.cpu.i_LD_A__DE_, 'a', 'de')
	
	def test_i_DEC_DE(self):
		self.SetUp()
		self.DecTest(self.cpu.i_DEC_DE, 'de')
		
	def test_i_INC_E(self):
		self.SetUp()
		self.IncTest(self.cpu.i_INC_E, 'e')
	
	def test_i_DEC_E(self):
		self.SetUp()
		self.DecTest(self.cpu.i_DEC_E, 'e')
		
	def test_i_LD_E_n(self):
		self.SetUp()
		self.LD_X_n_test(self.cpu.i_LD_E_n, 'e')
		
	def test_i_RR_A(self):
		self.SetUp()
		self.RR_X_test(self.cpu.i_RR_A, 'a')
		
	def test_i_JR_NZ_n(self):
		self.SetUp()
		self.RelJumpTest(self.cpu.i_JR_NZ_n, 'Nzero')
		
	def test_i_LD_HL_nn(self):
		self.SetUp()
		self.LD_XX_nn_test(self.cpu.i_LD_HL_nn, 'hl')
		
	def test_i_LDI_HL_A_mem(self):
		self.SetUp()
		self.LDI_XX_X_mem_test(self.cpu.i_LDI__HL__A, 'hl', 'a')
		
	def test_i_INC_HL(self):
		self.SetUp()
		self.IncTest(self.cpu.i_INC_HL, 'hl')
		
	def test_i_INC_H(self):
		self.SetUp()
		self.IncTest(self.cpu.i_INC_H, 'h')
		
	def test_i_DEC_H(self):
		self.SetUp()
		self.DecTest(self.cpu.i_DEC_H, 'h')
		
	def test_i_LD_H_n(self):
		self.SetUp()
		self.LD_X_n_test(self.cpu.i_LD_H_n, 'h')
		
	def test_i_DAA(self):
		self.SetUp()
		immediate = 0b00111100
		self.cpu['a'] = immediate
		self.cpu.i_DAA(self.cpu)
		self.assertEqual(self.cpu['a'], 0b01000010)
		immediate = 0b00110001
		self.cpu['a'] = immediate
		self.cpu.i_DAA(self.cpu)
		self.assertEqual(self.cpu['a'], immediate)
		
	def test_i_JR_Z_n(self):
		self.SetUp()
		self.RelJumpTest(self.cpu.i_JR_Z_n, "zero")
				
	def test_i_ADD_HL_HL(self):
		self.SetUp()
		self.ADD_XX_XX_test(self.cpu.i_ADD_HL_HL, 'hl', 'hl')
		
	def test_i_LDI_A_HL_mem(self):
		self.SetUp()
		self.LDI_X_XX_mem_test(self.cpu.i_LDI_A__HL_, 'a', 'hl')
	
	def test_i_DEC_HL(self):
		self.SetUp()
		self.DecTest(self.cpu.i_DEC_HL, 'hl')
		
	def test_i_INC_L(self):
		self.SetUp()
		self.IncTest(self.cpu.i_INC_L, 'l')
		
	def test_i_DEC_L(self):
		self.SetUp()
		self.DecTest(self.cpu.i_DEC_L, 'l')
		
	def test_i_LD_L_n(self):
		self.SetUp()
		self.LD_X_n_test(self.cpu.i_LD_L_n, 'l')
		
	def test_i_CPL(self):
		self.SetUp()
		immediate = 0b10111011
		self.cpu['a'] = immediate
		self.cpu.i_CPL(self.cpu)
		self.assertEqual(self.cpu['a'], 0b01000100)
		
	def test_i_JR_NC_n(self):
		self.SetUp()
		self.RelJumpTest(self.cpu.i_JR_NC_n, "Ncarry")
		
	def test_i_LD_SP_nn(self):
		self.SetUp()
		self.LD_XX_nn_test(self.cpu.i_LD_SP_nn, 'sp')
		
	def test_i_LDD_HL_A(self):
		self.SetUp()
		self.LDD_XX_X_test(self.cpu.i_LDD__HL__A, 'hl', 'a')
		
	def test_i_INC_SP(self):
		self.SetUp()
		self.IncTest(self.cpu.i_INC_SP, 'sp')
		
	def test_i_INC_HL_mem(self):
		self.SetUp()
		self.INC_XX_mem_test(self.cpu.i_INC__HL_, 'hl')
		
	def test_i_DEC_HL_mem(self):
		self.SetUp()
		self.DEC_XX_mem_test(self.cpu.i_DEC__HL_, 'hl')
		
	def test_i_LD_HL_n_mem(self):
		self.SetUp()
		self.LD_XX_n_mem_test(self.cpu.i_LD__HL__n, 'hl')
		
	def test_i_SCF(self):
		self.SetUp()
		self.cpu.setFlags(['zero', 'sub', 'halfcarry'])
		oldFlags = self.cpu['f']
		self.cpu.i_SCF(self.cpu)
		self.flags_test({'carry':1, 'sub':0, 'halfcarry':0, 'zero':1}, oldFlags)
		self.cpu.resetFlags(['zero'])
		self.cpu.setFlags(['sub', 'halfcarry'])
		oldFlags = self.cpu['f']
		self.cpu.i_SCF(self.cpu)
		self.flags_test({'carry':1, 'sub':0, 'halfcarry':0, 'zero':0}, oldFlags)
				
		
	def test_i_JR_C_n(self):
		self.SetUp()
		self.RelJumpTest(self.cpu.i_JR_C_n, "carry")
		
	def test_i_ADD_HL_SP(self):
		self.SetUp()
		self.ADD_XX_XX_test(self.cpu.i_ADD_HL_SP, 'hl', 'sp')
		
	def test_i_LDD_A_HL(self):
		self.SetUp()
		self.LDD_X_XX_test(self.cpu.i_LDD_A__HL_, 'a', 'hl')
		
	def test_i_DEC_SP(self):
		self.SetUp()
		self.DecTest(self.cpu.i_DEC_SP, 'sp')
		
	def test_i_INC_A(self):
		self.SetUp()
		self.IncTest(self.cpu.i_INC_A, 'a')
	
	def test_i_DEC_A(self):
		self.SetUp()
		self.DecTest(self.cpu.i_DEC_A, 'a')
		
	def test_i_LD_A_n(self):
		self.SetUp()
		self.LD_X_n_test(self.cpu.i_LD_A_n, 'a')
		
	def test_i_CCF(self):
		self.SetUp()
		self.cpu.setFlags(['sub', 'halfcarry', 'zero'])
		oldFlags = self.cpu['f']
		self.cpu.i_CCF(self.cpu)
		self.flags_test({'carry':1, 'sub':0, 'halfcarry':0, 'zero':1}, oldFlags)
		self.cpu.resetFlags(['zero'])
		self.cpu.setFlags(['sub', 'halfcarry'])
		oldFlags = self.cpu['f']
		self.cpu.i_CCF(self.cpu)
		self.flags_test({'carry':0, 'sub':0, 'halfcarry':0, 'zero':0}, oldFlags)
		
	def test_i_LD_B_B(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_B_B, 'b', 'b')
		
	def test_i_LD_B_C(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_B_C, 'b', 'c')
		
	def test_i_LD_B_D(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_B_D, 'b', 'd')
		
	def test_i_LD_B_E(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_B_E, 'b', 'e')
		
	def test_i_LD_B_H(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_B_H, 'b', 'h')
		
	def test_i_LD_B_L(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_B_L, 'b', 'l')

	def test_i_LD_B_HL_mem(self):
		self.SetUp()
		self.LD_X_XX_mem_test(self.cpu.i_LD_B__HL_, 'b', 'hl')
		
	def test_i_LD_B_A(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_B_A, 'b', 'a')
		
	def test_i_LD_C_B(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_C_B, 'c', 'b')
		
	def test_i_LD_C_C(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_C_C, 'c', 'c')
		
	def test_i_LD_C_D(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_C_D, 'c', 'd')
		
	def test_i_LD_C_E(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_C_E, 'c', 'e')
		
	def test_i_LD_C_H(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_C_H, 'c', 'h')
		
	def test_i_LD_C_L(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_C_L, 'c', 'l')
		
	def test_i_LD_C_HL_mem(self):
		self.SetUp()
		self.LD_X_XX_mem_test(self.cpu.i_LD_C__HL_, 'c', 'hl')

	def test_i_LD_C_A(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_C_A, 'c', 'a')
		
	def test_i_LD_D_B(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_D_B, 'd', 'b')
		
	def test_i_LD_D_C(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_D_C, 'd', 'c')
		
	def test_i_LD_D_D(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_D_D, 'd', 'd')
		
	def test_i_LD_D_E(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_D_E, 'd', 'e')
		
	def test_i_LD_D_H(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_D_H, 'd', 'h')
		
	def test_i_LD_D_L(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_D_L, 'd', 'l')
		
	def test_i_LD_D_HL_mem(self):
		self.SetUp()
		self.LD_X_XX_mem_test(self.cpu.i_LD_D__HL_, 'd', 'hl')

	def test_i_LD_D_A(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_D_A, 'd', 'a')
	
	def test_i_LD_E_B(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_E_B, 'e', 'b')
		
	def test_i_LD_E_C(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_E_C, 'e', 'c')
		
	def test_i_LD_E_D(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_E_D, 'e', 'd')
		
	def test_i_LD_E_E(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_E_E, 'e', 'e')
		
	def test_i_LD_E_H(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_E_H, 'e', 'h')
		
	def test_i_LD_E_L(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_E_L, 'e', 'l')
		
	def test_i_LD_E_HL_mem(self):
		self.SetUp()
		self.LD_X_XX_mem_test(self.cpu.i_LD_E__HL_, 'e', 'hl')

	def test_i_LD_E_A(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_E_A, 'e', 'a')
		
	def test_i_LD_H_B(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_H_B, 'h', 'b')
		
	def test_i_LD_H_C(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_H_C, 'h', 'c')
		
	def test_i_LD_H_D(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_H_D, 'h', 'd')
		
	def test_i_LD_H_E(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_H_E, 'h', 'e')
		
	def test_i_LD_H_H(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_H_H, 'h', 'h')
		
	def test_i_LD_H_L(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_H_L, 'h', 'l')
		
	def test_i_LD_H_HL_mem(self):
		self.SetUp()
		self.LD_X_XX_mem_test(self.cpu.i_LD_H__HL_, 'h', 'hl')

	def test_i_LD_H_A(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_H_A, 'h', 'a')	
		
	def test_i_LD_L_B(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_L_B, 'l', 'b')
		
	def test_i_LD_L_C(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_L_C, 'l', 'c')
		
	def test_i_LD_L_D(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_L_D, 'l', 'd')
		
	def test_i_LD_L_E(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_L_E, 'l', 'e')
		
	def test_i_LD_L_H(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_L_H, 'l', 'h')
		
	def test_i_LD_L_L(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_L_L, 'l', 'l')
		
	def test_i_LD_L_HL_mem(self):
		self.SetUp()
		self.LD_X_XX_mem_test(self.cpu.i_LD_L__HL_, 'l', 'hl')

	def test_i_LD_L_A(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_L_A, 'l', 'a')
		
	def test_i_LD_HL_B(self):
		self.SetUp()
		self.LD_XX_X_test(self.cpu.i_LD__HL__B, 'hl', 'b')
	
	def test_i_LD_HL_C(self):
		self.SetUp()
		self.LD_XX_X_test(self.cpu.i_LD__HL__C, 'hl', 'c')
		
	def test_i_LD_HL_D(self):
		self.SetUp()
		self.LD_XX_X_test(self.cpu.i_LD__HL__D, 'hl', 'd')
		
	def test_i_LD_HL_E(self):
		self.SetUp()
		self.LD_XX_X_test(self.cpu.i_LD__HL__E, 'hl', 'e')
		
	def test_i_LD_HL_H(self):
		self.SetUp()
		self.LD_XX_X_test(self.cpu.i_LD__HL__H, 'hl', 'h')
		
	def test_i_LD_HL_L(self):
		self.SetUp()
		self.LD_XX_X_test(self.cpu.i_LD__HL__L, 'hl', 'l')
		
	def test_i_HALT(self):
		pass
		#TODO: test properly
		
	def test_i_LD_HL_A(self):
		self.SetUp()
		self.LD_XX_X_test(self.cpu.i_LD__HL__A, 'hl', 'a')

	def test_i_LD_A_B(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_A_B, 'a', 'b')
		
	def test_i_LD_A_C(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_A_C, 'a', 'c')
		
	def test_i_LD_A_D(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_A_D, 'a', 'd')
		
	def test_i_LD_A_E(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_A_E, 'a', 'e')
		
	def test_i_LD_A_H(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_A_H, 'a', 'h')
		
	def test_i_LD_A_L(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_A_L, 'a', 'l')
		
	def test_i_LD_A_HL_mem(self):
		self.SetUp()
		self.LD_X_XX_mem_test(self.cpu.i_LD_A__HL_, 'a', 'hl')

	def test_i_LD_A_A(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_A_A, 'a', 'a')

	def test_i_ADD_B(self):
		self.SetUp()
		self.ADD_X_X_test(self.cpu.i_ADD_A_B, 'a', 'b')

	def test_i_ADD_C(self):
		self.SetUp()
		self.ADD_X_X_test(self.cpu.i_ADD_A_C, 'a', 'c')		
		
	def test_i_ADD_D(self):
		self.SetUp()
		self.ADD_X_X_test(self.cpu.i_ADD_A_D, 'a', 'd')
		
	def test_i_ADD_E(self):
		self.SetUp()
		self.ADD_X_X_test(self.cpu.i_ADD_A_E, 'a', 'e')
		
	def test_i_ADD_H(self):
		self.SetUp()
		self.ADD_X_X_test(self.cpu.i_ADD_A_H, 'a', 'h')
		
	def test_i_ADD_L(self):
		self.SetUp()
		self.ADD_X_X_test(self.cpu.i_ADD_A_L, 'a', 'l')
		
	def test_i_ADD_HL_mem(self):
		self.SetUp()
		self.ADD_X_XX_mem_test(self.cpu.i_ADD_A__HL_, 'a', 'hl')
		
	def test_i_ADD_A(self):
		self.SetUp()
		self.ADD_X_X_test(self.cpu.i_ADD_A_A, 'a', 'a')
		
	def test_i_ADC_B(self):
		self.SetUp()
		self.ADC_X_X_test(self.cpu.i_ADC_A_B, 'a', 'b')
		
	def test_i_ADC_C(self):
		self.SetUp()
		self.ADC_X_X_test(self.cpu.i_ADC_A_C, 'a', 'c')		
		
	def test_i_ADC_D(self):
		self.SetUp()
		self.ADC_X_X_test(self.cpu.i_ADC_A_D, 'a', 'd')	
		
	def test_i_ADC_E(self):
		self.SetUp()
		self.ADC_X_X_test(self.cpu.i_ADC_A_E, 'a', 'e')
		
	def test_i_ADC_H(self):
		self.SetUp()
		self.ADC_X_X_test(self.cpu.i_ADC_A_H, 'a', 'h')

	def test_i_ADC_L(self):
		self.SetUp()
		self.ADC_X_X_test(self.cpu.i_ADC_A_L, 'a', 'l')
	
	def test_i_ADC_HL_mem(self):
		self.SetUp()
		self.ADC_X_XX_mem_test(self.cpu.i_ADC_A__HL_, 'a', 'hl')

	def test_i_ADC_A(self):
		self.SetUp()
		self.ADC_X_X_test(self.cpu.i_ADC_A_A, 'a', 'a')
		
	def test_i_SUB_A_B(self):
		self.SetUp()
		self.SUB_X_X_test(self.cpu.i_SUB_A_B, 'a', 'b')
		
	def test_i_SUB_A_C(self):
		self.SetUp()
		self.SUB_X_X_test(self.cpu.i_SUB_A_C, 'a', 'c')
		
	def test_i_SUB_A_D(self):
		self.SetUp()
		self.SUB_X_X_test(self.cpu.i_SUB_A_D, 'a', 'd')
		
	def test_i_SUB_A_E(self):
		self.SetUp()
		self.SUB_X_X_test(self.cpu.i_SUB_A_E, 'a', 'e')

	def test_i_SUB_A_H(self):
		self.SetUp()
		self.SUB_X_X_test(self.cpu.i_SUB_A_H, 'a', 'h')
		
	def test_i_SUB_A_L(self):
		self.SetUp()
		self.SUB_X_X_test(self.cpu.i_SUB_A_L, 'a', 'l')
		
	def test_i_SUB_HL_mem(self):
		self.SetUp()
		self.SUB_X_XX_mem_test(self.cpu.i_SUB_A__HL_, 'a', 'hl')

	def test_i_SUB_A_A(self):
		self.SetUp()
		self.SUB_X_X_test(self.cpu.i_SUB_A_A, 'a', 'a')
		
	def test_i_SBC_B(self):
		self.SetUp()
		self.SBC_X_X_test(self.cpu.i_SBC_A_B, 'a', 'b')
		
	def test_i_SBC_C(self):
		self.SetUp()
		self.SBC_X_X_test(self.cpu.i_SBC_A_C, 'a', 'c')
		
	def test_i_SBC_D(self):
		self.SetUp()
		self.SBC_X_X_test(self.cpu.i_SBC_A_D, 'a', 'd')
		
	def test_i_SBC_E(self):
		self.SetUp()
		self.SBC_X_X_test(self.cpu.i_SBC_A_E, 'a', 'e')
		
	def test_i_SBC_H(self):
		self.SetUp()
		self.SBC_X_X_test(self.cpu.i_SBC_A_H, 'a', 'h')
		
	def test_i_SBC_L(self):
		self.SetUp()
		self.SBC_X_X_test(self.cpu.i_SBC_A_L, 'a', 'l')
		
	def test_i_SBC_HL_mem(self):
		self.SetUp()
		self.SBC_X_XX_mem_test(self.cpu.i_SBC_A__HL_, 'a', 'hl')
	
	def test_i_SBC_A(self):
		self.SetUp()
		self.SBC_X_X_test(self.cpu.i_SBC_A_A, 'a', 'a')	
	
	def test_i_AND_B(self):
		self.SetUp()
		self.AND_X_X_test(self.cpu.i_AND_B, 'a', 'b')	

	def test_i_AND_C(self):
		self.SetUp()
		self.AND_X_X_test(self.cpu.i_AND_C, 'a', 'c')
		
	def test_i_AND_D(self):
		self.SetUp()
		self.AND_X_X_test(self.cpu.i_AND_D, 'a', 'd')	
		
	def test_i_AND_E(self):
		self.SetUp()
		self.AND_X_X_test(self.cpu.i_AND_E, 'a', 'e')	

	def test_i_AND_H(self):
		self.SetUp()
		self.AND_X_X_test(self.cpu.i_AND_H, 'a', 'h')	
		
	def test_i_AND_L(self):
		self.SetUp()
		self.AND_X_X_test(self.cpu.i_AND_L, 'a', 'l')
		
	def test_i_AND_HL_mem(self):
		self.SetUp()
		self.AND_X_XX_mem_test(self.cpu.i_AND__HL_, 'a', 'hl')

	def test_i_AND_A(self):
		self.SetUp()
		self.AND_X_X_test(self.cpu.i_AND_A, 'a', 'a')
		
	def test_i_XOR_B(self):
		self.SetUp()
		self.XOR_X_X_test(self.cpu.i_XOR_B, 'a', 'b')
		
	def test_i_XOR_C(self):
		self.SetUp()
		self.XOR_X_X_test(self.cpu.i_XOR_C, 'a', 'c')
		
	def test_i_XOR_D(self):
		self.SetUp()
		self.XOR_X_X_test(self.cpu.i_XOR_D, 'a', 'd')
		
	def test_i_XOR_E(self):
		self.SetUp()
		self.XOR_X_X_test(self.cpu.i_XOR_E, 'a', 'e')
		
	def test_i_XOR_H(self):
		self.SetUp()
		self.XOR_X_X_test(self.cpu.i_XOR_H, 'a', 'h')
		
	def test_i_XOR_L(self):
		self.SetUp()
		self.XOR_X_X_test(self.cpu.i_XOR_L, 'a', 'l')
		
	def test_i_XOR_HL_mem(self):
		self.SetUp()
		self.XOR_X_XX_mem_test(self.cpu.i_XOR__HL_, 'a', 'hl')
		
	def test_i_XOR_A(self):
		self.SetUp()
		self.XOR_X_X_test(self.cpu.i_XOR_A, 'a', 'a')
		
	def test_i_OR_B(self):
		self.SetUp()
		self.OR_X_X_test(self.cpu.i_OR_B, 'a', 'b')
		
	def test_i_OR_C(self):
		self.SetUp()
		self.OR_X_X_test(self.cpu.i_OR_C, 'a', 'c')
		
	def test_i_OR_D(self):
		self.SetUp()
		self.OR_X_X_test(self.cpu.i_OR_D, 'a', 'd')
		
	def test_i_OR_E(self):
		self.SetUp()
		self.OR_X_X_test(self.cpu.i_OR_E, 'a', 'e')
		
	def test_i_OR_H(self):
		self.SetUp()
		self.OR_X_X_test(self.cpu.i_OR_H, 'a', 'h')

	def test_i_OR_L(self):
		self.SetUp()
		self.OR_X_X_test(self.cpu.i_OR_L, 'a', 'l')
		
	def test_i_OR_HL_mem(self):
		self.SetUp()
		self.OR_X_XX_mem_test(self.cpu.i_OR__HL_, 'a', 'hl')
		
	def test_i_OR_A(self):
		self.SetUp()
		self.OR_X_X_test(self.cpu.i_OR_A, 'a', 'a')	
		
	def test_i_CP_B(self):
		self.SetUp()
		self.CP_X_X_test(self.cpu.i_CP_B, 'a', 'b')

	def test_i_CP_C(self):
		self.SetUp()
		self.CP_X_X_test(self.cpu.i_CP_C, 'a', 'c')
		
	def test_i_CP_D(self):
		self.SetUp()
		self.CP_X_X_test(self.cpu.i_CP_D, 'a', 'd')
		
	def test_i_CP_E(self):
		self.SetUp()
		self.CP_X_X_test(self.cpu.i_CP_E, 'a', 'e')
		
	def test_i_CP_H(self):
		self.SetUp()
		self.CP_X_X_test(self.cpu.i_CP_H, 'a', 'h')
		
	def test_i_CP_L(self):
		self.SetUp()
		self.CP_X_X_test(self.cpu.i_CP_L, 'a', 'l')
		
	def test_i_CP_HL_mem(self):
		self.SetUp()
		self.CP_X_XX_mem_test(self.cpu.i_CP__HL_, 'a', 'hl')
		
	def test_i_CP_A(self):
		self.SetUp()
		self.CP_X_X_test(self.cpu.i_CP_A, 'a', 'a')
		
	def test_i_RET_NZ(self):
		self.SetUp()
		self.RET_test(self.cpu.i_RET_NZ, 'Nzero')
		
	def test_i_POP_BC(self):
		self.SetUp()
		self.POP_XX_test(self.cpu.i_POP_BC, 'bc')
		
	def test_i_JP_NZ_nn(self):
		self.SetUp()
		self.RelJumpTest(self.cpu.i_JP_NZ_nn, "Nzero", twobytes=1)
		
	def test_i_JP_nn(self):
		self.SetUp()
		self.RelJumpTest(self.cpu.i_JP_nn, ".", twobytes=1)
		
	def test_i_CALL_NZ_nn(self):
		self.SetUp()
		self.CALL_nn_test(self.cpu.i_CALL_NZ_nn, 'Nzero')
	
	def test_i_PUSH_BC(self):
		self.SetUp()
		self.PUSH_XX_test(self.cpu.i_PUSH_BC, 'bc')
		
	def test_i_ADD_A_n(self):
		self.SetUp()
		self.ADD_X_n_test(self.cpu.i_ADD_A_n, 'a')
		
	def test_i_RST_0(self):
		self.SetUp()
		self.RST_X_test(self.cpu.i_RST_0, '0')
		
	def test_i_RET_Z(self):
		self.SetUp()
		self.RET_test(self.cpu.i_RET_Z, 'zero')
		
	def test_i_RET(self):
		self.SetUp()
		self.RET_test(self.cpu.i_RET, '.')
		
	def test_i_JP_Z_nn(self):
		self.SetUp()
		self.RelJumpTest(self.cpu.i_JP_Z_nn, "zero", twobytes=1)
		
	#def test_i_Ext_ops(self):
	#	self.SetUp()
	#	self.assertEqual(self.cpu.extraOpcodeMode, 0)
	#	self.cpu.i_Ext_ops(self.cpu)
	#	self.assertEqual(self.cpu.extraOpcodeMode, 1)
		
	def test_i_CALL_Z_nn(self):
		self.SetUp()
		self.CALL_nn_test(self.cpu.i_CALL_Z_nn, 'zero')
		
	def test_i_CALL_nn(self):
		self.SetUp()
		self.CALL_nn_test(self.cpu.i_CALL_nn, ".")
		
	def test_i_ADC_A_n(self):
		self.SetUp()
		self.ADC_X_n_test(self.cpu.i_ADC_A_n, 'a')
		
	def test_i_RST_8(self):
		self.SetUp()
		self.RST_X_test(self.cpu.i_RST_8, '8')
		
	def test_i_RET_NC(self):
		self.SetUp()
		self.RET_test(self.cpu.i_RET_NC, "Ncarry")
		
	def test_i_POP_DE(self):
		self.SetUp()
		self.POP_XX_test(self.cpu.i_POP_DE, 'de')
		
	def test_i_JP_NC_nn(self):
		self.SetUp()
		self.RelJumpTest(self.cpu.i_JP_NC_nn, "Ncarry", twobytes=1)
	
	def test_i_XXd3(self):
		pass
		
	def test_i_CALL_NC_nn(self):
		self.SetUp()
		self.CALL_nn_test(self.cpu.i_CALL_NC_nn, "Ncarry")
		
	def test_i_PUSH_DE(self):
		self.SetUp()
		self.PUSH_XX_test(self.cpu.i_PUSH_DE, 'de')
		
	def test_i_SUB_A_n(self):
		self.SetUp()
		self.SUB_X_n_test(self.cpu.i_SUB_A_n, 'a')
		
	def test_i_RST_10(self):
		self.SetUp()
		self.RST_X_test(self.cpu.i_RST_10, '16')
		
	def test_i_RET_C(self):
		self.SetUp()
		self.RET_test(self.cpu.i_RET_C, "carry")
		
	def test_i_RETI(self):
		self.SetUp()
		self.RET_test(self.cpu.i_RETI, ".")
		self.assertEqual(self.cpu.cpu.ime, 1)
		
	def test_i_JP_C_nn(self):
		self.SetUp()
		self.RelJumpTest(self.cpu.i_JP_C_nn, 'carry', twobytes=1)
	
	def test_i_XXdb(self):
		pass
	
	def test_i_CALL_C_nn(self):
		self.SetUp()
		self.CALL_nn_test(self.cpu.i_CALL_C_nn, 'carry')
		
	def test_i_XXdd(self):
		pass
		
	def test_i_SBC_A_n(self):
		self.SetUp()
		self.SBC_X_n(self.cpu.i_SBC_A_n, 'a')
		
	def test_i_RST_18(self):
		self.SetUp()
		self.RST_X_test(self.cpu.i_RST_18, str(int(0x18)))
		
	def test_i_LDH_n_A(self):
		self.SetUp()
		immediate = 0x41
		immediate2 = 0x40
		self.cpu['a'] = immediate2
		self.cpu[self.cpu['pc']+1] = immediate
		self.cpu.i_LDH__n__A(self.cpu)
		self.assertEqual(self.cpu.mmu.rb(0xff00 + immediate), immediate2)
		
	def test_i_POP_HL(self):
		self.SetUp()
		self.POP_XX_test(self.cpu.i_POP_HL, 'hl')
		
	def test_i_LDH_C_A(self):
		self.SetUp()
		immediate = 0x41
		immediate2 = 0xab
		self.cpu['a'] = immediate2
		self.cpu['c'] = immediate
		self.cpu.i_LDH__C__A(self.cpu)
		self.assertEqual(self.cpu.mmu.rb(0xff00 + immediate), immediate2)
		
	def test_i_XXe3(self):
		pass
		
	def test_i_XXe4(self):
		pass
		
	def test_i_PUSH_HL(self):
		self.SetUp()
		self.PUSH_XX_test(self.cpu.i_PUSH_HL, 'hl')
		
	def test_i_AND_n(self):
		self.SetUp()
		immediate = 0x13
		immediate2 = 0xab
		self.cpu['a'] = immediate2
		self.cpu[self.cpu['pc']+1] = immediate
		self.cpu.i_AND_n(self.cpu)
		self.assertEqual(self.cpu['a'], immediate2 & immediate)
		
	def test_i_RST_20(self):
		self.SetUp()
		self.RST_X_test(self.cpu.i_RST_20, str(int(0x20)))
		
	def test_i_ADD_SP_d(self):
		self.SetUp()
		self.ADD_X_n_test(self.cpu.i_ADD_SP_d, 'sp', signed=1)
		
	def test_i_JP__HL_(self):
		self.SetUp()
		address = 0x00be
		immediate = 0x1337
		self.cpu['hl'] = address
		self.cpu.mmu.ww(address, immediate)
		self.cpu.i_JP__HL_(self.cpu)
		self.assertEqual(self.cpu['pc'], immediate)
		
	def test_i_LD_nn_A(self):
		self.SetUp()
		address = 0xface
		immediate = 0x13
		self.cpu.mmu.ww(self.cpu['pc']+1, address)
		self.cpu['a'] = immediate
		self.cpu.i_LD__nn__A(self.cpu)
		self.assertEqual(self.cpu.mmu.rw(address), immediate)		
	
	def test_i_XXeb(self):
		pass
		
	def test_i_XXec(self):
		pass
		
	def test_i_XXed(self):
		pass
		
	def test_i_XOR_n(self):
		self.SetUp()
		immediate = 0x13
		immediate2 = 0xab
		self.cpu['a'] = immediate2
		self.cpu[self.cpu['pc']+1] = immediate
		self.cpu.i_XOR_n(self.cpu)
		self.assertEqual(self.cpu['a'], immediate2 ^ immediate)		
	
	def test_i_RST_28(self):
		self.SetUp()
		self.RST_X_test(self.cpu.i_RST_28, str(int(0x28)))
		
	def test_i_LDH_A_n(self):
		self.SetUp()
		immediate = 0x13
		immediate2 = 0x37
		address = 0xab
		self.cpu[self.cpu['pc']+1] = address
		self.cpu[address+0xff00] = immediate2
		self.cpu.i_LDH_A__n_(self.cpu)
		self.assertEqual(self.cpu['a'], immediate2)

	def test_i_POP_AF(self):
		self.SetUp()
		self.POP_XX_test(self.cpu.i_POP_AF, 'af')
		
	def test_i_XXf2(self):
		pass
		
	def test_i_DI(self):
		self.SetUp()
		self.cpu.cpu.ime = 1
		self.assertEquals(self.cpu.cpu.ime, 1)
		self.cpu.i_DI(self.cpu)
		self.assertEquals(self.cpu.cpu.ime, 0)
		
	def test_i_XXf4(self):
		pass
		
	def test_i_PUSH_AF(self):
		self.SetUp()
		self.PUSH_XX_test(self.cpu.i_PUSH_AF, 'af')
		
	def test_i_OR_n(self):
		self.SetUp()
		immediate = 0x13
		immediate2 = 0xab
		self.cpu['a'] = immediate2
		self.cpu[self.cpu['pc']+1] = immediate
		self.cpu.i_OR_n(self.cpu)
		self.assertEqual(self.cpu['a'], immediate2 | immediate)			
	
	def test_i_RST_30(self):
		self.SetUp()
		self.RST_X_test(self.cpu.i_RST_30, str(int(0x30)))
		
	def test_i_LDHL_SP_d(self):
		self.SetUp()
		currentSp = 0x1337
		immediate = 0x0a
		self.cpu['sp'] = currentSp
		self.cpu[self.cpu['pc']+1] = immediate
		self.cpu.i_LDHL_SP_d(self.cpu)
		self.assertEqual(self.cpu['hl'], immediate + currentSp)
		immediate = 0xba
		self.cpu['sp'] = currentSp
		self.cpu[self.cpu['pc']+1] = immediate
		self.cpu.i_LDHL_SP_d(self.cpu)
		self.assertEqual(self.cpu['hl'], currentSp - ((~immediate + 1) & 255))
		
	def test_i_LD_SP_HL(self):
		self.SetUp()
		self.LD_X_X_test(self.cpu.i_LD_SP_HL, 'sp', 'hl')
		
	def test_i_LD_A_nn(self):
		self.SetUp()
		self.LD_X_nn_test(self.cpu.i_LD_A__nn_, 'a')
		
	def test_i_EI(self):
		self.SetUp()
		self.cpu.cpu.ime = 0
		self.cpu.i_EI(self.cpu)
		self.assertEquals(self.cpu.cpu.ime, 1)
		
	def test_i_XXfc(self):
		pass
		
	def test_i_XXfd(self):
		pass
		
	def test_i_CP_n(self):
		self.SetUp()
		self.CP_n_test(self.cpu.i_CP_n)
		
	def test_i_RST_38(self):
		self.SetUp()
		self.RST_X_test(self.cpu.i_RST_38, str(int(0x38)))
		
	# secondary opcodes
	
	def test_i2_RLC_B(self):
		self.SetUp()
		self.RLC_X_test(self.cpu.i2_RLC_B, 'b')
		
	def test_i2_RLC_C(self):
		self.SetUp()
		self.RLC_X_test(self.cpu.i2_RLC_C, 'c')
		
	def test_i2_RLC_D(self):
		self.SetUp()
		self.RLC_X_test(self.cpu.i2_RLC_D, 'd')
		
	def test_i2_RLC_E(self):
		self.SetUp()
		self.RLC_X_test(self.cpu.i2_RLC_E, 'e')	
		
	def test_i2_RLC_H(self):
		self.SetUp()
		self.RLC_X_test(self.cpu.i2_RLC_H, 'h')
		
	def test_i2_RLC_L(self):
		self.SetUp()
		self.RLC_X_test(self.cpu.i2_RLC_L, 'l')
		
	def test_i2_RLC_HL_mem(self):
		self.SetUp()
		self.RLC_XX_mem_test(self.cpu.i2_RLC__HL_, 'hl')
	
	def test_i2_RLC_A(self):
		self.SetUp()
		self.RLC_X_test(self.cpu.i2_RLC_A, 'a')
		
	def test_i2_RRC_B(self):
		self.SetUp()
		self.RRC_X_test(self.cpu.i2_RRC_B, 'b')

	def test_i2_RRC_C(self):
		self.SetUp()
		self.RRC_X_test(self.cpu.i2_RRC_C, 'c')
		
	def test_i2_RRC_D(self):
		self.SetUp()
		self.RRC_X_test(self.cpu.i2_RRC_D, 'd')
		
	def test_i2_RRC_E(self):
		self.SetUp()
		self.RRC_X_test(self.cpu.i2_RRC_E, 'e')
		
	def test_i2_RRC_H(self):
		self.SetUp()
		self.RRC_X_test(self.cpu.i2_RRC_H, 'h')
		
	def test_i2_RRC_L(self):
		self.SetUp()
		self.RRC_X_test(self.cpu.i2_RRC_L, 'l')
		
	def test_i2_RRC_HL_mem(self):
		self.SetUp()
		self.RRC_XX_mem_test(self.cpu.i2_RRC__HL_, 'hl')

	def test_i2_RRC_A(self):
		self.SetUp()
		self.RRC_X_test(self.cpu.i2_RRC_A, 'a')
	
	def test_i2_RL_B(self):
		self.SetUp()
		self.RL_X_test(self.cpu.i2_RL_B, 'b')

	def test_i2_RL_C(self):
		self.SetUp()
		self.RL_X_test(self.cpu.i2_RL_C, 'c')
		
	def test_i2_RL_D(self):
		self.SetUp()
		self.RL_X_test(self.cpu.i2_RL_D, 'd')
		
	def test_i2_RL_E(self):
		self.SetUp()
		self.RL_X_test(self.cpu.i2_RL_E, 'e')
		
	def test_i2_RL_H(self):
		self.SetUp()
		self.RL_X_test(self.cpu.i2_RL_H, 'h')
		
	def test_i2_RL_L(self):
		self.SetUp()
		self.RL_X_test(self.cpu.i2_RL_L, 'l')
		
	def test_i2_RL_HL_mem(self):
		self.SetUp()
		self.RL_XX_mem_test(self.cpu.i2_RL__HL_, 'hl')
		
	def test_i2_RL_A(self):
		self.SetUp()
		self.RL_X_test(self.cpu.i2_RL_A, 'a')
	
	def test_i2_RR_B(self):
		self.SetUp()
		self.RR_X_test(self.cpu.i2_RR_B, 'b')

	def test_i2_RR_C(self):
		self.SetUp()
		self.RR_X_test(self.cpu.i2_RR_C, 'c')
		
	def test_i2_RR_D(self):
		self.SetUp()
		self.RR_X_test(self.cpu.i2_RR_D, 'd')
		
	def test_i2_RR_E(self):
		self.SetUp()
		self.RR_X_test(self.cpu.i2_RR_E, 'e')
		
	def test_i2_RR_H(self):
		self.SetUp()
		self.RR_X_test(self.cpu.i2_RR_H, 'h')
		
	def test_i2_RR_L(self):
		self.SetUp()
		self.RR_X_test(self.cpu.i2_RR_L, 'l')
		
	def test_i2_RR_HL_mem(self):
		self.SetUp()
		self.RR_XX_mem_test(self.cpu.i2_RR__HL_, 'hl')
		
	def test_i2_RR_A(self):
		self.SetUp()
		self.RR_X_test(self.cpu.i2_RR_A, 'a')
	
	
if __name__=="__main__":
	print "%d/256 primary opcodes tested" % len(filter(lambda x: x[0:7] == "test_i_",dir(TestCPU)))
	print "%d/256 secondary opcodes tested" % len(filter(lambda x: x[0:7] == "test_i2",dir(TestCPU)))
	unittest.main()
