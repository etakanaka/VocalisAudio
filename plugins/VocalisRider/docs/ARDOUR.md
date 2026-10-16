# Ardour for Linux setup

Ardour was not installed/detected here. Actual scanning, embedding, automation recording and session restoration remain unverified.

1. Install with `scripts/install.sh`. Restart Ardour or rescan VST3 from its plugin manager/preferences. Search for Vocalis Rider Live and insert it on a mono or stereo vocal track.
2. Play vocal audio with plugin Off. Check Rider movement and audible gain adjustment. Keep Output at 0 initially. Adjust Spill to reject room/stage noise.
3. Open the processor's Pin Configuration window. Enable Side Chain and add audio sidechain input(s); connect the instrumental bus output to the Music input pins. Vocal track audio must connect to the main Vocal pins; output remains connected only to the track's main output. Use mono or stereo main layouts and mono/stereo music. Check the Music indicator while playing the instrumental bus.
4. Expose the plugin Rider automation lane. Set its automation state to Write or Touch and plugin mode to WRITE. Play a short passage. Inspect for recorded points. If no points appear, this host/version has not recorded the plugin-generated edits; do not rely on the fader motion as proof. Set plugin READ and the lane Play/Read for playback. Exact state labels vary with version.
5. Save the session, reopen, check all plugin parameters, sidechain connections and automation playback. This workflow needs local host verification.

CSV export retains the real Write-mode data independently of host recording. Ardour CSV automation import is not implemented. If native recording fails, retain/export the CSV for external conversion or use Off for live automatic riding. This limitation is explicit; no fake automation lane or file is generated.

References: [Ardour manual](https://manual.ardour.org/ardourmanual.html), [plugin manager](https://manual.ardour.org/working-with-plugins/plugin-manager/). The manual describes Side Chain and Add Sidechain Input in the Pin Configuration window.
