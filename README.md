# R387 AI Decorator — Geode source prototype

This is a **starter project**, not a finished working `.geode` mod. It includes
Geode project metadata, a CMake build file, an editor-hook entry point, and a
prototype decorator popup / deterministic local decoration-plan generator.

## Current status

- [x] Geode C++ project layout and metadata
- [x] Prototype editor entry point and decorator popup
- [x] Object-limit/detail/style settings represented in the source
- [x] Local deterministic decoration recipe scaffold
- [ ] Read actual selected editor objects and derive selection bounds
- [ ] Insert generated `GameObject`s through the installed GD/Geode editor APIs
- [ ] Ensure insertion participates in Geometry Dash undo/redo
- [ ] Real natural-language AI backend
- [ ] Compile/test against the exact installed Geode + Geometry Dash versions

**Do not expect this source to place decorations yet.** The version-specific
editor selection and object-insertion adapter is deliberately called out rather
than claiming it works without compilation and in-game testing. The Geode C++
APIs and `EditorUI::init` signatures can change across SDK/game versions.

## Build on Windows

1. Install Geometry Dash and Geode.
2. Install the Geode SDK / CLI following the official docs:
   https://docs.geode-sdk.org/getting-started/
3. Open a terminal in this folder.
4. Run `geode build` (or `geode build --ninja` if your setup requires Ninja).
5. If the build succeeds, locate the `.geode` package in the build output.
   Install it through Geode's mod installer. The build has **not** been run
   in this environment because it does not have your Windows Geode SDK.

## AI backend design

The mod should send a compact request to a separate backend, for example:

```json
{
  "prompt": "Ancient stone wall covered in moss, with a shattered window",
  "style": "Realistic",
  "detail": 75,
  "maxObjects": 750,
  "selectionBounds": {"x": 0, "y": 0, "width": 120, "height": 150}
}
```

The backend should return a validated *decoration plan*, not executable code:
object IDs, positions relative to selection bounds, scale, rotation, and optional
color/group parameters. The mod must clamp all values, reject unknown IDs,
enforce the object limit, preview the plan, and only then insert objects.

For arbitrary natural-language designs, use an LLM or vision-language model on a
server. Keep API keys on that server, not in the `.geode` file. The backend can
use a schema-constrained response so the mod only receives structured JSON.
A local template generator can work offline but will not truly understand
arbitrary descriptions.

## Undo safety

The final placement adapter should use the same editor object-add path used by
Geometry Dash itself, keep a list of inserted objects for rollback, and expose
an explicit preview/confirm step. Do not mutate level save strings directly.
Validate undo/redo and save/reopen behavior in a disposable test level first.

## Important

This repository is a prototype scaffold. It is not yet a compiled or tested
`.geode` binary. Build against the exact SDK version installed on your PC before
relying on it in a real level.
