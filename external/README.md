# Setting up dependencies

The project currently depends on
legacy [Discord Game SDK v3.2.1](https://discord.com/developers/docs/developer-tools/game-sdk).

To set it up, download the archive from the [SDK](https://discord.com/developers/docs/developer-tools/game-sdk) page and
copy its contents to `discord_game_sdk` directory.

It should look something like this:

```
├───external
│   ├───discord_game_sdk
│   │   ├───cpp
│   │   ├───lib
│   │   ├───...
│   ├───CMakeLists.txt
│   ├───README.md - this readme you are looking at
├───...
```