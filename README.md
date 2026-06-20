# Elicit

**Elicit** — a Live screen magnifier and color picker, written using the Enlightenment Foundation Libraries. Designed for UI designers and developers.

![Elicit screenshot](https://github.com/user-attachments/assets/94a9778c-6f18-40fd-9053-c9645dfe38be)

---

## Current Features

- Live X11 screen capture
- Color picker
- RGB/Hex based on color output
- Zoom (1 to 10) factor
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

- Square grid overlay based on zoom factor (Grid toggle UI is present, not yet implemented)
- Hex entry manual update (copy/paste)
- Update the color swatch based on RGB values
- Save swatch color to a palette directory
- ELM_SCALE: live scale based on Enlightenment's scale factor

---

## Acknowledgements

Elicit was originally coined and written by Brian Mattern (rephorm) almost 20+ years ago. It was his means to learn the EFL. He later ported Elicit to [GTK/Python](https://github.com/rephorm/elicit-gtk), which includes a lot of features that this version does not have. The original EFL/Edje version can be found [here](https://github.com/rephorm/elicit).

---

## License

BSD-2-Clause license 
