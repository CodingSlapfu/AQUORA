/* SPDX-License-Identifier: MIT; original KIRA-9 procedural portrait. */
#ifndef K9_FACE_H
#define K9_FACE_H
#include <stdint.h>
typedef struct {
  uint8_t mouth, blink;
  int8_t gaze, jaw, cheek, neck, brow;
  uint8_t lid, module;
  uint16_t open;
} k9_face;

/* Render any horizontal band; caller owns the pixels. No engine mutation or
 * heap. */
void k9_face_render(uint16_t *pixels, unsigned y0, unsigned height,
                    const k9_face *pose);
/* The FM-1 canvas stores RGB565 most significant byte first. */
void k9_face_render_canvas(uint16_t *pixels, unsigned y0, unsigned height,
                           const k9_face *pose);
#endif
