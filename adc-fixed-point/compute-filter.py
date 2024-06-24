import math

Fs = 2048 # Samples/Sec
f3db = 7 # Hz

half_Fs = Fs/2

omega3db = f3db * (math.pi/half_Fs)
alpha = math.cos(omega3db) - 1 + math.sqrt(math.cos(omega3db)**2 - 4*math.cos(omega3db) + 3)

print(f"alpha = {alpha} beta = {1 - alpha}")

alpha_fp = int(alpha * (1 << 18))
beta_fp = int((1 - alpha) * (1 << 18))

print(f"alpha_fp = {alpha_fp}, beta_fp = {beta_fp}")