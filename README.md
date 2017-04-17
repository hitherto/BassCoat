Description
===========

The BassCoat is a project I built for Burning Man 2016. It consists of a fake-fur coat studded with LEDs which pulse in time to any
nearby source of music.

Alternatively, it can be set to various other "modes" which are non-reactive.

Here Be Dragons
===============

This code is as complete as I could get it while also packing for Burning Man, assisting with camp tasks, helping out with long woodshop hours for [The Śiṣya](https://www.indiegogo.com/projects/the-si-ya-burning-man-2016-art-experience#/), and planning a massive (intentional) on-Playa fire.

In the months since, I've moved house twice, taken on a mortgage and a demanding job.

So, the code is frozen exactly where it was the day before I left for Black Rock City. There are parts of it I'm not super-happy with. But it did its job admirably; the most fragile part of the project turned out to be the soldered links to the LEDs which, between flexing inside a jacket and coping with desert dust, came loose several times.

Parts
=====

* 1 Cheapish faux-fur jacket (I purchased [this one](https://www.amazon.com/gp/product/B00P2BWKFA/) from Amazon, but anything similar can work)
* 1 [Adafruit Flora](https://www.adafruit.com/product/659)
* 1 [Sparkfun Sound Detector Board](https://www.sparkfun.com/products/12642)
* 1 [50pc strand of WS2811 RGB LEDs](https://www.amazon.com/gp/product/B00B4UKG2W/) (see "Lights" in "Development Considerations" below for more info on this)
* 2 [6Ah Lithium Ion Batteries](https://www.sparkfun.com/products/13856) (see "Power" in "Development Considerations" below for more info on this)
* 1 [Lilypad Button Board](https://www.sparkfun.com/products/8776)
* [Conductive thread](https://www.sparkfun.com/products/10867)
* Solder
* Sewing thread which matches the color of your coat; quite a bit of it
* Velcro strips

**Note! If you're planning to build this coat for Burning Man, awesome! Just buy the parts before July, when everyone else will scavenge sites like Adafruit, DigiKey and Sparkfun dry looking for components for similar projects**

Tools
=====

* Soldering Iron
* Seam Ripper
* Sewing Needles
* Hot Glue Gun

Development Considerations
==========================

Before we dig into the nuts-and-bolts, I wanted to cover a couple of issues I worked through as I was putting the jacket together. This is partly in the spirit of being as informative as possible; partly to save anyone else from following down the same dead-ends that I did during development.

Lights
------

There are a plethora of addressable LEDs on the market nowadays, most of them relying on the WS2811 or WS2812 controllers. When I first started building the coat, my goal was to use Adafruit's gorgeous [Flora NeoPixels](https://www.adafruit.com/product/1260), connected together with conductive thread. I knew that resistance might be an issue here -- the thread isn't as conductive as most wires, and I was going to be covering a lot of area. But the scant information on exactly how resistive the thread was led me to believe I could get away with it. 

Nope!

For the record, once you've covered a distance of around 2 feet from source (and around 15 LEDs), power and control signals are going to start having a hard time reaching your LEDs.

I also had a [NeoPixel Strip](https://www.adafruit.com/product/1260) to hand (it was highly useful during initial code/beat-detection testing), but there was no good way to secure it to the inside of the coat facing out, and the LEDs were too close together.

Eventually I stumbled upon the solution listed in "Parts" above -- a stranded set of 50 LEDs with decent spacing between them, WS2811-controlled. They were heavier and a little more unwieldy than the NeoPixels, but in the end provided good light and a reliable connection.

Power
-----

Those are big batteries listed in the "parts" section. The biggest that are easily available, in fact. The size was based on several considerations:

1. Potentially I'd be taking this coat out as my primary source of Playa illumination (and dancefloor fun) all night.
2. At maximum brightness, most LEDs draw around 60mA.
3. With 50 LEDs, the draw would be (potentially) 3000mA. Add in the (small) power draw from the Flora board, and I couldn't necessarily expect more than 2 hours' life out of a 6Ah battery. Hence the secondary backup.

With more time, I might have got more comfortable trying to run the board via its USB connector using more conventional backup batteries (for one thing, the connectors on these things are fiddly). But ultimately, these batteries worked just fine!

Building the Coat
=================

This section would be _far_ more awesome with images and diagrams. I'll try to find time to put some together sometime.

Connections
-----------

With the battery going into the Flora's power port (or power coming from USB), these are the connections you'll need to make:

Soldered:

* LEDs Black wire -> Flora GND
* LEDs Red wire -> Flora VBATT
* LEDs White wire -> Flora D6

**Note! WS2811-powered lights are directional. make _really_ sure you're connecting the "front" end to the Flora before you solder.**

Sewn:

* Sound Detector VCC -> Flora 3.3v
* Sound Detector GND -> Flora GND
* Sound Detector Envelope -> Flora D9
* Button Pad "S" -> Flora "RX" (pin 0)
* Button Pad "-" -> Flora GND

Despite conductive thread not working for light control, it was perfect for the connections between the soundboard, Flora and button pad; providing a reliable connection which could flex with the coat. As I was situating the electronics in the coat (see below for placement info), I sewed the connections into place.

**Note! It's best to use a separate GND port for each ground connection.** 

Sewing
------

The coat works best if the lights are sewn into place between the lining and the outside of the jacket. To achieve this, you'll need to open up the lining along its bottom and side edges using a seam ripper, then turn the lining inside out so you can work with the inside of the main outer ("fur") layer of fabric.

I secured the main electronics (button, Flora, sound detector board) in the bottom corner of the left front panel (from the wearer's perspective).

* The Flora can be secured just by loop-stitching it with the pin-outs that you won't be using (D10, D12, SCL, TX)
* The Sound Board can be secured by loop-stitching through the 2 holes on the board; I added a little extra reinforcement on the other edge with a hot glue gun. I also poked a small hole in the fabric for the microphone to poke through, and reinforced the hole's edges with hot glue so the coat wouldn't unravel.
* The lily button board was stitched into the hem to make it relatively easy to track down by touch when wearing the coat.

Having secured the electronics and created the conductive-thread paths for shorter connections, I now needed to secure the rest of the lights into the coat. This was by far the most time-consuming and tedious part of the entire construction.

I decided that, starting at the bottom of one "front" panel, I wanted a fairly even distribution of lights. Because they were a single strand, this meant...

1. Sewing the first 13 lights in a snaking pattern leading up the right front panel.
2. Taking the strand under the right armpit and snaking 14 lights around the top of the back panel.
3. Taking the strand under the left armit and snaking 11 lights around the left front panel, ending at the bottom of the coat.
4. Distributing the remaining 12 lights across the lower portion of the back panel.

(Yes, these proportions aren't quite balanced; it doesn't make a noticeable difference on the finished coat. Some of the "modes" in the code (see below) are hard-coded to these distributions; you might need to alter them if you change the distribution of lights).

For each light, I secured them with multiple heavy loops of sewing thread around the base of the LED housing. 

Once the lights were all secured, I sewed the lining back into the coat, leaving a flap about 3" high and 4" wide where the electronics were, secured with Velcro (for access), and some reinforcement stitching to provide a snug "pocket" for the battery to sit in.

Voila! One BassCoat (and about 5 hours of your life if you're reasonably quick with a needle).

Modes
=====

The concept of varying "modes" came quite late in the project, and ended up being achieved with a simple button that cycles through different states. 

The exact function of each mode can be difficult to follow in the code, since initial setup happens in the `buttonPushed()` interrupt, but other more complex handling takes place elsewhere for some of the dynamic modes.

As presented in the current code, here's what each mode does...

0. Rainbow - Cycles through a "rainbow" of colors (non-reactive)
1. Dancefloor Rainbow - Cycles through a "rainbow" of colors while "pulsing" to any nearby bass notes
2. Thundercloud - Lights are a dim white; each time a bass note hits a random light will shine brightly then slowly fade back over the next 4 cycles
3. Glow - Lights gently pulse, unreactive to music
4. Street Legal - White lights in front, red lights in back
5. Mariner - Green lights starboard, red lights port
6. White - Static white lights
7. Blue - Static blue lights

Changing these modes or adding more of your own is really just a matter of coming up with creative ideas and executing them in code! Note that, in the end, only modes 1 and 2 actually use the bass-detection feature; the others are mostly there to provide extra utility (keeping the coat fun-but-less-distracting; "Street Legal" mode is _great_ for biking around Black Rock City at night!)

Dependencies
============

The code relies on the [FastLED](http://fastled.io/) Arduino library. As long as your Arduino IDE has access to it, you should be good to go.

Acknowledgments
===============

BassCoat wouldn't have happened without my dear friend Melissa's guidance, assistance and patient teaching regarding tearing apart and re-sewing garments.

For some early experiments in wearable LED-wrangling, Adafruit's [Ampli-tie](https://learn.adafruit.com/led-ampli-tie/the-code) project helped greatly.	

For getting my head around the best approach to beat detection on a tiny micoprocessor, [Damien Peckett's awesome tutorial](http://damian.pecke.tt/beat-detection-on-the-arduino) was invaluable.

For digging into debouncing the mode-change button, [this thread](http://forum.arduino.cc/index.php?topic=45000.0) on the Arduino forum was a great resource.

License and Author
==================

Author:: Simon Batistoni (<github@hitherto.net>)

Copyright:: 2016 Simon Batistoni

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.