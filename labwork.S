  # labwork.S
  # Written 2015-2017 by F Lundevall
  # Skeleton file for IS1200/IS1500 lab 1.
  # The macros PUSH and POP are in the public domain.
  # Please add your own code at the end of the file.

  #
  # Please keep the two macros PUSH and POP unchanged
  #
.macro	PUSH reg
	addi	$sp,$sp,-4
	sw	\reg,0($sp)
.endm

.macro	POP reg
	lw	\reg,0($sp)
	addi	$sp,$sp,4
.endm
  #
  # Please add your own code below this line
  #
  .global delay, time2string, enable_interrupt
  .text
# T: delay: delays for (hopefully) around $a0 ms
delay:	li	$t0,0		# T: counter for outer loop
	li	$t1,6663		# T: max value for inner loop counter. OBS!!! 6663 ~ 1 ms in chip
outlp:	addi	$t0,$t0,1		# T: increment outer counter
	li	$t2,0		# T: counter for inner loop
inlop:	addi	$t2,$t2,1		# T: increment inner counter
	slt 	$t3,$t1,$t2	# T: $t3 is 0 if max is higher than counter
	beqz	$t3,inlop	# T: loop to inner increment if inner counter is not equal to max
	nop
	bne	$t0,$a0,outlp	# T: loop to inner counter reset and outer counter increment unless outer counter is the same as argument
	nop
	jr	$ra		# T: return
	nop
# T: time2string supposed to set the time as ASCII character in this format "MM:SS\0" at the location saved in $a0
time2string:
	# T: save to stack
	PUSH	$s0
	PUSH	$s1
	PUSH	$s2
	PUSH	$s3
	PUSH	$s4
	PUSH	$s5
	PUSH	$s6
	PUSH	$s7
	PUSH	$ra		# T: since we jump in this method we need to keep the $ra saved
	PUSH	$a0
	PUSH	$a1
	li	$s0,16		# T: loop countdown, multiplied by 4 because it is use for bitshifting
	li	$s6,-8		# T: same, but reverse
	la	$s3,($a0)	# T: sets $s3 to address stored in $a0
	li	$s4,3		# T: used to check if it has looped 2 times
	li	$s7,4		# T: same but 3
	li	$s5,0		# T: stores temp version of data
	
t2sloop:
	addi	$s0,$s0,-4	# T: reduce loop countdown
	addi	$s6,$s6,8	# T: increment loop counter
	addi	$s4,$s4,-1
	addi	$s7,$s7,-1
	srlv	$a0,$a1,$s0	# T: makes sure the right bits are in the 4 lsb positions
	jal	hexasc		# T: calls hexasc on $a0, returns in $v0
	nop
	beqz	$s4,jump	# T: jump adds ':' and increases the address by 4
	nop
	
	beqz	$s7,jump2	# T: jumps in the last loop
	nop
	
	sllv	$v0,$v0,$s6	# T: format
	
	or	$s5,$s5,$v0	# T: set data into $s5
	
	bnez	$s0,t2sloop	# T: loops when not done
	nop
	
	sw	$s5,4($s3)	# T: adds last characters at next mem-address
	
	# T: Restore stack
	POP	$a1
	POP	$a0
	POP	$ra
	POP	$s7
	POP	$s6
	POP	$s5
	POP	$s4
	POP	$s3
	POP	$s2
	POP	$s1
	POP	$s0
	
	
	jr	$ra		# T: return
	nop
jump:	
	addi	$s4,$s4,-1
	li	$t0,0x3a0000	# T: 0x3a is ASCII for ':'
	or	$s5,$s5,$t0	# T: store it in location
	addi	$s6,$s6,8		# T: needed for correct bitshifting
	jr	$ra		# T: returns
	nop
	
jump2:
	sw	$s5,0($s3)	# T: stores it
	li	$s6,0		# T: correct formatting
	li	$s5,0		# T: empty $s5
	addi	$s7,$s7,-1
	jr	$ra
	nop


  # you can write your code for subroutine "hexasc" below this line
  #
hexasc:
	li	$v0,0xf		# sets $v0 = 0000 0000 0000 1111
	and	$v0,$v0,$a0	# bitwise and on $v0 and $a0 removes all bits over the 4 lsb and copies the 4 lsb
	addi	$v0,$v0,0x30	# '0' is 0x30 so this shifts 0-9 in input to the respective '0'-'9' char values
	slti	$t0,$v0,0x3a	# 0x39 is the value for '9' so if this sets $t1 to 0 then $t0 needs to be increased to the ASCII value for 'A'
	bne	$t0,$0,return	# can skip next step if the value is not over 0x39, return
	nop			# just in case
	addi	$v0,$v0,7	# add 7 to go from 0x3a to 0x41 which is the value for 'A'
return:
	jr	$ra		# return
	nop			# just in case

enable_interrupt:
	ei
	jr	$ra
	nop
