# REAPER for Linux setup

Host execution was unavailable in this environment. These steps are based on REAPER's documented routing and automation model and must be verified on your host version.

1. Run `scripts/install.sh`, then Preferences → Plug-ins → VST. Include `~/.vst3` in search paths if needed and rescan. Insert **VST3: Vocalis Rider Live (Vocalis Audio)** on the vocal track.
2. Main vocal input/output supports mono or stereo. REAPER commonly runs FX in stereo; both channels receive linked gain. Start with a Default preset, Off, Slow, Output 0. Play actual audio and check Input/Ride and Rider motion.
3. For music adaptation, create a music bus without the vocal in it. Set vocal track channels to four, and send music bus audio 1/2 to vocal-track channels 3/4. Open the plugin pin connector: main Vocal L/R → 1/2, Music L/R → 3/4, output → 1/2. Enable/request the auxiliary bus as needed. For VST3, **Request VST3 bus channel count** may be needed to keep the main bus stereo. Avoid routing vocal back to its own music sidechain. Music indicator should respond to music; audio from channels 3/4 must not leak into vocal output.
4. To attempt native automation recording, show/arm only the plugin **Rider** parameter envelope, set the track/envelope to Write or Touch, select plugin WRITE, and play. Confirm actual points appear; a moving GUI alone is not evidence of recording. Stop and select plugin READ; put the track in Read. Replay and verify Rider follows the recorded envelope. Off resumes internal automatic riding.
5. Save the project, close it, reopen and inspect the preset label/parameters, routing, and automation. This step remains unverified locally.

## Explicit CSV workflow

Use this if native host recording does not retain Rider points.

1. Select plugin Off, stop playback, press CLEAR, select WRITE and play one uninterrupted pass. Set host automation to Read/Trim to avoid recording unintended parameters. Stop, select Off, and press EXPORT to save CSV. Keep the host/plugin open until exported.
2. In REAPER Actions → Show action list → ReaScript → Load, load `scripts/Import-Vocalis-Rider.lua`.
3. Select the vocal track containing exactly one instance of Vocalis Rider Live; run the script and choose the CSV. The script creates that instance's Rider envelope, appends linear points at captured timeline seconds, sets plugin READ and track Read. Existing envelope points are preserved; use a fresh envelope for a clean import, or undo the action if needed.
4. Inspect the envelope before playback. The script identifies the instance and Rider parameter by name. Multiple plugin instances require separate tracks for this importer. Capture with seeks/loops can have duplicate times. No automatic time offset is applied.

The importer has not been executed inside REAPER in this environment. It is supplied as an explicit, reviewable fallback, not a claim of tested host automation support.

References: [REAPER user guide](https://www.reaper.fm/userguide.php), [VST3 routing compatibility notes in the official guide](https://dlz.reaper.fm/userguide/ReaperUserGuide720c.pdf), [ReaScript API](https://www.reaper.fm/sdk/reascript/reascripthelp.html).
