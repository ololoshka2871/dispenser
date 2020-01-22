import machine

p4 = machine.Pin(4)
p0 = machine.Pin(0, machine.Pin.IN)

pwm4 = machine.PWM(p4)
pwm4.freq(1000)
pwm4.duty(0)


prevState = p0.value()
while True:
	v = p.value()
	if v != prevState:
		prevState = v
		if not v:
			pwm4.duty(256)
		else:
			pwm4.duty(0)
