# AQUORA Sans bitmap font

The firmware bitmap face is an offline derivative of Inter Tight, Copyright
2022 The Inter Project Authors, distributed under SIL OFL 1.1 in OFL.txt.
Upstream unmodified source: google/fonts `ofl/intertight/InterTight[wght].ttf`.
No ChoralRoot bitmap tables or UI assets are included.

`device/tools/gen_aquora_font.py` produces the packed four-bit bitmap atlas with
native optical sizes, tabular figures, optical weight choices, slash-zero,
barred I and footed l. The modified bitmap family is named AQUORA Sans.
The unchanged vector source keeps its upstream family name. Pillow/FreeType
is build-time only; the firmware uses the existing bounded bitmap renderer.
