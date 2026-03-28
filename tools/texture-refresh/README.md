# Titan Texture Refresh

This directory contains the conservative texture refresh pipeline for the
`textures/titan/*` pilot pass.

The goal is not to redesign materials. The script extracts the original shipped
textures from `gamedata/base/data-1.1.0.pk3`, upscales them, applies a mild
contrast/detail pass, and writes loose overrides into `assets/textures/titan`.

The first pass targets these shared wall/structure materials:

- `basic_panel_b.jpg`
- `dark_metal.jpg`
- `dark_metal_2.jpg`
- `multipanels.jpg`
- `multipanels_2.jpg`
- `support_1.jpg`
- `support_2.jpg`
- `grate.jpg`
- `grate_b.jpg`
- `grate_edge.jpg`

Outputs:

- `reference/titan-originals/`: extracted shipped sources
- `reference/titan-refresh-board.jpg`: side-by-side original/refresh preview
- `assets/textures/titan/`: runtime overrides copied into the staged build

Run:

```powershell
powershell -ExecutionPolicy Bypass -File tools/texture-refresh/Refresh-TitanTextures.ps1
```
