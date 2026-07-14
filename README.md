# Elicit

**Elicit** — a Live screen magnifier and color picker, written using the Enlightenment Foundation Libraries. Designed for UI designers and developers.

![Elicit screenshot](https://github.com/user-attachments/assets/4213a84c-9355-440a-b194-f4c3130bd18a)

---

## Current Features

- Live X11 screen capture
- Color picker
- Overlay grid 
- RGB, Hex and Color Class output
- Hex and RGB entries drive sliders and the color swatch
- Zoom (1 to 12) factor
- Stores last used values (zoom level, RGB/Hex values) upon exit

---

## Dependencies

- EFL (Evas, Elementary, Eet)
- Meson + Ninja
- No external dependencies beyond the EFL itself

---

## Building

```bash
git clone https://github.com/ttocyob/elicit.git
cd elicit
meson setup build
ninja -C build
sudo ninja -C build install
```

---

## Missing Features

- Save swatch color to a palette directory

---

## Acknowledgements

Elicit was originally coined and written by Brian Mattern (rephorm) 21+ years ago. It was his means to learn the EFL. He later ported Elicit to [GTK/Python](https://github.com/rephorm/elicit-gtk), which includes a lot of features that this version does not have. The original EFL/Edje version can be found [here](https://github.com/rephorm/elicit).

---

## License

BSD-2-Clause license 
