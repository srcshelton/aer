&aelig;r
==

> Audio hardware is standard; even without extensions they can support
> low-latency (3 ms input-to-output) audio streams.  24-bit ADAT 8-channel
> optical ports are built-in, along with S/PDIF or AES/EBU optical and coaxial
> ports.  This makes the Octane into a respectable digital audio workstation.
> 
> &mdash; <cite>Wikipedia, "[SGI Octane<sup>2</sup>](http://en.wikipedia.org/wiki/SGI_Octane2#Audio_subsystem)"</cite>

... unfortunately, the IRIX audio subsystem was also only designed to be used
by only a single process at once, and so there is no native multiplexer or
remixer to cope with any discrepancy between different concurrent audio
streams.  This can be an issue if, say, you're playing music when a web page
loads FlashPlayer - which suddenly halves the playback frequency.

&aelig;r is an elegant solution to this problem - it will confirm that the
audio system can accept the specified sample-rate or 44.1kHz, subscribe to
audio system notifications, and then `select()`s on audio system `RATE` events,
thereby making the minimum possible system impact.  However, as soon as the
sample rate is changed by another source, &aelig;r instantly awakes and reverts
the alteration - ensuring uninterrupted playback at the correct rate.

Well written applications may even detect that they did not yet the sample rate
they requested, and then automatically up-mix their output to give accurate
playback even at higher sample rates.

Binaries built for several SGI platforms with
[MIPSpro](http://www.nekochan.net/wiki/MIPSpro) can be found in the bin
sub-directory - the only difference between these files are the optimisation
options specified at compile-time, although these may in turn have pulled-in
processor-specific fixes and work-arounds.
