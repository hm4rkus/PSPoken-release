# Changelog

## 0.9.0-alpha

PSP only for now: the PS Vita build of 0.9.0 is coming soon (0.8.0-alpha stays the Vita version until then).

- **Playback speed for each book**, ×0.05 to ×3.00 in steps of 0.05, with the pitch kept: ↑ / ↓ in the player (a readout shows the speed), or △ → Speed in the book menu. The time left follows the speed. Uses the sonic library (Apache 2.0).
- **The CPU clock follows playback:** while a book plays, the PSP raises its clock (up to 333 MHz) only when a fast speed needs it, and lowers it again when it doesn't. HE-AAC books keep up at ×3.00; full-cast stereo books may have short gaps near ×3.00.
- **The legends** show L R ±30s as one entry, to make room for ↕ Speed, and △ is "Options".

## 0.8.0-alpha

The first community test release, for the PSP and the native PS Vita app. So far, I've been its only user; compatibility feedback is welcome.

- **The library:** a list with covers, sorted by recently played, title or author; an Authors tab and author / series shortcuts; each book's progress; a menu per book (Play, Play from Beginning, Mark as finished / not finished, Information).
- **The player:** cover, chapter, chapter progress and time left; skips of 10 to 60 s; scrubbing with L / R held or the analog stick; previous and next chapter; a chapter list.
- **Your place in every book**, saved so you can pick up where you stopped.
- **Formats:** DRM-free `.m4b` with AAC-LC on every model, and HE-AAC v1 at full quality on the PSP Go, the PSP-3000 and the PS Vita.
- **Screen off with the book playing:** HOLD, the PSP Go's slider, and SELECT held on the PS Vita; the screen also dims and turns off by itself while a book plays.
- **Headphone buttons:** Bluetooth media controls on the PSP Go and PS Vita, including with the screen off; Sony’s wired remote support on PSP-1000 to 3000 awaits hardware testing.
- **Looks:** the console’s system fonts and familiar button symbols, interface sounds, two background styles, 40 color pairs in the same or neighboring hues, Small / Normal / Large text.
- **Optional battery percentage**, off by default; the now-playing title adapts to the space beside it.
- **Accented filenames on PSP:** fall back to FAT short paths when firmware truncates the long name.

See the [README](README.md#alpha-limitations) for tested devices and alpha limitations.
