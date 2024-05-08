# brainfuck

| Category | Available points (Bonus) | Best result |
|---|---|---|
| :rocket: Speedrun | 8 (3) | 11pts - 7.17 instructions/ns |

# Explanation
Many optimizations here
have been made based on assumptions
and by carefully inspecting test cases.

The idea here is to preprocess the given program
and to optimize it as much as possible before executing it.

Following optimizations have been made:
* Joining - multiple `+-><` after eachother have been joined
  into single parameters to avoid executing such operations
  multiple times
* Bytecode - different instructions have been merged into single
  operations to improve speed of reading
* Flags - things such as comparison or moving the cursor to the left
  or the right are marked in the bytecode using bit masks to avoid
  code overhead and reduce the amount of branching
* Loops preprocessing - matched brackets are precompiled to a jump table
  to avoid calculation each time you have to jump; mismatched brackets
  are taken care of in the same step
* LOOP bytecode - simple "move and multiply" loops of the form `[->++>+<<<]`
  have been merged into a single bytecode for simply multiplying out
  the values
* Output caching - instead of multiple `write` system calls, the output
  of the program gets saved until the end and is being printed out only once
