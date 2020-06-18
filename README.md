Planetary
==

2020-Fixup

You can open this in xcode 11 though and it should work out of the box by building and running on a device (simulator wont work as it requires apple music subscription with some albums added to your collection)

Submodules are still enabled in the repo (I shouldremove them) but I copied in the source instead to make it easier to change and get working.

General updates include:
Update for 64bit architecture
Updates to project file and build settings
Update to make work with default clang compiler
Update to a compatible cinder with some tweaks
Update boost
(cinder/boost combination is a bit frankenstein)
Rotation lock to landscape to avoid deprecated setStatusBar code
Touch fixes
General things to stop crashes (including some threading :/)
Some hacks and fixes to make iPhone compile and build and look 'ok'

Rendering/textures etc is the original bloom version.  opengl is still deprecated on apple platforms, but since this code uses glow effects, on modern devices there is no need for any fancy stuff and it hums along quite nicely.  Even though it can run on iOS 9 devices+ I recomend running on as new hardware as possible as some optimisations (loading) was removed to facilitate getting it to work.


See also
--

* [Planetary: collecting and preserving code as a living object](https://www.cooperhewitt.org/object-of-the-day/2013/08/26/planetary-collecting-and-preserving-code-living-object) (Cooper-Hewitt Object of the Day weblog)
* [Planetary object page on the Cooper-Hewitt collections website](http://collection.cooperhewitt.org/objects/35520989/)
* [Planetary extras](https://github.com/cooperhewitt/PlanetaryExtras)
