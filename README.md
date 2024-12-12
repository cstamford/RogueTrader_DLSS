Adds DLSS to Warhammer 40k: RT. [Comparisons](https://imgsli.com/MzI2NzMw/2/5)

# Installation

1. Extract `EnhancedGraphics_Native.zip` to the game directory, such that: `../Warhammer 40,000 Rogue Trader/EnhancedGraphics_Native.asi` (+ `nvngx_dlss.dll` and `winhttp.dll`)
2. Extract `EnhancedGraphics.zip` to the Owlcat Mods directory, such that: `%userprofile%/AppData/LocalLow/Owlcat Games/Warhammer 40000 Rogue Trader/UnityModManager/EnhancedGraphics/EnhancedGraphics.dll`
3. On the mod menu in-game (ctrl+f10 by default), open Enhanced graphics, and set the Upscaler Type to `DlssUpscaler`. Select a preset or drag the slider to make a custom preset. I recommend against messing with any of the settings below custom preset.

# Known Issues

1. [base game bug - will fix] Some minor ghosting behind cloth, as cloth isn't included in motion vectors.
2. [base game bug - will fix] Particles scale as the render resolution decreases.

# Future Plans

1. Full-resolution rendering on the character doll/inventory screen.
2. Frame generation

# Credits

1. `nvngx_dlss.dll`: NVIDIA
2. `winhttp.dll`: [Ultimate-ASI-Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases)