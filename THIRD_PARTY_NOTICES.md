# Third-Party Notices

## Course game engine and viewer

The C++ engine sources (`Action`, `Board`, `Game`, `Main`, `Player`, `Registry`,
`Settings`, `State`, `Structs`, `Utils`, `Random`, `Info`, headers and
`config/default.cnf`) and the HTML/JavaScript viewer under `Viewer/` are course
materials provided for the FIB–UPC EDA Harry Potter tournament.

They are included in this repository only to make the bundled players runnable
and verifiable from a clean clone. Redistribution rights belong to the course
authors unless explicitly granted.

## Bundled reference players

- `AIDemo.cc` and `AINull.cc` are course/reference players shipped with the
  authorized engine bundle.
- `AIBasic_player.cc` is a sanitized baseline opponent derived from the course
  `Basic_player.cc` template, adapted for safe invalid-direction handling.

## Custom implementation

- `AIGen7.cc` registers player `Gen765` and contains the tournament agent
  authored for the course assignment.

## jQuery / jQuery UI (viewer assets)

The viewer includes minified jQuery 1.7.1 and jQuery UI 1.8.18 assets under
`Viewer/js/` and `Viewer/css/`. These are third-party libraries used only by the
local replay viewer.
