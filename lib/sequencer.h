// Copyright 2023 Zack Scholl.
//
// Author: Zack Scholl (zack.scholl@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// See http://creativecommons.org/licenses/MIT/ for more information.

#ifndef SEQUENCER_LIB
#define SEQUENCER_LIB 1

#define SEQUENCER_MAX_STEPS 255

typedef struct Sequencer {
  // stored data
  uint8_t rec_pos;
  uint8_t rec_len;
  uint8_t rec_key[SEQUENCER_MAX_STEPS];
  uint16_t rec_steps[SEQUENCER_MAX_STEPS];
  uint32_t rec_step_offset;

  // playing data
  uint8_t play_pos;
  uint32_t play_step_offset;
  bool play_finished;
} Sequencer;

void Sequencer_clear(Sequencer *seq) {
  seq->rec_pos = 0;
  seq->rec_len = 0;
  seq->rec_step_offset = 0;
  for (uint8_t i = 0; i < SEQUENCER_MAX_STEPS; i++) {
    seq->rec_key[i] = 0;
    seq->rec_steps[i] = 0;
  }
  seq->play_finished = true;
}

Sequencer *Sequencer_create() {
  Sequencer *seq = (Sequencer *)malloc(sizeof(Sequencer));
  Sequencer_clear(seq);
  return seq;
}

// Sequencer_add adds a key and step to the sequencer.
// The start of the sequence is always 0 and the last item of the sequence
// is not played.
void Sequencer_add(Sequencer *seq, uint8_t key, uint32_t step) {
  if (seq->rec_len < SEQUENCER_MAX_STEPS) {
    if (seq->rec_step_offset == 0) {
      seq->rec_step_offset = step;
    } else {
      --step;  // fix off by one error
    }
    seq->rec_key[seq->rec_len] = key;
    seq->rec_steps[seq->rec_len] = step - seq->rec_step_offset;
    ++seq->rec_len;
  }
}

int8_t Sequencer_emit(Sequencer *seq, uint32_t step) {
  if (seq->rec_len == 0) {
    return -1;
  }
  if (seq->play_finished) {
    seq->play_finished = false;
    seq->play_pos = 0;
    seq->play_step_offset = 0;
  }
  if (seq->play_step_offset == 0) {
    seq->play_step_offset = step;
  }
  if (step - seq->play_step_offset == seq->rec_steps[seq->play_pos]) {
    int8_t key = seq->rec_key[seq->play_pos];
    ++seq->play_pos;
    if (seq->play_pos >= seq->rec_len) {
      seq->play_finished = true;
      return -1;
    }
    return key;
  }
  return -1;
}

bool Sequencer_is_finished(Sequencer *seq) { return seq->play_finished; }
void Sequencer_stop(Sequencer *seq) { seq->play_finished = true; }

#endif