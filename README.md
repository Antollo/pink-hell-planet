# Pink Hell Planet Project

## Building

1. Clone with:

   ```
   git clone --recurse-submodules <url>
   ```

2. Download glad and assets, setup config

   Run `init.sh` or `init.ps1` or manually:

   * Download `glad` from [here](https://onedrive.live.com/download?cid=F7B2E5ED214ED032&resid=F7B2E5ED214ED032%214910&authkey=ANa6TuYeKnGai4A)
     and unpack to `thirdparty/glad/glad/` (temporary solution)

   * Download and unpack models from (washing machine) [here](https://drive.google.com/file/d/1Iqd9X8vdOz4EeJjR4xL109-a1jnuhWYn)
    and (penguin) [here](https://onedrive.live.com/download?cid=F7B2E5ED214ED032&resid=F7B2E5ED214ED032%214909&authkey=ADt8ZYAVrLtcSyY)
    to `bin/`

   * Download and unpack terrain textures from [here](https://onedrive.live.com/download?cid=F7B2E5ED214ED032&resid=F7B2E5ED214ED032%214929&authkey=AMzTuQnU0EVMqOo) to `bin/`

   * Copy `src/shaders/` folder to `bin/`

   * Copy `src/config/config.default.json` to `bin/config.json`. You can manually change `graphics` to `low`, will be possible from gui in future

3. Install dependencies

   On ubuntu install packeges

   ```
   sudo apt install libgl1-mesa-dev libglx-dev libopengl-dev libbz2-dev libpng-dev libasound2 libasound2-dev libjack-dev libxcursor-dev libxrandr-dev libxi-dev
   ```

   [//]: # (todo: arch)

4. Compile with:

   - Windows

   ```
   cmake -A Win32 -B build
   cmake --build build --config Release
   ```

   - Linux

   ```
   cmake -B build
   cmake --build build --config Release
   ```

## Sounds and music

- [www.fesliyanstudios.com/royalty-free-music/download/retro-platforming/454](https://www.fesliyanstudios.com/royalty-free-music/download/retro-platforming/454)
- [freesound.org/people/HenryRichard/sounds/448274/](https://freesound.org/people/HenryRichard/sounds/448274/)
- [freesound.org/people/EVRetro/sounds/533034/](https://freesound.org/people/EVRetro/sounds/533034/)
- [freesound.org/people/Robinhood76/sounds/273332/](https://freesound.org/people/Robinhood76/sounds/273332/)
- [freesound.org/people/cydon/sounds/268557/](https://freesound.org/people/cydon/sounds/268557/)
- [freesound.org/people/FilmmakersManual/sounds/522464/](https://freesound.org/people/FilmmakersManual/sounds/522464/)
