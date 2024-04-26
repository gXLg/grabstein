# Collatz Obrit

| Category | Available points (Bonus) | Best result |
|---|---|---|
| :rocket: Speedrun | 5 (1)  | 6pts - 26.29ns |

# Explanation
In order to calculate Collatz numbers efficiently
I decided to create a lookup table.

The lookup table is empty in this repository to save space.
To generate the lookup table, change the `bits` variable
inside `generate_lookup.py` and run the script.
The table will be put in between `<lookup>` and `</lookup>`
inside `collatzorbit.c`.

The lookup table defines 4 parameters for different
bit masks for the specified `bits` amount.
These parameters are: `a`, `b`, `c` and `max`.

First I check if `n > max`, then return `0`.
Else I do `n = (n >> a) * b + c` which works
for every single bitmask.
Using this lookup table, I can efficiently
skip as many steps calculations as many bits were
specified in the table. My final submission
was using 20 bits so it could skip 20 steps at a time.
Best submission was using 22 bits.

Source: I found it by myself.

# Size Comparison
Of course, the downside of such a lookup
table, is that it tends to grow exponentially.

Here are some sizes I've tried out:
| Bits | Size | Time |
|--|--|--|
| 16 | 7kB | ~30s |
| 20 | 8MB | ~2min |
| 22 | 21MB | ~10min |

Generated on Cortex A78.
