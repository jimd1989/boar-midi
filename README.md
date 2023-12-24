# boar-midi

Convert MIDI on/off messages to [boar](https://github.com/jimd1989/boar) commands, or vice/versa.

## Build

    cc boar-midi.c -lsndio -o boar-midi
    cc midi-boar.c -lsndio -o midi-boar

Requires sndio, which comes installed by default on OpenBSD. Nothing else is supported.

## Usage

### MIDI → Boar

To read MIDI data from a port into a `boar` instance, run

    midi-boar <-c n|-delay n> | boar

The program will listen to all channels at `midi/0` by default. Set `MIDIDEVICE` environment variable to read from something else.

Input will always go to stdout. Redirect using standard Unix conventions.

Will read from all 16 MIDI channels by default. To filter for a specific channel, use the `-c n` argument, where `n` is the channel number. Run multiple instances of `midi-boar` to read from multiple specific channels; sndio's handling of the MIDI port is efficient enough to support many readers.

Some MIDI sequencers send an off signal at the same time as their on signal. If the instrument further down the pipe is having trouble with this, you can delay the printing of an off signal for `n` microseconds with the `-delay n` flag.

### Boar → MIDI

While `midi-boar` has been extensively refactored to be more efficient and composable, this program hasn't. Its commands and internals are subject to change pending the author's level of motivation.

To use the `n` and `o` commands from `boar` as MIDI on and off events respectively, run

    boar -echo-notes | boar-midi <MIDI channel> <MIDI port (optional)>

The program will write to `midi/0` if a port is not specified. Velocity is ignored for now. On/off events are newline delimited exclusively, like

    n 60
    n 63
    n 67

I suspect that there are some edge cases where the parser will fail, but it's unlikely that the input will be hand-written. It should read from `boar -echo-notes` fine.

See the [boar-extras](https://github.com/jimd1989/boar-extras) package for additional helper scripts.

## Caveats

+ Other MIDI instructions are completely ignored.
