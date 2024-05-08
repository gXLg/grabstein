#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "brainfuck.h"

// JIT instructions
#define NOP        0b00000000
#define READ       0b01111111
#define WRITE      0b11111111

#define A          0b00001000
#define L          0b00001010
#define R          0b00001011
#define J          0b00001100

#define AL         0b00010010
#define ALJ        0b00010110
#define LJ         0b00100110

#define AR         0b00010011
#define ARJ        0b00010111
#define RJ         0b00100111

#define AJ         0b00100100

// flags
#define JM         0b00000100
#define G          0b00000010
#define IS_R       0b00000001

#define GFLAGS     0b00000011

#define LOOP       0b10101010


int brainfuck(struct BFState* state, const char* program) {

  // JIT compilation

  uint16_t jump_stack[256] = { 0 };
  uint16_t js_i = 0;

  unsigned char final_code[300] = { 0 };
  uint8_t args_add[260]; // +/-
  uint16_t args_go[260]; // >/<
  unsigned char* args_jmp[260]; // [/]
  uint8_t args_cmp[260]; // [/]
  uint16_t fc_i = 0;

  int16_t* args_loop2[260] = { 0 }; // optimize [mul]
  int16_t loop2_mem[8193] = { 0 };
  uint16_t l2mi = 0;

  uint8_t last_op = NOP;
  int16_t aa = 0;
  uint16_t ag = 0;
  uint16_t aj = 0;
  uint8_t ac = 0;

  unsigned char loop[8193] = { 0 };
  size_t li = 0;
  uint8_t lio = 0;
  int16_t offset = 0;
  int16_t leftest = 0;
  int16_t rightest = 0;

  char ch;
  for (const char* p = program; (ch = *p); p++) {
    switch (ch) {
      case '+':
        switch (last_op) {
          case NOP: last_op = A;
          case A: aa ++; break;
          default: goto push_op;
        }
        break;
      case '-':
        switch (last_op) {
          case NOP: last_op = A;
          case A: aa --; break;
          default: goto push_op;
        }
        break;
      case '>':
        switch (last_op) {
          case NOP: last_op = R;
          case R: ag ++; break;
          case A: last_op = AR;
          case AR: ag ++; break;
          default: goto push_op;
        }
        offset ++;
        if (offset > rightest) rightest = offset;
        break;
      case '<':
        switch (last_op) {
          case NOP: last_op = L;
          case L: ag ++; break;
          case A: last_op = AL;
          case AL: ag ++; break;
          default: goto push_op;
        }
        offset --;
        if (offset < leftest) leftest = offset;
        break;
      case '[':
        switch (last_op) {
          case NOP: last_op = J; break;
          case A: last_op = AJ; break;
          case L: last_op = LJ; break;
          case R: last_op = RJ; break;
          case AL: last_op = ALJ; break;
          case AR: last_op = ARJ; break;
          default: goto push_op;
        }
        jump_stack[js_i++] = fc_i;
        aj = 298;
        ac = 1;

        // init loop saving for later optimization
        li = 0;
        lio = 0;
        offset = 0;
        leftest = 0;
        rightest = 0;

        break;

      case ']':
        switch (last_op) {
          case NOP: last_op = J; break;
          case A: last_op = AJ; break;
          case L: last_op = LJ; break;
          case R: last_op = RJ; break;
          case AL: last_op = ALJ; break;
          case AR: last_op = ARJ; break;
          default: goto push_op;
        }
        if (js_i) {
          size_t match = jump_stack[--js_i];
          args_jmp[match] = final_code + fc_i;
          aj = match;

          // perform loop optimization
          // if no I/O operations were used inside of the loop
          // and the loop is balanced
          if (lio == 0 && offset == 0) {
            int16_t* adds = loop2_mem + l2mi;

            uint16_t ai = 0;
            int16_t lo = 9000;
            int16_t values = 0;
            for (uint16_t i = 0; i < li; i ++) {
              switch (loop[i]) {
                case '>': offset ++; break;
                case '<': offset --; break;
                case '+':
                  if (offset != lo) {
                    values ++;
                    lo = offset;
                    ai ++;
                    adds[ai ++] = offset;
                  }
                  adds[ai] ++;
                  break;
                case '-':
                  if (offset != lo) {
                    values ++;
                    lo = offset;
                    ai ++;
                    adds[ai ++] = offset;
                  }
                  adds[ai] --;
                  break;
              }
            }
            adds[0] = values;
            args_jmp[match] = final_code + match + 1;
            last_op = LOOP;

            args_loop2[match + 1] = adds;
            l2mi += ai + 1;

            fc_i = match + 1;
          }

        } else {
          aj = 298;
        }
        ac = 0;
        break;

      case '.':
        if (last_op != NOP) { goto push_op; }
        last_op = WRITE;
        loop[li++] = ch;

        lio = 1;
        break;

      case ',':
        if (last_op != NOP) { goto push_op; }
        last_op = READ;

        lio = 1;
        break;

      default: continue;

    }

  if (ch != '[' && ch != ']')
    loop[li++] = ch;

  continue;
  push_op:
    final_code[fc_i] = last_op;
    args_add[fc_i] = aa;
    args_go[fc_i] = ag;
    args_jmp[fc_i] = final_code + aj;
    args_cmp[fc_i] = ac;

    final_code[++ fc_i] = NOP;

    p --;
    last_op = NOP;
    aa = 0;
    ag = 0;
    aj = 0;
    ac = 0;

  }

  final_code[fc_i] = last_op;
  args_add[fc_i] = aa;
  args_go[fc_i] = ag;
  args_jmp[fc_i] = final_code + aj;
  args_cmp[fc_i] = ac;
  final_code[++ fc_i] = NOP;

  // printing out the bytecode for reference or debugging

//  for (int i = 0; final_code[i]; i++) {
//    printf("(%d) \t", i);
//    if (final_code[i] == LOOP) {
//      printf("o: %d lx: %hd rx: %hd\n", final_code[i], args_loop[i][0], args_loop[i][1]);
//    } else {
//      printf("o: %d a: %hd g: %hu j: %lu c: %hd\n", final_code[i], args_add[i], args_go[i], args_jmp[i] - final_code, args_cmp[i]);
//    }
//  }
//  return 1;



  // execution
  uint8_t* max = (state -> array) + (state -> array_len);
  uint8_t* min = (state -> array);
  register uint8_t* cursor = state -> cur;

  char output[1000] = { 0 };
  uint16_t oi = 0;

  register unsigned char* op = final_code;
  for (;; op++) {
    switch (*op) {
      case NOP: goto ex;
      case READ:
        *cursor = getchar();
        break;
      case WRITE:
        output[oi++] = *cursor;
        break;
      case LOOP:
        // assume minus == -1
        register int16_t *adds = args_loop2[op - final_code];

        int16_t values = *adds++;
        int16_t q = *cursor;
        for (int16_t off = 0; off < values; off ++) {
          int16_t offs = *adds++;
          int16_t aval = *adds++;
          if (cursor + offs >= max) {
            state -> cur = max + 1;
            fwrite(output, oi, 1, stdout);
            return -1;
          }
          if (cursor + offs < min) {
            state -> cur = min - 1;
            fwrite(output, oi, 1, stdout);
            return -1;
          }
          *(cursor + offs) += aval * q;
        }
        break;
      default:
        *cursor += args_add[op - final_code];
        register int16_t d = args_go[op - final_code];
        switch (*op & GFLAGS) {
          case G:
            if (cursor < min + d) {
              state -> cur = min - 1;
              fwrite(output, oi, 1, stdout);
              return -1;
            }
            cursor -= d;
            break;
          case G | IS_R:
            if (cursor >= max - d) {
              state -> cur = max + 1;
              fwrite(output, oi, 1, stdout);
              return -1;
            }
            cursor += d;
        }
        if ((*op & JM) && (!*cursor) == args_cmp[op - final_code]) op = args_jmp[op - final_code];
    }
  }

  ex:
  state -> cur = cursor;
  fwrite(output, oi, 1, stdout);
  return (op - final_code == 299) ? -1 : 0;
}
