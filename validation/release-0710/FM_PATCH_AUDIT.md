# Pitched instrument FM audit

All patches use a sine carrier modulated by a sine operator. Patch IDs and family envelopes are stable across the v0.7.9 boundary. IDs 0–23 moved from harmonic tables to phase modulation. IDs 24–62 retain their operator values and now scale index by voice velocity. Depth is the profile coefficient before brightness and envelope scaling. These values describe source behavior. Audible realism, register-by-register response and worst-case native timing are open.

| ID | Patch | Family envelope | Attack s | Release s | Ratio | Depth | Envelope | Detune |
|---:|---|---|---:|---:|---:|---:|---:|---:|
| 0 | Earth Sub | Sine | .028 | .72 | 1. | .022 | .12 | 0. |
| 1 | Warm Temple Bass | Tri | .02 | .82 | 2. | .12 | .34 | 0. |
| 2 | Sacred Pulse | Hollow | .08 | 1.35 | 1. | .18 | .35 | 0. |
| 3 | Deep Halo Drone | Sine | 1.8 | 3.4 | 1. | .045 | .19 | .0006 |
| 4 | Evolving Warmth | Choir Wave | 1.0 | 3.0 | 2. | .15 | .24 | .0013 |
| 5 | Celestial Air | Hollow | 2.4 | 4.2 | 1.5 | .11 | .52 | .002 |
| 6 | Felt Piano | Pluck | .004 | 1.25 | 2. | .17 | .71 | 0. |
| 7 | Soft Grand | Pluck | .004 | 1.85 | 3.004 | .24 | .83 | 0. |
| 8 | Glass Piano | Pluck | .003 | 2.8 | 3.79 | .29 | .86 | 0. |
| 9 | Nylon Guitar | Pluck | .008 | .85 | 2. | .22 | .81 | 0. |
| 10 | Harp Strings | Pluck | .006 | 1.50 | 3. | .18 | .72 | 0. |
| 11 | Soft Dulcimer | Pluck | .007 | 1.85 | 4.01 | .26 | .83 | 0. |
| 12 | Breath Flute | Sine | .18 | 1.0 | 1. | .08 | .33 | 0. |
| 13 | Wooden Flute | Hollow | .12 | 1.1 | 2. | .10 | .28 | 0. |
| 14 | Bamboo Air | Sine | .34 | 1.7 | 3. | .064 | .20 | 0. |
| 15 | English Horn | Reed | .22 | 1.4 | 1. | .31 | .32 | 0. |
| 16 | Warm Clarinet | Reed | .11 | 1.1 | 2. | .28 | .33 | 0. |
| 17 | Crystal Bowl | Hollow | .004 | 4.6 | 2.73 | .26 | .22 | 0. |
| 18 | Silver Bell | Hollow | .003 | 3.3 | 3.91 | .36 | .57 | 0. |
| 19 | Angelic Voices | Choir Wave | .80 | 3.8 | 2. | .17 | .28 | .0018 |
| 20 | Chamber Strings | Reed | .65 | 2.8 | 1. | .25 | .29 | .0023 |
| 21 | Viola Mist | Reed | .45 | 2.4 | 2. | .17 | .29 | .0011 |
| 22 | Ocean Glass | Hollow | 1.2 | 4.5 | 1.4142 | .16 | .31 | .001 |
| 23 | Linen Pad | Choir Wave | 1.5 | 3.9 | 2. | .08 | .22 | .0025 |
| 24 | Moon Glass Keys | Sine | .006 | 2.2 | 3.01 | .17 | .80 | .000 |
| 25 | Halo Choir FM | Sine | 1.1 | 3.7 | 2. | .15 | .22 | .003 |
| 26 | Silk Harmonics | Sine | 1.8 | 4.0 | 1.4142 | .14 | .30 | .001 |
| 27 | Rain Harp | Pluck | .009 | 1.7 | 2.75 | .22 | .85 | .000 |
| 28 | Deep Resonant Bass | Sine | .028 | .86 | 1. | .12 | .38 | .000 |
| 29 | Velvet Reed FM | Sine | .19 | 1.5 | 3. | .18 | .46 | .002 |
| 30 | Pure Sub FM | Sine | .018 | .70 | 1. | .035 | .90 | 0. |
| 31 | Rounded Ember FM | Sine | .035 | 1.1 | 1.001 | .12 | .50 | 0. |
| 32 | Wood Resonance FM | Sine | .012 | .85 | 2. | .26 | .85 | .002 |
| 33 | Harmonic Root FM | Sine | .045 | 1.4 | 3. | .29 | .35 | 0. |
| 34 | Plucked Earth FM | Pluck | .004 | .46 | 2.5 | .40 | .97 | 0. |
| 35 | Cinema Depth FM | Sine | .32 | 2.6 | .5 | .22 | .18 | .001 |
| 36 | Nylon Resonance FM | Pluck | .006 | .78 | 1. | .32 | .96 | 0. |
| 37 | Steel String FM | Pluck | .003 | 1.25 | 3. | .47 | .98 | .001 |
| 38 | Palm Mute FM | Pluck | .002 | .23 | 2. | .25 | .995 | 0. |
| 39 | Chorus Guitar FM | Sine | .018 | 1.35 | 1.002 | .19 | .60 | .003 |
| 40 | Dusk Harp FM | Pluck | .005 | 1.65 | 2. | .23 | .94 | 0. |
| 41 | Sunlit Harp FM | Pluck | .003 | 2.05 | 3. | .42 | .97 | 0. |
| 42 | Ripple Harp FM | Pluck | .008 | .91 | 2.5 | .31 | .985 | .001 |
| 43 | Air Flute FM | Sine | .14 | .65 | 1. | .055 | .30 | .001 |
| 44 | Bamboo Legato FM | Sine | .075 | .92 | 2. | .09 | .40 | 0. |
| 45 | Alto Reed FM | Sine | .06 | .72 | 1. | .40 | .54 | .001 |
| 46 | Tenor Velvet FM | Sine | .095 | .88 | 1. | .28 | .42 | 0. |
| 47 | Breathy Sax FM | Sine | .13 | 1.15 | 3. | .32 | .38 | .002 |
| 48 | Alto Saxophone | Sine | .037 | .42 | 1. | .30 | .25 | 0. |
| 49 | Tenor Saxophone | Sine | .058 | .53 | 1. | .24 | .38 | 0. |
| 50 | Chamber Clarinet | Sine | .045 | .38 | 2. | .21 | .18 | 0. |
| 51 | Oboe | Sine | .065 | .48 | 1. | .43 | .16 | 0. |
| 52 | Bassoon | Sine | .071 | .55 | 2. | .29 | .35 | 0. |
| 53 | Concert Flute | Sine | .075 | .46 | 1. | .065 | .55 | 0. |
| 54 | Alto Flute | Sine | .125 | .64 | 2. | .045 | .65 | 0. |
| 55 | Piccolo | Sine | .043 | .28 | 1. | .11 | .42 | 0. |
| 56 | Soprano Ah Choir | Sine | .62 | 2.9 | 2. | .20 | .50 | .003 |
| 57 | Male Ooh Choir | Sine | .84 | 2.5 | 1. | .135 | .34 | .001 |
| 58 | Mixed Vowel Choir | Sine | .55 | 3.2 | 2. | .25 | .46 | .002 |
| 59 | Angelic Air Choir | Sine | 1.1 | 4.0 | 3. | .12 | .50 | .003 |
| 60 | Pizzicato Cello | Pluck | .012 | .24 | 2. | .31 | .96 | 0. |
| 61 | Rosewood Mallet | Pluck | .003 | .45 | 3.99 | .27 | .99 | 0. |
| 62 | Kalimba Tines | Pluck | .002 | .62 | 5.95 | .33 | .99 | 0. |
