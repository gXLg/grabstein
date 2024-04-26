from random import randrange as rng

bits = 20

# n:
# a, b, c
# a - bit shift of original 'n'
# b - multiplier
# c - number
# so the result is: b * (n >> a) + c

def mul(n):
  a, b, c = n
  a, b, c = (a, b * 3, c * 3 + 1)
  # calculate maximum
  m = ((2 ** 64 - 1) - c) // b * (1 << a)
  return ((a, b, c), m)


def div(n):
  a, b, c = n
  if c % 2 == 0:
    # b * (n>>a) is even, c is even
    c //= 2
    if b % 2 == 0:
      # b is even
      b //= 2
    else:
      # n>>a is even -> n>>a = 2*(n>>(a+1))
      a += 1

  else:
    # in this case b is always odd
    assert (b + c) % 2 == 0

    # n>>a = 2*(n>>(a+1))+1
    # b*(n>>a) = b*(2 * (n>>(a+1)) + 1) =
    # 2*b*(n>>(a+1)) + b
    # div by 2: b*(n>>(a+1)) + (b+c)/2
    a += 1
    c = (c + b) // 2

  return (a, b, c)


def mins(num):
  h = hex(num)
  s = str(num)
  return min(h, s, key=len)

func_list = []
func_index = 0
func_idxs = {}
func_lookup = []

max_list = []
max_index = 0
max_idxs = {}
max_lookup = []

for n in range(2 ** bits):
  func = (0, 1, 0)
  gmx = 2 ** 64 - 1
  number = n

  for _ in range(bits):
    if n % 2:
      n = n * 3 + 1
      func, mx = mul(func)
      if mx < gmx: gmx = mx
    else:
      n //= 2
      func = div(func)

  if func not in func_idxs:
    func_idxs[func] = func_index
    func_list.append("{" + ",".join(map(mins, func)) + "}")
    func_lookup.append(mins(func_index))
    func_index += 1
  else:
    func_lookup.append(mins(func_idxs[func]))

  if gmx not in max_idxs:
    max_idxs[gmx] = max_index
    max_list.append(mins(gmx))
    max_lookup.append(mins(max_index))
    max_index += 1
  else:
    max_lookup.append(mins(max_idxs[gmx]))

print(f"Total of {func_index + max_index} different setups for {bits} bits")

print("Writing...")

with open("collatzorbit.c", "r") as file:
  before, ins, after = file.read().partition("<lookup>\n")
  _, ins2, after = after.partition("// </lookup>")


with open("collatzorbit.c", "w") as file:
  file.write(before + ins)

  file.write(f"uint64_t bits = {bits};\nuint64_t mask = {hex((1 << bits) - 1)};\n")

  file.write("uint64_t max_i[] = {" + ",".join(max_lookup) + "};\n")
  file.write("uint64_t max[] = {" + ",".join(max_list) + "};\n")

  file.write("uint64_t func_i[] = {" + ",".join(func_lookup) + "};\n")
  file.write("uint64_t func[] = {" + ",".join(func_list) + "};\n")

  file.write(ins2 + after)

